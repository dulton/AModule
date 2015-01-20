#ifndef MSHEAD_H
#define MSHEAD_H 

//#include "common.h"

/*
MSHead�ڴ�洢�ṹʾ��ͼ
+++++++++++++++++++++
+                   +
+     MSHEADINFO    +
+                   +
+                   +
+-------------------+
+-                 -+
+-    SEGINFO_0    -+
+-                 -+
+-------------------+
+      ......       +
+-------------------+
+-                 -+
+-    SEGINFO_n    -+
+-                 -+
+-------------------+
+++++++++++++++++++++
*/

#define MSHEAD_FLAG 0x534d4248 // "HBMS" - HanBang Media Stantard V1.0 
#define MSHDV2_FLAG 0x3256534d // "MSV2" - HanBang Media Stantard V2.0 

#define MSHEAD_MAXSIZE 511 // ý��ͷ��󳤶�

#define MSHEAD_KEYFRAME 1 // �ؼ�֡���
#define MSHEAD_DATAP(mshead) (((int)mshead)+((PMSHEAD)mshead)->mshsize) // ��ȡý������ָ��
#define MSHEAD_SEGP(mshead) (((int)mshead)+sizeof(MSHEAD)) // ��ȡý������ָ��

// Audio algorithm, ż��
#define ISO_G722 0
#define ISO_G728 2
#define ISO_G729 4
#define ISO_PCM  6
#define ISO_G711A   8
#define ISO_G726    10

// Video algorithm, ����
#define ISO_MPEG4   1
#define ISO_H264    3 /* h24e�㷨,�����ο�ģʽ */
#define ISO_H264_2X 5 /* h24e�㷨,���2����֡�ο�ģʽ */
#define ISO_H264_4X 7 /* h24e�㷨,���4����֡�ο�ģʽ */
#define ISO_PIC_JPG 9 /* ͼƬ,JPG */

#define ISMSHDV1(fd)        (MSHEAD_FLAG == ((PMSHEAD)(fd))->flag)
#define ISMSHDV2(fd)        (MSHDV2_FLAG == ((PMSHEAD)(fd))->flag)
#define ISMSHEAD(fd)        (ISMSHDV1(fd) || ISMSHDV2(fd))
#define ISVIDEOFRAME(fd)    ((((PMSHEAD)(fd))->algorithm) & 1)
#define ISKEYFRAME(fd)      (MSHEAD_KEYFRAME == ((PMSHEAD)(fd))->type)

#define MUX(x, a, b)        ((x) ? (a) : (b))   /* x��Ϊ0��Ϊa,Ϊ0��Ϊb */

#define MSHEAD_GETMSHSIZE(fd)		(((PMSHEAD)(fd))->mshsize) /* ��ȡý��ͷ��С */
#define MSHEAD_GETMSDSIZE(fd)		MUX(ISMSHDV2(fd), ((PMSHEAD)(fd))->msdsize << 2, ((PMSHEAD)(fd))->msdsize)
#define MSHEAD_GETFRAMESIZE(fd)		(MSHEAD_GETMSHSIZE(fd) + MSHEAD_GETMSDSIZE(fd)) /* ��ȡý������֡��С(����ý��ͷ��С) */

typedef enum
{
	SEGID_OSD = 0,      /* OSD��Ϣ,OSD��Ϣ���ַ�����ʽ�����data��,�ַ��������˽�����'\0' */
	SEGID_TIME,         /* ʱ����Ϣ,���MSHEAD_SEG��data������(��size==sizeof(MSHEAD_SEG)),ȡMSHEAD�е�time_sec��time_msec����ʾʱ��,����ͬOSD */
	SEGID_UTF8_OSD,     /* OSD��Ϣ,OSD��Ϣ���ַ�����ʽ�����MSHEAD_SEG�ṹ��֮��,�ַ��������˽�����'\0',�ַ��������ʽΪUTF8 */
	SEGID_VENC_INFO,    /* ������Ϣ */
	SEGID_USER = 8,     /* �û��Զ���ID��ʼֵ,�Ӵ���SEGID_END�������û��Զ���ʹ��,�����Զ������Ϣ��,��size,id���ܸı���,������Ϣ�ɸ�����Ҫ�Զ��� */
	SEGID_END = 15      /* ID����ֵ */
} SEG_ID;

#ifdef _MSC_VER
#pragma warning(disable: 4200)
#endif

typedef struct
{
	unsigned int size : 9;        /* ����Ϣ���� */
	unsigned int id : 4;          /* SEGID */
	unsigned int show : 1;        /* ��ʾ����1,0-����ʾ,1-��ʾ */
	unsigned int trans : 1;       /* ͸������,0-��͸��,1-͸�� */
	unsigned int show_sec : 1;    /* ��ʾ����2,0-����ʾ,1-��ʾ */
	unsigned int reserve : 2;     /* ����λ */
	unsigned int x : 7;           /* X������(��λΪ16����) */
	unsigned int y : 7;           /* Y������(��λΪ16����) */
	signed char data[];           /* ����Ϣ��������,��Ϊ�ַ���������˽�����'\0' */
} MSHEAD_SEG, *PMSHEAD_SEG;
#define MSHEAD_SEG_LEN  sizeof(MSHEAD_SEG)

typedef struct {
  unsigned int flag;          // MSHEAD_FLAG
  unsigned int mshsize : 9;   // ý��ͷ��Ϣ��С, MAX size=511
  unsigned int msdsize : 19;  // ý����������С, Max size=512K-1
  unsigned int algorithm : 4; // ý������׼��ISO_...
  unsigned int type : 2;      // 0-P frame, 1-I frame
  unsigned int width : 7;     // ��Ϊ��Ƶ�����ʾ��ȣ���λ16���أ�����Ϊ��Ƶ�����ʾ����λ����λbit��
  unsigned int height : 7;    // ��Ϊ��Ƶ�����ʾ�߶ȣ���λ16���أ�����Ϊ��Ƶ�����ʾ�����ʣ���λK[1000]��
  unsigned int serial_no : 9; // 0~511, ѭ������������������֡�������жϣ��������֡��Ų�����ʱ����ʾ�м�֡��ʧ
  unsigned int time_msec : 7; // ��ǰ֡ʱ�䣨��λ10���룩
  unsigned int time_sec;      // ��ǰ֡ʱ�䣨��λ1�룩����ʾ�ӹ�Ԫ1970��1��1��0ʱ0��0�����������UTCʱ��������������
  unsigned int fcount;        // ����������֡����
  unsigned char data[];       // ������Ϊ����Ϣ����ý������
} MSHEAD, *PMSHEAD;

#ifdef _MSC_VER
#pragma warning(default: 4200)
#endif

#endif

