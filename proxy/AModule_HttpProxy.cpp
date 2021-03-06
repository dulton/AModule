#include "stdafx.h"
#include "../base/AModule_API.h"
#include "../io/AModule_io.h"


static struct HTTPRequest {
	const char *method;
	long        length;
} HTTPRequests[] = {
#define HTTPREQ(method) { method, sizeof(method)-1 }
	HTTPREQ("GET"), HTTPREQ("HEAD"), HTTPREQ("POST"), HTTPREQ("PUT"),
	HTTPREQ("DELETE"), HTTPREQ("TRACE"), HTTPREQ("OPTIONS"),
	HTTPREQ("CONNECT"), HTTPREQ("PATCH"), { NULL, 0 }
};

static long HTTPProxyProbe(AObject *object, AMessage *msg)
{
	if (msg->size < 10)
		return -1;

	for (int ix = 0; HTTPRequests[ix].method != NULL; ++ix) {
		if ((_strnicmp(msg->data,HTTPRequests[ix].method,HTTPRequests[ix].length) == 0)
		 && (msg->data[HTTPRequests[ix].length] == ' '))
			return 60;
	}
	return -1;
}

struct HTTPProxy {
	AObject   object;
	AObject  *from;
	AObject  *to;
	AOption  *option;
	AMessage *openmsg;

	AMessage  inmsg;
	char      indata[2048];

	AMessage  outmsg;
	char      outdata[2048];
};
#define to_proxy(obj) container_of(obj, HTTPProxy, object)
#define from_inmsg(msg) container_of(msg, HTTPProxy, inmsg)
#define from_outmsg(msg) container_of(msg, HTTPProxy, outmsg)

static void HTTPProxyRelease(AObject *object)
{
	HTTPProxy *proxy = to_proxy(object);
	release_s(proxy->from, aobject_release, NULL);
	release_s(proxy->to, aobject_release, NULL);
	free(proxy);
}

static long HTTPProxyCreate(AObject **object, AObject *parent, AOption *option)
{
	HTTPProxy *proxy = (HTTPProxy*)malloc(sizeof(HTTPProxy));
	if (proxy == NULL)
		return -ENOMEM;

	extern AModule HTTPProxyModule;
	aobject_init(&proxy->object, &HTTPProxyModule);
	proxy->from = parent; aobject_addref(parent);
	proxy->to = NULL;
	proxy->option = aoption_find_child(option, "io");
	*object = &proxy->object;

	long result = aobject_create(&proxy->to, &proxy->object, proxy->option, "tcp");
	return result;
}

static long HTTPProxyOutputTo(AMessage *msg, long result);
static long HTTPProxyInputFrom(AMessage *msg, long result)
{
	HTTPProxy *proxy = from_outmsg(msg);
	while (result > 0)
	{
		proxy->outmsg.done = &HTTPProxyOutputTo;
		amsg_init(&proxy->outmsg, AMsgType_Unknown, proxy->outdata, sizeof(proxy->outdata));
		result = proxy->to->request(proxy->to, Aio_Output, &proxy->outmsg);
		if (result > 0)
		{
			//proxy->outmsg.data[proxy->outmsg.size] = '\0';
			//OutputDebugStringA(proxy->outmsg.data);

			proxy->outmsg.type |= AMsgType_Custom;
			proxy->outmsg.done = &HTTPProxyInputFrom;
			result = proxy->from->request(proxy->from, Aio_Input, &proxy->outmsg);
		}
	}
	if (result != 0)
		aobject_release(&proxy->object);
	return result;
}
static long HTTPProxyOutputTo(AMessage *msg, long result)
{
	HTTPProxy *proxy = from_outmsg(msg);
	while (result > 0)
	{
		proxy->outmsg.type |= AMsgType_Custom;
		proxy->outmsg.done = &HTTPProxyInputFrom;
		result = proxy->from->request(proxy->from, Aio_Input, &proxy->outmsg);
		if (result > 0)
		{
			proxy->outmsg.done = &HTTPProxyOutputTo;
			amsg_init(&proxy->outmsg, AMsgType_Unknown, proxy->outdata, sizeof(proxy->outdata));
			result = proxy->to->request(proxy->to, Aio_Output, &proxy->outmsg);
		}
	}
	if (result != 0)
		aobject_release(&proxy->object);
	return result;
}

