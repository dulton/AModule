#include "stdafx.h"
#include "../base/AModule_API.h"
#include "../base/SliceBuffer.h"
#include "../io/AModule_io.h"
#include "PvdNetCmd.h"
#include "md5.h"

typedef struct PVDClient PVDClient;
typedef struct PVDNetCmd PVDNetCmd;

struct PVDClient {
	AObject     object;
	AObject    *io;
	PVDStatus   status;
	DWORD       userid;
	BYTE        md5id;
	long        last_error;
	STRUCT_SDVR_DEVICE_EX device2;

	AMessage    outmsg;
	AMessage   *outfrom;
	SliceBuffer outbuf;
};
#define to_pvd(obj) container_of(obj, PVDClient, object);
#define from_outmsg(msg) container_of(msg, PVDClient, outmsg)

static void PVDRelease(AObject *object)
{
	PVDClient *pvd = to_pvd(object);
	release_s(pvd->io, aobject_release, NULL);
	SliceFree(&pvd->outbuf);

	free(pvd);
}

static long PVDCreate(AObject **object, AObject *parent, AOption *option)
{
	PVDClient *pvd = (PVDClient*)malloc(sizeof(PVDClient));
	if (pvd == NULL)
		return -ENOMEM;

	extern AModule PVDClientModule;
	aobject_init(&pvd->object, &PVDClientModule);

	pvd->io = NULL;
	pvd->status = pvdnet_invalid;
	pvd->userid = 0;
	pvd->md5id = 0;
	memset(&pvd->device2, 0, sizeof(pvd->device2));

	pvd->outfrom = NULL;
	SliceInit(&pvd->outbuf);

	AOption *io_opt = aoption_find_child(option, "io");
	long result = aobject_create(&pvd->io, &pvd->object, io_opt, NULL);

	*object = &pvd->object;
	return 1;//result;
}

extern long PVDTryOutput(DWORD userid, SliceBuffer *outbuf, AMessage *outmsg)
{
	outmsg->data = SliceCurPtr(outbuf);
	outmsg->size = SliceCurLen(outbuf);

	long result = PVDCmdDecode(userid, outmsg->data, outmsg->size);
	if (result < 0)
		return result;

	if ((result == 0) || (result > outmsg->size)) {
		if (max(result,1024) > SliceCapacity(outbuf)) {
			if (SliceResize(outbuf, result, 2048) < 0)
				return -ENOMEM;
			outmsg->data = SliceCurPtr(outbuf);
		}
		if (!(outmsg->type & AMsgType_Custom)) {
			return 0;
		}
		if (result == 0) {
			TRACE("%p: reset buffer(%d), drop data(%d).\n", userid, outbuf->siz, outmsg->size);
			SliceReset(outbuf);
			return 0;
		}
		result = outmsg->size;
	} else {
		outmsg->size = result;
	}
	return result;
}

static inline long PVDDoOpen(PVDClient *pvd, PVDStatus status)
{
	pvd->outmsg.type = AMsgType_Option;
	pvd->outmsg.data = (char*)aoption_find_child((AOption*)pvd->outfrom->data, "io");
	pvd->outmsg.size = 0;

	pvd->status = status;
	return pvd->io->open(pvd->io, &pvd->outmsg);
}

static inline long PVDDoClose(PVDClient *pvd, PVDStatus status)
{
	amsg_init(&pvd->outmsg, AMsgType_Unknown, NULL, 0);

	pvd->status = status;
	return pvd->io->close(pvd->io, &pvd->outmsg);
}

static inline void PVDInitInput(PVDClient *pvd, PVDStatus status, long type, long body)
{
	pvd->outmsg.type = AMsgType_Custom|type;
	pvd->outmsg.data = SliceResPtr(&pvd->outbuf);
	pvd->outmsg.size = PVDCmdEncode(pvd->userid, pvd->outmsg.data, type, body);

	pvd->status = status;
}

static inline long PVDDoOutput(PVDClient *pvd)
{
	amsg_init(&pvd->outmsg, AMsgType_Unknown, SliceResPtr(&pvd->outbuf), SliceResLen(&pvd->outbuf));
	return pvd->io->request(pvd->io, Aio_Output, &pvd->outmsg);
}

