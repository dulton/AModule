#include "stdafx.h"
#include <process.h>
#include <MSWSock.h>
#include "../base/AModule_API.h"
#include "../io/AModule_io.h"


struct TCPServer {
	AObject  object;
	SOCKET   sock;
	HANDLE   listen_thread;
	AOption *option;

	AModule *io_module;
	long     async_tcp;
	AOption *default_bridge;

	long     io_family;
	struct TCPClient *prepare;
	LPFN_ACCEPTEX acceptex;
	AOperator sysio;
};
#define to_server(obj) container_of(obj, TCPServer, object)


enum TCPStatus {
	tcp_accept,
	tcp_client_open,
	tcp_recv_probe,
	tcp_proxy_open,
	tcp_client_output,
	tcp_proxy_input,

	tcp_bridge_open,
	tcp_bridge_input,
	tcp_bridge_output,
};
struct TCPClient {
	TCPServer *server;
	TCPStatus  status;
	SOCKET     sock;
	AObject   *client;
	AObject   *proxy;
	long       probe_type;
	long       probe_size;
	AMessage   inmsg;
	char       indata[1800];
};
#define from_inmsg(msg) container_of(msg, TCPClient, inmsg)

static void TCPClientRelease(TCPClient *client)
{
	if ((client->proxy != NULL) && (client->proxy->cancel != NULL))
		client->proxy->cancel(client->proxy, Aio_Input, NULL);
	//TRACE("%p: result = %d.\n", client, result);
	release_s(client->proxy, aobject_release, NULL);
	release_s(client->client, aobject_release, NULL);
	release_s(client->sock, closesocket, INVALID_SOCKET);
	aobject_release(&client->server->object);
	free(client);
}

static long TCPClientInmsgDone(AMessage *msg, long result);
static TCPClient* TCPClientCreate(TCPServer *server)
{
	TCPClient *client = (TCPClient*)malloc(sizeof(TCPClient));
	if (client != NULL) {
		client->server = server; aobject_addref(&server->object);
		client->status = tcp_accept;
		client->sock = INVALID_SOCKET;
		client->client = NULL;
		client->proxy = NULL;
		client->probe_type = AMsgType_Unknown;
		client->probe_size = 0;
		amsg_init(&client->inmsg, AMsgType_Unknown, NULL, 0);
		client->inmsg.done = &TCPClientInmsgDone;
	}
	return client;
}

static DWORD WINAPI TCPClientProcess(void *p)
{
	TCPClient *client = (TCPClient*)p;
	return TCPClientInmsgDone(&client->inmsg, 1);
}

