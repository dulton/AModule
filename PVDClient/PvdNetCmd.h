#ifndef _PVDNETCMD_H_
#define _PVDNETCMD_H_

#pragma warning(disable: 4200)
#include "NetCmdDef.h"
#include "NetCmdStruct.h"
#include "NetCmdIntelligentStruct.h"
#pragma warning(default: 4200)


#define NET_CMD_HEAD_FLAG    ('S'|('D'<<8)|('V'<<16)|('R'<<24))

typedef struct ST_NET_CMD_HEAD {
	unsigned long  uFlag;    //固定'SDVR'
	unsigned long  uUserId;  //用户ID
	unsigned short uCmd;     //网络命令
	unsigned short uLen;     //数据长度
	unsigned short uResult;  //返回结果(0-失败,1-成功)
	unsigned short uReserve; //保留
	//unsigned char  data[0];  //数据
} pvdnet_head;

inline int PVDCmdEncode(unsigned long userid, void *ptr, int type, int body)
{
	pvdnet_head *phead = (pvdnet_head*)ptr;
	phead->uFlag    = NET_CMD_HEAD_FLAG;
	phead->uUserId  = userid;
	phead->uCmd     = (unsigned short)type;
	phead->uLen     = (unsigned short)body;
	phead->uResult  = 0;
	phead->uReserve = (unsigned short)(body>>16);
	return sizeof(pvdnet_head)+body;
}

inline int PVDCmdDecode(unsigned long user_id, void *data, int size)
{
	if (size < sizeof(pvdnet_head)) {
		return 0;
	}

	pvdnet_head *phead = (pvdnet_head*)data;
	int cmdlen = sizeof(pvdnet_head) + phead->uLen;
	if (phead->uResult) {
		cmdlen += long(phead->uReserve)<<16;
	}

	if (phead->uFlag != NET_CMD_HEAD_FLAG) {
		return -ERROR_INVALID_DATA;
	}

	if ((user_id != 0) && (user_id != phead->uUserId)) {
		return -ERROR_INVALID_OWNER;
	}
	return cmdlen;
}


#endif