static long PVDDoLogin(PVDClient *pvd, PVDStatus status)
{
	SliceReset(&pvd->outbuf);
	PVDInitInput(pvd, status, NET_SDVR_LOGIN, sizeof(STRUCT_SDVR_LOGUSER));

	const char *usr = "admin";
	AOption *usr_opt = aoption_find_child((AOption*)pvd->outfrom->data, "username");
	if (usr_opt != NULL)
		usr = usr_opt->value;

	const char *pwd = "888888";
	AOption *pwd_opt = aoption_find_child((AOption*)pvd->outfrom->data, "password");
	if (pwd_opt != NULL)
		pwd = pwd_opt->value;

	STRUCT_SDVR_LOGUSER *login = (STRUCT_SDVR_LOGUSER*)(pvd->outmsg.data+sizeof(pvdnet_head));
	strcpy_s(login->szUserName, usr);
	MD5_enc(pvd->md5id, (BYTE*)pwd, strlen(pwd), (BYTE*)login->szPassWord);
	login->dwNamelen = strlen(usr);
	login->dwPWlen = PASSWD_LEN;

	return pvd->io->request(pvd->io, Aio_Input, &pvd->outmsg);
}

static long PVDOpenStatus(PVDClient *pvd, long result)
{
	pvdnet_head *phead;
	do {
		if ((result < 0) && (pvd->status != pvdnet_disconnected))
			pvd->status = pvdnet_closing;
		switch (pvd->status)
		{
		case pvdnet_connecting:
			pvd->userid = 0;
			SliceReset(&pvd->outbuf);
			if (SliceResize(&pvd->outbuf, 8*1024, 2048) < 0) {
				result = -ENOMEM;
			} else {
				PVDInitInput(pvd, pvdnet_syn_md5id, NET_SDVR_MD5ID_GET, 0);
				result = pvd->io->request(pvd->io, Aio_Input, &pvd->outmsg);
			}
			break;

		case pvdnet_syn_md5id:
			pvd->status = pvdnet_ack_md5id;
			result = PVDDoOutput(pvd);
			break;

		case pvdnet_ack_md5id:
			SlicePush(&pvd->outbuf, pvd->outmsg.size);
			result = PVDTryOutput(pvd->userid, &pvd->outbuf, &pvd->outmsg);
			if (result < 0)
				break;
			if (result == 0) {
				result = PVDDoOutput(pvd);
				break;
			}
			SlicePop(&pvd->outbuf, pvd->outmsg.size);

			phead = (pvdnet_head*)pvd->outmsg.data;
			if ((phead->uCmd != NET_SDVR_MD5ID_GET) || (phead->uResult == 0)) {
				result = -EFAULT;
				break;
			}
			pvd->md5id = *(BYTE*)(phead+1);
			result = PVDDoClose(pvd, pvdnet_fin_md5id);
			if (result == 0)
				break;

		case pvdnet_fin_md5id:
			result = PVDDoOpen(pvd, pvdnet_reconnecting);
			break;

		case pvdnet_reconnecting:
			result = PVDDoLogin(pvd, pvdnet_syn_login);
			break;

		case pvdnet_syn_login:
			pvd->status = pvdnet_ack_login;
			result = PVDDoOutput(pvd);
			break;

		case pvdnet_ack_login:
			SlicePush(&pvd->outbuf, pvd->outmsg.size);
			result = PVDTryOutput(pvd->userid, &pvd->outbuf, &pvd->outmsg);
			if (result < 0)
				break;
			if (result == 0) {
				result = PVDDoOutput(pvd);
				break;
			}
			SlicePop(&pvd->outbuf, pvd->outmsg.size);

			phead = (pvdnet_head*)pvd->outmsg.data;
			if ((phead->uCmd != NET_SDVR_LOGIN) || (phead->uResult == 0)) {
				result = -EFAULT;
				break;
			}

			result -= sizeof(pvdnet_head);
			memcpy(&pvd->device2, phead+1, min(sizeof(pvd->device2), result));
			pvd->userid = phead->uUserId;

			if (result == sizeof(STRUCT_SDVR_DEVICE_EX))
				pvd->status = pvdnet_con_devinfo2;
			else if (result == sizeof(STRUCT_SDVR_DEVICE))
				pvd->status = pvdnet_con_devinfo;
			else
				pvd->status = pvdnet_con_devinfox;
			pvd->last_error = result;
			return result;

		case pvdnet_closing:
			pvd->last_error = result;
			result = PVDDoClose(pvd, pvdnet_disconnected);
			if (result == 0)
				break;

		case pvdnet_disconnected:
			pvd->status = pvdnet_invalid;
			return pvd->last_error;

		default:
			assert(FALSE);
			return -EACCES;
		}
	} while (result != 0);
	return result;
}