static long TCPClientInmsgDone(AMessage *msg, long result)
{
	TCPClient *client = from_inmsg(msg);
	TCPServer *server = client->server;
	while (result > 0)
	{
		switch (client->status)
		{
		case tcp_accept:
			result = server->io_module->create(&client->client, NULL, NULL);
			if (result < 0)
				break;

			client->status = tcp_client_open;
			amsg_init(&client->inmsg, AMsgType_Handle, (char*)client->sock, 0);
			result = client->client->open(client->client, &client->inmsg);
			break;

		case tcp_client_open:
			client->sock = INVALID_SOCKET;
			client->status = tcp_recv_probe;
			if (client->probe_size != 0) {
				amsg_init(&client->inmsg, client->probe_type, NULL, 0);
				break;
			}

			amsg_init(&client->inmsg, AMsgType_Unknown, client->indata, sizeof(client->indata)-1);
			result = client->client->request(client->client, Aio_Output, &client->inmsg);
			break;

		case tcp_recv_probe:
			client->probe_type = client->inmsg.type;
			client->probe_size += client->inmsg.size;
			client->indata[client->probe_size] = '\0';
			amsg_init(&client->inmsg, client->probe_type, client->indata, client->probe_size);

			AModule *module;
			module = amodule_probe("proxy", client->client, &client->inmsg);
			if ((module == NULL) && (client->inmsg.size < 8))
			{
				TRACE("retry probe: %s\n", client->indata);
				amsg_init(&client->inmsg, AMsgType_Unknown,
					client->indata+client->probe_size,
					sizeof(client->indata)-1-client->probe_size);
				result = client->client->request(client->client, Aio_Output, &client->inmsg);
				break;
			}

			AOption *proxy_opt;
			if (module != NULL)
				proxy_opt = aoption_find_child(server->option, module->module_name);
			else
				proxy_opt = NULL;
			if ((module == NULL) || (proxy_opt != NULL && _stricmp(proxy_opt->value, "bridge") == 0))
			{
				if (proxy_opt == NULL)
					proxy_opt = server->default_bridge;
				if (proxy_opt == NULL) {
					result = -EFAULT;
				} else {
					result = server->io_module->create(&client->proxy, client->client, proxy_opt);
				}
				amsg_init(&client->inmsg, AMsgType_Option, (char*)proxy_opt, 0);
				client->status = tcp_bridge_open;
			}
			else
			{
				result = module->create(&client->proxy, client->client, proxy_opt);
				amsg_init(&client->inmsg, AMsgType_Object, (char*)client->client, 0);
				client->status = tcp_proxy_open;
			}
			if (result >= 0) {
				result = client->proxy->open(client->proxy, &client->inmsg);
			}
			break;

		case tcp_proxy_open:
			amsg_init(&client->inmsg, client->probe_type, client->indata, client->probe_size);

		case tcp_client_output:
			if (server->sock == INVALID_SOCKET) {
				result = -EINTR;
			} else {
				client->status = tcp_proxy_input;
				result = client->proxy->request(client->proxy, Aio_Input, &client->inmsg);
			}
			break;

		case tcp_proxy_input:
			if (client->inmsg.data != NULL) {
				amsg_init(&client->inmsg, AMsgType_Unknown, NULL, 0);
				if (!server->async_tcp) {
					QueueUserWorkItem(&TCPClientProcess, client, 0);
					return 0;
				}
			}
			result = client->proxy->request(client->proxy, Aio_Input, &client->inmsg);
			if (result < 0) {
				amsg_init(&client->inmsg, AMsgType_Unknown, client->indata, sizeof(client->indata)-1);
				result = 1;
			}
			if (result > 0) {
				client->status = tcp_client_output;
				result = client->client->request(client->client, Aio_Output, &client->inmsg);
			}
			break;

		case tcp_bridge_open:
			TCPClient *bridge;
			bridge = TCPClientCreate(server);
			if (bridge == NULL) {
				result = -ENOMEM;
				break;
			}

			bridge->status = tcp_bridge_input;
			bridge->client = client->proxy; aobject_addref(client->proxy);
			bridge->proxy = client->client; aobject_addref(client->client);
			if (server->async_tcp) {
				TCPClientProcess(bridge);
			} else {
				QueueUserWorkItem(&TCPClientProcess, bridge, 0);
			}

			amsg_init(&client->inmsg, client->probe_type, client->indata, client->probe_size);
			msg->data[msg->size] = '\0';
			OutputDebugStringA(msg->data);
			fputs(msg->data, stdout);

		case tcp_bridge_output:
			if (server->sock == INVALID_SOCKET) {
				result = -EINTR;
			} else {
				client->inmsg.type |= AMsgType_Custom;
				client->status = tcp_bridge_input;
				result = client->proxy->request(client->proxy, Aio_Input, &client->inmsg);
			}
			break;

		case tcp_bridge_input:
			amsg_init(&client->inmsg, AMsgType_Unknown, client->indata, sizeof(client->indata)-1);
			client->status = tcp_bridge_output;
			result = client->client->request(client->client, Aio_Output, &client->inmsg);
			break;

		default:
			assert(FALSE);
			result = -EACCES;
			break;
		}
	}
	if (result < 0) {
		TCPClientRelease(client);
	}
	return result;
}

static unsigned __stdcall TCPServerProcess(void *p)
{
	TCPServer *server = to_server(p);
	struct sockaddr addr;
	int addrlen;
	SOCKET sock;

	for ( ; ; ) {
		memset(&addr, 0, sizeof(addr));
		addrlen = sizeof(addr);

		sock = accept(server->sock, &addr, &addrlen);
		if (sock == INVALID_SOCKET)
			break;

		TCPClient *client = TCPClientCreate(server);
		if (client == NULL) {
			closesocket(sock);
			continue;
		}

		client->sock = sock;
		if (server->async_tcp) {
			TCPClientProcess(client);
		} else {
			QueueUserWorkItem(&TCPClientProcess, client, 0);
		}
	}
	TRACE("%p: quit.\n", &server->object);
	aobject_release(&server->object);
	return 0;
}

static void TCPServerRelease(AObject *object)
{
	TCPServer *server = to_server(object);
	release_s(server->sock, closesocket, INVALID_SOCKET);
	release_s(server->listen_thread, CloseHandle, NULL);
	release_s(server->option, aoption_release, NULL);
	release_s(server->prepare, TCPClientRelease, NULL);
	free(server);
}

static long TCPServerCreate(AObject **object, AObject *parent, AOption *option)
{
	TCPServer *server = (TCPServer*)malloc(sizeof(TCPServer));
	if (server == NULL)
		return -ENOMEM;

	extern AModule TCPServerModule;
	aobject_init(&server->object, &TCPServerModule);
	server->sock = INVALID_SOCKET;
	server->listen_thread = NULL;
	server->option = NULL;
	server->prepare = NULL;

	*object = &server->object;
	return 1;
}