static DWORD WINAPI HTTPProxy_OutputTo_InputFrom(void *p)
{
	HTTPProxy *proxy = to_proxy(p);
	long result = HTTPProxyInputFrom(&proxy->outmsg, 1);
	return result;
}

static long HTTPProxyOutputFrom(AMessage *msg, long result);
static long HTTPProxyInputTo(AMessage *msg, long result)
{
	HTTPProxy *proxy = from_inmsg(msg);
	while (result > 0)
	{
		proxy->inmsg.done = &HTTPProxyOutputFrom;
		amsg_init(&proxy->inmsg, AMsgType_Unknown, proxy->indata, sizeof(proxy->indata));
		result = proxy->from->request(proxy->from, Aio_Output, &proxy->inmsg);
		if (result > 0)
		{
			proxy->inmsg.type |= AMsgType_Custom;
			proxy->inmsg.done = &HTTPProxyInputTo;
			result = proxy->to->request(proxy->to, Aio_Input, &proxy->inmsg);
		}
	}
	if (result != 0)
		aobject_release(&proxy->object);
	return result;
}

static long HTTPProxyOutputFrom(AMessage *msg, long result)
{
	HTTPProxy *proxy = from_inmsg(msg);
	while (result > 0)
	{
		//proxy->inmsg.data[proxy->inmsg.size] = '\0';
		//OutputDebugStringA(proxy->inmsg.data);

		proxy->inmsg.type |= AMsgType_Custom;
		proxy->inmsg.done = &HTTPProxyInputTo;
		result = proxy->to->request(proxy->to, Aio_Input, &proxy->inmsg);
		if (result > 0)
		{
			proxy->inmsg.done = &HTTPProxyOutputFrom;
			amsg_init(&proxy->inmsg, AMsgType_Unknown, proxy->indata, sizeof(proxy->indata));
			result = proxy->from->request(proxy->from, Aio_Output, &proxy->inmsg);
		}
	}
	if (result != 0)
		aobject_release(&proxy->object);
	return result;
}

static void HTTPProxy_OutputFrom_InputTo(HTTPProxy *proxy)
{
	AMessage *msg = proxy->openmsg;
	proxy->openmsg = NULL;

	aobject_addref(&proxy->object);
	QueueUserWorkItem(HTTPProxy_OutputTo_InputFrom, &proxy->object, 0);

	amsg_init(&proxy->inmsg, AMsgType_Unknown, proxy->indata, sizeof(proxy->indata));
	amsg_copy(&proxy->inmsg, msg->type, msg->data, msg->size);

	proxy->inmsg.data[proxy->inmsg.size] = '\0';
	OutputDebugStringA(proxy->inmsg.data);

	aobject_addref(&proxy->object);
	HTTPProxyOutputFrom(&proxy->inmsg, 1);
}

static long HTTPProxyOpenDone(AMessage *msg, long result)
{
	HTTPProxy *proxy = from_inmsg(msg);
	if (result > 0) {
		HTTPProxy_OutputFrom_InputTo(proxy);
	}
	result = proxy->openmsg->done(proxy->openmsg, result);
	return result;
}

static long HTTPProxyOpen(AObject *object, AMessage *msg)
{
	HTTPProxy *proxy = to_proxy(object);

	proxy->openmsg = msg;
	proxy->inmsg.done = &HTTPProxyOpenDone;

	amsg_init(&proxy->inmsg, AMsgType_Option, (char*)proxy->option, 0);
	long result = proxy->to->open(proxy->to, &proxy->inmsg);
	if (result > 0) {
		HTTPProxy_OutputFrom_InputTo(proxy);
	}
	return result;
}

static long HTTPProxyClose(AObject *object, AMessage *msg)
{
	HTTPProxy *proxy = to_proxy(object);
	if (proxy->from != NULL)
		proxy->from->close(proxy->from, NULL);
	if (proxy->to != NULL)
		proxy->to->close(proxy->to, NULL);
	return 1;
}

AModule HTTPProxyModule = {
	"proxy",
	"HTTPProxy",
	sizeof(HTTPProxy),
	NULL, NULL,
	&HTTPProxyCreate,
	&HTTPProxyRelease,
	&HTTPProxyProbe,
	0,
	&HTTPProxyOpen,
	NULL, NULL,
	NULL, NULL,
	&HTTPProxyClose,
};