static long PVDOpenDone(AMessage *msg, long result)
{
	PVDClient *pvd = from_outmsg(msg);
	result = PVDOpenStatus(pvd, result);
	if (result != 0)
		result = pvd->outfrom->done(pvd->outfrom, result);
	return result;
}

static long PVDOpen(AObject *object, AMessage *msg)
{
	if ((msg->type != AMsgType_Option)
	 || (msg->data == NULL)
	 || (msg->size != 0))
		return -EINVAL;

	PVDClient *pvd = to_pvd(object);
	pvd->outmsg.done = &PVDOpenDone;
	pvd->outfrom = msg;

	if (pvd->io == NULL) {
		AOption *io_opt = aoption_find_child((AOption*)pvd->outfrom->data, "io");
		if (io_opt == NULL)
			return -EINVAL;

		long result = aobject_create(&pvd->io, &pvd->object, io_opt, NULL);
		if (result < 0)
			return result;
	}

	long result = PVDDoOpen(pvd, pvdnet_connecting);
	if (result != 0)
		result = PVDOpenStatus(pvd, result);
	return result;
}

static long PVDSetOption(AObject *object, AOption *option)
{
	PVDClient *pvd = to_pvd(object);
	if (_stricmp(option->name, "io") == 0) {
		release_s(pvd->io, aobject_release, NULL);
		pvd->io = (AObject*)option->extend;
		if (pvd->io != NULL) {
			aobject_addref(pvd->io);
			pvd->status = pvdnet_con_devinfo2;
		}
		return 1;
	}
	return -ENOSYS;
}

static long PVDGetOption(AObject *object, AOption *option)
{
	PVDClient *pvd = to_pvd(object);
	if (_stricmp(option->name, "version") == 0) {
		_itoa_s(pvd->device2.byDVRType, option->value, 10);
		return 1;
	}
	if (_stricmp(option->name, "session_id") == 0) {
		_itoa_s(pvd->userid, option->value, 10);
		return 1;
	}
	if (_stricmp(option->name, "login_data") == 0) {
		if (option->extend != NULL)
			memcpy(option->extend, &pvd->device2, sizeof(pvd->device2));
		return 1;
	}
	return -ENOSYS;
}

static long PVDOutputStatus(PVDClient *pvd)
{
	long result;
	do {
		SlicePush(&pvd->outbuf, pvd->outmsg.size);
		result = PVDTryOutput(pvd->userid, &pvd->outbuf, &pvd->outmsg);
		if (result < 0) {
			break;
		}
		if (result > 0) {
			SlicePop(&pvd->outbuf, pvd->outmsg.size);

			pvdnet_head *phead = (pvdnet_head*)pvd->outmsg.data;
			amsg_copy(pvd->outfrom, AMsgType_Custom|phead->uCmd, pvd->outmsg.data, pvd->outmsg.size);
			break;
		}
		result = PVDDoOutput(pvd);
	} while (result > 0);
	return result;
}

static long PVDOutputDone(AMessage *msg, long result)
{
	PVDClient *pvd = from_outmsg(msg);
	if (result >= 0)
		result = PVDOutputStatus(pvd);
	if (result != 0)
		result = pvd->outfrom->done(pvd->outfrom, result);
	return result;
}