static long TCPServerDoAcceptEx(TCPServer *server)
{
	server->prepare = TCPClientCreate(server);
	if (server->prepare == NULL)
		return -ENOMEM;

	server->prepare->sock = socket(server->io_family, SOCK_STREAM, IPPROTO_TCP);
	if (server->prepare->sock == INVALID_SOCKET)
		return -EFAULT;

	long result = sizeof(struct sockaddr_in)+16;
	if (server->io_family == AF_INET6)
		result = sizeof(struct sockaddr_in6)+16;

	DWORD tx = 0;
	result = server->acceptex(server->sock, server->prepare->sock, server->prepare->indata,
	                          sizeof(server->prepare->indata)-1-result*2, result, result, &tx, &server->sysio.ao_ovlp);
	if (!result) {
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return -EIO;
	}
	return 0;
}

static void TCPServerAcceptExDone(AOperator *sysop, int result)
{
	TCPServer *server = container_of(sysop, TCPServer, sysio);
	if (result > 0) {
		server->prepare->probe_type = AMsgType_Unknown;
		server->prepare->probe_size = result;
		result = setsockopt(server->prepare->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
		                    (const char *)&server->sock, sizeof(server->sock));
	}
	if (result >= 0) {
		if (server->async_tcp) {
			TCPClientProcess(server->prepare);
		} else {
			QueueUserWorkItem(&TCPClientProcess, server->prepare, 0);
		}
		server->prepare = NULL;
	} else {
		result = -WSAGetLastError();
		release_s(server->prepare, TCPClientRelease, NULL);
	}

	result = TCPServerDoAcceptEx(server);
	if (result < 0) {
		release_s(server->prepare, TCPClientRelease, NULL);
		aobject_release(&server->object);
	}
}

static long TCPServerOpen(AObject *object, AMessage *msg)
{
	if ((msg->type != AMsgType_Option)
	 || (msg->data == NULL)
	 || (msg->size != 0))
		return -EINVAL;

	TCPServer *server = to_server(object);
	release_s(server->option, aoption_release, NULL);
	server->option = aoption_clone((AOption*)msg->data);
	if (server->option == NULL)
		return -ENOMEM;

	AOption *opt = aoption_find_child(server->option, "family");
	if ((opt != NULL) && (_stricmp(opt->value, "inet6") == 0))
		server->io_family = AF_INET6;
	else
		server->io_family = AF_INET;

	opt = aoption_find_child(server->option, "port");
	if (opt == NULL)
		return -EINVAL;

	server->sock = tcp_bind(server->io_family, IPPROTO_TCP, (u_short)atoi(opt->value));
	if (server->sock == INVALID_SOCKET)
		return -EINVAL;

	long backlog = 8;
	opt = aoption_find_child(server->option, "backlog");
	if (opt != NULL)
		backlog = atol(opt->value);
	long result = listen(server->sock, backlog);
	if (result != 0)
		return -EIO;

	opt = aoption_find_child(server->option, "io");
	server->io_module = amodule_find("io", opt?opt->value:"tcp");
	server->async_tcp = (_stricmp(server->io_module->module_name, "async_tcp") == 0);
	server->default_bridge = aoption_find_child(server->option, "default_bridge");

	aobject_addref(&server->object);
	if (!server->async_tcp) {
		server->listen_thread = (HANDLE)_beginthreadex(NULL, 0, &TCPServerProcess, server, 0, NULL);
		return 1;
	}

	GUID ax_guid = WSAID_ACCEPTEX;
	DWORD tx = 0;
	result = WSAIoctl(server->sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &ax_guid, sizeof(ax_guid),
	                  &server->acceptex, sizeof(server->acceptex), &tx, NULL, NULL);
	if (result != 0)
		return -EIO;

	result = athread_bind(NULL, (HANDLE)server->sock);
	memset(&server->sysio.ao_ovlp, 0, sizeof(server->sysio.ao_ovlp));
	server->sysio.userdata = server;
	server->sysio.callback = &TCPServerAcceptExDone;

	release_s(server->prepare, TCPClientRelease, NULL);
	result = TCPServerDoAcceptEx(server);
	return (result >= 0) ? 1 : result;
}

static long TCPServerClose(AObject *object, AMessage *msg)
{
	TCPServer *server = to_server(object);
	release_s(server->sock, closesocket, INVALID_SOCKET);
	if (server->listen_thread != NULL) {
		WaitForSingleObject(server->listen_thread, INFINITE);
		CloseHandle(server->listen_thread);
		server->listen_thread = NULL;
	}
	return 1;
}

AModule TCPServerModule = {
	"server",
	"tcp_server",
	sizeof(TCPServer),
	NULL, NULL,
	&TCPServerCreate,
	&TCPServerRelease,
	NULL,
	0,

	&TCPServerOpen,
	NULL,
	NULL,
	NULL,
	NULL,
	&TCPServerClose,
};