static long PVDRequest(AObject *object, long reqix, AMessage *msg)
{
	PVDClient *pvd = to_pvd(object);
	if (reqix != Aio_Output)
	{
		if ((reqix == Aio_Input) && (msg->type & AMsgType_Custom)) {
			pvdnet_head *phead = (pvdnet_head*)msg->data;
			phead->uUserId = pvd->userid;
		}
		return pvd->io->request(pvd->io, reqix, msg);
	}

	pvd->outmsg.done = &PVDOutputDone;
	pvd->outfrom = msg;

	pvd->outmsg.data = NULL;
	pvd->outmsg.size = 0;
	return PVDOutputStatus(pvd);
}

static long PVDCancel(AObject *object, long reqix, AMessage *msg)
{
	PVDClient *p = to_pvd(object);
	if (p->io == NULL)
		return 1;

	return p->io->cancel(p->io, reqix, msg);
}

static long PVDCloseStatus(PVDClient *pvd, long result)
{
	pvdnet_head *phead;
	do {
		if ((result < 0) && (pvd->status != pvdnet_disconnected))
			pvd->status = pvdnet_closing;
		switch (pvd->status)
		{
		case pvdnet_syn_logout:
			pvd->status = pvdnet_ack_logout;
			amsg_init(&pvd->outmsg, AMsgType_Unknown, NULL, 0);

		case pvdnet_ack_logout:
			SlicePush(&pvd->outbuf, pvd->outmsg.size);
			result = PVDTryOutput(pvd->userid, &pvd->outbuf, &pvd->outmsg);
			if (result < 0)
				break;
			if (result == 0) {
				result = PVDDoOutput(pvd);
				break;
			}
			SlicePop(&pvd->outbuf, pvd->outmsg.size);

			phead = (pvdnet_head*)pvd->outmsg.data;
			if (phead->uCmd != NET_SDVR_LOGOUT) {
				pvd->outmsg.data = NULL;
				pvd->outmsg.size = 0;
				break;
			}
			if ((pvd->outfrom->type == AMsgType_Unknown) || (pvd->outfrom->type & AMsgType_Custom)) {
				amsg_copy(pvd->outfrom, AMsgType_Custom|phead->uCmd, pvd->outmsg.data, pvd->outmsg.size);
			}
		case pvdnet_closing:
			pvd->last_error = result;
			result = PVDDoClose(pvd, pvdnet_disconnected);
			if (result == 0)
				break;

		case pvdnet_disconnected:
			pvd->status = pvdnet_invalid;
			return pvd->last_error;

		default:
			assert(FALSE);
			return -EACCES;
		}
	} while (result != 0);
	return result;
}

static long PVDCloseDone(AMessage *msg, long result)
{
	PVDClient *pvd = from_outmsg(msg);
	result = PVDCloseStatus(pvd, result);
	if (result != 0)
		result = pvd->outfrom->done(pvd->outfrom, result);
	return result;
}

static long PVDClose(AObject *object, AMessage *msg)
{
	PVDClient *pvd = to_pvd(object);
	if (pvd->io == NULL)
		return -ENOENT;

	if (msg == NULL)
		return pvd->io->close(pvd->io, NULL);

	pvd->outmsg.done = &PVDCloseDone;
	pvd->outfrom = msg;

	long result;
	if (pvd->status < pvdnet_con_devinfo) {
		result = -ENOENT;
	} else {
		SliceResize(&pvd->outbuf, 1024, 2048);
		PVDInitInput(pvd, pvdnet_syn_logout, NET_SDVR_LOGOUT, 0);
		result = pvd->io->request(pvd->io, Aio_Input, &pvd->outmsg);
	}
	if (result != 0)
		result = PVDCloseStatus(pvd, result);
	return result;
}

AModule PVDClientModule = {
	"stream",
	"PVDClient",
	sizeof(PVDClient),
	NULL, NULL,
	&PVDCreate,
	&PVDRelease,
	NULL,
	2,

	&PVDOpen,
	&PVDSetOption,
	&PVDGetOption,
	&PVDRequest,
	&PVDCancel,
	&PVDClose,
};
