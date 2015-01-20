#ifndef __NETCMDDEV_H__
#define __NETCMDDEV_H__

//��������
#define NET_SDVR_LOGIN				0x00		//��¼
#define NET_SDVR_LOGOUT				0x01		//ע��

#define NET_SDVR_REAL_PLAY			0x02		//��ʵʱ��Ƶ
#define NET_SDVR_REAL_STOP			0x03		//ֹͣʵʱ��Ƶ

#define NET_SDVR_VEFF_GET			0x04		//��ȡ��Ƶ����
#define NET_SDVR_VEFF_SET			0x05		//������Ƶ����

#define NET_SDVR_KEYFRAME			0x06		//����ؼ�֡

#define NET_SDVR_PTZ_CTRL			0x07		//��̨����
#define NET_SDVR_PTZ_TRANS			0x08		//͸����̨
#define NET_SDVR_CTRL				0x15		//��������(��������,�ػ���)
#define NET_SDVR_UPDATE_START		0x16		//��������
#define NET_SDVR_FORMAT_DISK		0x17		//��ʽ������Ӳ��
#define NET_SDVR_FORMAT_STATE       0x18        //��ȡ��ʽ��״̬
#define NET_SDVR_ALARM_GET			0x19		//��ȡ�������
#define NET_SDVR_ALARM_SET			0x1A		//���ñ������

#define NET_SDVR_VOICE_START		0x1B		//�����Խ�����
#define NET_SDVR_VOICE_STOP			0x1C		//ֹͣ�Խ�����

#define NET_SDVR_SERIAL_START		0x1D		//����͸��ͨ��
#define NET_SDVR_SERIAL_STOP		0x1E		//ֹͣ͸��ͨ��

#define NET_SDVR_CLICK_KEY			0x1F		//�������

#define NET_SDVR_MANURECORD_SET		0x20		//�����ֶ�¼��
#define NET_SDVR_MANURECORD_GET		0x21		//��ȡ�ֶ�¼��

#define NET_SDVR_WORK_STATE			0x22		//��ȡ��������״̬

#define NET_SDVR_LOG_QUERY			0x23		//������־��ѯ

#define NET_SDVR_DEVICECFG_GET		0x26		//��ȡ�豸Ӳ����Ϣ
#define NET_SDVR_DEVICECFG_SET		0x27		//�����豸Ӳ����Ϣ

#define NET_SDVR_NETCFG_GET			0x28		//��ȡ�豸�������
#define NET_SDVR_NETCFG_SET			0x29		//�����豸�������

#define NET_SDVR_PICCFG_GET			0x2A		//��ȡͨ������(��������¼��,�ƶ�¼���)
#define NET_SDVR_PICCFG_SET			0x2B		//����ͨ������(��������¼��,�ƶ�¼���)

#define NET_SDVR_COMPRESSCFG_GET	0x2C		//��ȡѹ������
#define NET_SDVR_COMPRESSCFG_SET	0x2D		//����ѹ������

#define NET_SDVR_RECORDCFG_GET		0X2E		//��ȡ��ʱ¼�����
#define NET_SDVR_RECORDCFG_SET		0X2F		//���ö�ʱ¼�����

#define NET_SDVR_DECODERCFG_GET		0X30		//��ȡ����������(��̨����)
#define NET_SDVR_DECODERCFG_SET		0X31		//���ý���������(��̨����)

#define NET_SDVR_ALARMINCFG_GET		0X32		//��ȡ�����������(��չ)
#define NET_SDVR_ALARMINCFG_SET		0X33		//���ñ����������
#define NET_SDVR_ALARMOUTCFG_GET	0X34		//��ȡ�����������(��չ)
#define NET_SDVR_ALARMOUTCFG_SET	0X35		//���ñ����������

#define NET_SDVR_TIMECFG_GET		0X36		//��ȡ����ʱ��
#define NET_SDVR_TIMECFG_SET		0X37		//��������ʱ��

#define NET_SDVR_USERCFG_GET		0X38		//��ȡ�û�Ȩ��
#define NET_SDVR_USERCFG_SET		0X39		//�����û�Ȩ��

#define NET_SDVR_SERIAL_GET			0x3A		//��ȡ���ڲ���
#define NET_SDVR_SERIAL_SET			0x3B		//���ô��ڲ���

#define NET_SDVR_MD5ID_GET			0x40		//��ȡMD5������,��½ʱʹ��

#define NET_SDVR_REFRESH_FLASH		0x41		//����DVR FLASH

#define NET_SDVR_RECOVER_DEFAULT	0x42		//�ָ�DVRȱʡ����
#define NETCOM_QUERY_RECFILE        0x45		//PC��DVR���͵Ĳ�ѯ¼���ļ�����Ϣ����
#define NETCOM_QUERY_RECFILE_EX     0x46		//PC��DVR���͵Ĳ�ѯ¼���ļ�����Ϣ����(�����Ų�ѯ)

#define NET_SDVR_GET_HEARTBEAT      0x48		//��ȡ����ͻ��˵ĺ�ʱ����

#define NETCOM_VOD_RECFILE_REQ      0x49		//PC��DVR���͵ĵ㲥¼���ļ�����
#define NETCOM_VOD_RECFILE_REQ_EX	0x4A		//PC��DVR���͵ĵ㲥��ʱ��β���¼���ļ�����  add by cui 08.12.19
#define NETCOM_VOD_RECFILE_CTL	    0x4C		//tcp��ʽ�����õ㲥�ٶȣ�
#define NETCOM_VOD_RECFILE_END      0x4F		//PC��DVR���͵ĵ㲥����
#define NETCOM_BACKUP_RECFILE_REQ   0x51		//PC��DVR���͵ı���¼���ļ�����
#define NETCOM_BACKUP_FILEHEAD      0x5A		//DVR��PC���͵ı����ļ�ͷ��־(�ڱ��ݵ�������)
#define NETCOM_BACKUP_END           0x5C		//DVR��PC���͵ı����ļ�������־(�ڱ��ݵ�������) 
#define NETCOM_BACKUP_RECFILE_OK    0x53		//DVR��DVR���͵ı���¼���ļ�����ɹ�
#define NETCOM_BACKUP_RECFILE_NOOK  0x55		//DVR��DVR���͵ı���¼���ļ�����ʧ��

#define NETVOD_START				0x56		//PC ��DVR ���͵ĵ㲥��ʼ
#define NETVOD_DATA					0x57		//PC ��DVR ���͵ĵ㲥����
#define NETVOD_PAUSE				0x58		//PC ��DVR ���͵ĵ㲥��ͣ
#define NETVOD_STOP					0x59		//PC ��DVR ���͵ĵ㲥����

#define	NET_SDVR_SHAKEHAND			0xFF		//����Э��
#define NET_SDVR_DNS_GET			0x60		//��ȡDNS
#define NET_SDVR_DNS_SET			0x61		//����DNS   sDNSState=1ʱ���½   
#define NET_SDVR_PPPoE_GET			0x62		//��ȡPPPoE
#define NET_SDVR_PPPoE_SET			0x63		//����PPPoE  sPPPoEState=1ʱ�򲦺�
#define	NET_SDVR_SERVERCFG_GET		0x64		//��ȡƽ̨����������
#define	NET_SDVR_SERVERCFG_SET		0x65		//������ƽ̨����������
#define	NET_SDVR_CLEARALARM			0x66		//�������
#define NET_SDVR_SET_SERIALID		0x67		//�������к�
#define NET_SDVR_GET_SERIALID		0x68		//��ȡ���к�
#define NET_SDVR_GET_VLostStatus	0x69		//��ȡ��Ƶ��ʧ
#define NET_SDVR_GET_PHOTO			0x70		//����
#define NET_SDVR_GET_DVRTYPE		0x71		//�豸����
#define NET_SDVR_SET_DVRTYPE		0x72		//�豸����(����汾)

#define NET_SDVR_SET_PRESETPOLL		0x73		//���ö�Ԥ�õ���Ѳ
#define NET_SDVR_GET_PRESETPOLL		0x74		//��ȡ��Ԥ�õ���Ѳ

#define NET_SDVR_SET_VIDEOSYS       0x75		//������Ƶ��ʽ(N/P�л�)
#define NET_SDVR_GET_VIDEOSYS       0x76		//��ȡ��Ƶ��ʽ(N/P�л�)

#define NET_SDVR_GET_REAL_DEFENCE	0x7A		//��ȡʵʱ����״̬
#define NET_SDVR_SET_REAL_DEFENCE	0x7B		//ʵʱ����(���Ǵ������ʼ����������������ʱһ��ʱ���(����60s))
#define NET_SDVR_GET_DELAY_PHOTO	0x7C		//ץ��ʱͼƬ
#define NET_SDVR_GET_UPLOAD_ALARMTYPE  0x7D  // ��ȡ֧�������ϴ������ı������� 
#define NET_SDVR_GET_UPLOAD_ALARMINFO  0x7E  //��ȡ���������ϴ����� 
#define NET_SDVR_SET_UPLOAD_ALARMINFO  0x7F  //�������������ϴ����� 

#define NET_SDVR_PTZTYPE_GET		0x79		//��ȡ��̨Э��

#define NET_SDVR_IPCWIRELESS_SET    0x87        //����IPC���߲�������
#define NET_SDVR_IPCWIRELESS_GET    0x88		//��ȡIPC���߲�������

#define NET_SDVR_NTPCONFIG_SET	    0x90        //����NTP����
#define NET_SDVR_NTPCONFIG_GET	    0x91        //��ȡNTP����

#define NET_SDVR_SMTPCONFIG_SET	    0x92        //����SMTP����
#define NET_SDVR_SMTPCONFIG_GET	    0x93        //��ȡSMTP����

#define NET_SDVR_POLLCONFIG_SET	    0x94        //������Ѳ���ò���
#define NET_SDVR_POLLCONFIG_GET	    0x95        //��ȡ��Ѳ���ò���

#define NET_SDVR_VIDEOMATRIX_SET	0x96        //������Ƶ�������
#define NET_SDVR_VIDEOMATRIX_GET	0x97        //��ȡ��Ƶ�������

#define NET_SDVR_GET_VCOVER_DETECT  0x98		//��ȡ�ڵ��������� 
#define NET_SDVR_SET_VCOVER_DETECT  0x99		//�����ڵ���������

#define NET_SDVR_IPCWORKPARAM_GET   0xB0        //��ȡIPC��������
#define NET_SDVR_IPCWORKPARAM_SET   0xB1		//����IPC��������	

#define NET_SDVR_GET_CHLPARAM_SUPPORT     0xE6             //��ȡ����ͨ������֧�ַ�Χ
//#define NET_SDVR_GET_USER_PERSION_VER     0XB3             //��ȡ�����û�Ȩ��ģʽ
#define NET_SDVR_USERCFG_GET_EXT1	      0XE8			   //��ȡ�û���Ϣ��չ1
#define NET_SDVR_USERCFG_SET_EXT1		  0XE9			   //�����û���Ϣ��չ1

#define NET_SDVR_GET_PLATFORM_PARAM	0xF0	    //��ȡƽ̨����
#define NET_SDVR_SET_PLATFORM_PARAM	0xF1	    //����ƽ̨����




#define NET_SDVR_INITIATIVE_LOGIN		0xF2    //������������ƽ̨������
#define NET_SDVR_REAL_PLAY_EX			0xF3	//��������ģʽ����Ƶ������չ
#define NETCOM_VOD_RECFILE_REQ_EX1		0xF4	//��������ģʽԶ��¼��㲥
#define NETCOM_BACKUP_RECFILE_REQ_EX1	0xF5	//��������ģʽԶ��¼�񱸷�
#define NET_SDVR_VOICE_START_EX			0xF6	//��������ģʽ������
#define NET_SDVR_GET_PHOTO_EX			0xF7	//��������ģʽץͼ

#define NET_SDVR_UPGRADE_REQ_EX			0xF9    //��������Զ������������չ

#define NET_SDVR_SET_VOICE_DECODE      0xDE   //���öԽ��������ݸ�ʽ 

#define SDVR_SHUTDOWN				0			//�ر��豸
#define SDVR_REBOOT					1			//�����豸
#define SDVR_RESTORE				2			//�ָ�������


#define NET_SDVR_GET_ZERO_VENCCONF					0xA8	//��ȡ����ͨ����Ƶ����
#define NET_SDVR_SET_ZERO_VENCCONF					0xA9	//���ø���ͨ����Ƶ����
#define NET_SDVR_DST_TIME_GET						0xAA	//��ȡ����ʱʱ��������Ϣ
#define NET_SDVR_DST_TIME_SET						0xAB	//��������ʱʱ��������Ϣ

#define NETCOM_QUERY_RECFILE_EXIST  0X44		//��ѯ�����������Ƿ�ÿ�춼��¼���ļ�  2012-3-19

#define NET_SDVR_WORK_STATE_EX		0xC1		//��ȡ�����豸����״̬(��չ)NVR
#define NET_SDVR_LOG_QUERY_EX		0xC3		//NVR��־��Ϣ��ѯ

#define NET_SDVR_DEVICECFG_GET_EX	0xC6		//��ȡ�豸��Ϣ(��չ)NVR
#define NET_SDVR_DEVICECFG_SET_EX	0xC7		//�����豸��Ϣ(��չ)NVR

#define NET_SDVR_PTZLIST_GET_NVR	0xC8		//��ȡNVR��̨Э���б�

#define NET_SDVR_ALRMIN_GET_NVR		0xCA		//��ȡNVR�����������
#define NET_SDVR_ALRMIN_SET_NVR		0xCB		//����NVR�����������
#define NET_SDVR_ALRMOUT_GET_NVR	0xCC		//��ȡNVR�����������
#define NET_SDVR_ALRMOUT_SET_NVR	0xCD		//����NVR�����������

#define NET_SDVR_PICCFG_GET_EX_NVR	0xD2		//��ȡNVRͨ������
#define NET_SDVR_PICCFG_SET_EX_NVR	0xD3		//����NVRͨ������
#define NET_SDVR_RECORD_GET_EX_NVR	0xD4		//��ȡNVR¼�����
#define NET_SDVR_RECORD_SET_EX_NVR	0xD5		//����NVR¼�����
#define NET_SDVR_MOTION_DETECT_GET_NVR	0xD6	//��ȡNVR�ƶ�������
#define NET_SDVR_MOTION_DETECT_SET_NVR	0xD7	//����NVR�ƶ�������

#define NET_SDVR_STORDEVCFG_GET         0x09    //��ȡ�洢�豸����
#define NET_SDVR_FORMAT_STORDEV         0x0A    //��ʽ�������洢�豸

#define NET_SDVR_OSDCFG_GET				0x3C	//��ȡͨ���ַ�������Ϣ
#define NET_SDVR_OSDCFG_SET				0x3D	//����ͨ���ַ�������Ϣ

#define NET_SDVR_QUERY_MONTH_RECFILE	0x47		//Զ�̲�ѯ���ĳ�µ�¼���¼

#define NET_SDVR_GET_TEMPERATURE			0x6A		//��ȡ�����¶�

#define NET_SDVR_GET_RECORDSTATISTICS    0x6B		//��ȡ¼��ͳ��
#define NET_SDVR_RECORDNOTENOUGHDATE_ALARM	0x6C		//¼���������㱨��

#define NET_SDVR_ALARM_REQ           0xF8   //���������ϴ�

#define NET_SDVR_SPECIFIC_DDNS						0x5E    //��ȡָ��DDNS����������
#define NET_SDVR_DDNSSERVERTLIST_GET         0x5F    //��ȡ������֧�ֵ�DDNS�б�

#define NET_SDVR_PARAM_FILE_EXPORT           0xDA   //���������ļ����� 
#define NET_SDVR_PARAM_FILE_IMPORT           0xDB   //���������ļ����� 

#define NETCOM_BACKUP_RECFILE_REQ_EX       0x52   //PC��DVR���͵ı���¼���ļ�����,����ʵ�ֶϵ�����
#define NET_SDVR_SUPPORT_FUNC					   0x5D   //��ȡ������֧�ֵĹ���

#define NET_SDVR_CHAN_CARDCOR_SET             0x13    //����ͨ������¼��״̬ 
#define NET_SDVR_CHAN_CARDCOR_GET             0x14   //��ȡͨ������¼��״̬ 

#define NET_SDVR_AUTO_TEST_DEVICE				 0xF000	//�����Զ�������
#define NET_SDVR_AUTOTEST_HARDDISK_VOD    0xF001 //Ӳ��ʶ�𡢸�ʽ����¼�����ݷ������Զ�������

#define NET_SDVR_GET_CH_CLIENT_IP		0xC2		//��ȡ���ӵ���ͨ���Ŀͻ���IP�б�
#define NET_SDVR_SERIAL_START_NVR	0xC4		//����NVR͸��ͨ��
#define NET_SDVR_SERIAL_STOP_NVR		0xC5		//�ر�NVR͸��ͨ��
#define NET_SDVR_ALRM_ATTR_NVR		0xC9		//��ȡNVR������������˿�����
#define NET_SDVR_ALRMIN_STATUS_GET_NVR			0xCE		//��ȡNVR��������״̬
#define NET_SDVR_ALRMOUT_STATUS_GET_NVR		0xCF		//��ȡNVR�������״̬
#define NET_SDVR_ALRMOUT_STATUS_SET_NVR		0xD1		//����NVR�������״̬
#define NET_SDVR_ABNORMAL_ALRM_GET_NVR		0xD8		//��ȡNVR�쳣��������
#define NET_SDVR_ABNORMAL_ALRM_SET_NVR		0xD9		//����NVR�쳣��������
#define NET_SDVR_RESOLUTION_GET_NVR		0xDC		//��ȡNVR�����ֱ���
#define NET_SDVR_RESOLUTION_SET_NVR		0xDD		//����NVR�����ֱ���
#define NET_SDVR_GET_REMOTE_RESOLUTION       0xDF   //��ȡNVR ǰ���豸�ֱ����б�

#define NET_SDVR_GET_LOGO_UPDATE_INFO						0xF002		//��ȡLOGO������Ϣ
#define NET_SDVR_AUTOTEST_HARDDISK_VOD_STOP			0xF003		//Ӳ��ʶ�𡢸�ʽ����¼�����ݷ������Զ������Խ���

#define NET_SDVR_ADD_IPDEV								0x10   //���IP�豸 
#define NET_SDVR_DEL_IPDEV								0x11   //ɾ��IP�豸 
#define NET_SDVR_GET_ALLIPCH_INFO					0x12   //��ȡ����������ӵ�IP�豸��Ϣ
#define NET_SDVR_GET_IPCH_INFO							0x4b   //��ȡ������ӵ�ĳ��IP�豸��Ϣ
#define NET_SDVR_IPDEV_REBOOT							0x6D   //Զ������IP�豸
#define NET_SDVR_GET_IPDEV_NETPARAM				0x4d   //��ȡIP�豸�������
#define NET_SDVR_SET_IPDEV_NETPARAM				0x4e   //����IP�豸�������
#define NET_SDVR_GET_IPDEV_TIME						0x50   //��ȡIP�豸ϵͳʱ��
#define NET_SDVR_SET_IPDEV_TIME						0x54   //����IP�豸ϵͳʱ��
#define NET_SDVR_GET_LOCH_STATUE					0x6E   //��ȡ����ͨ������״̬ 
#define NET_SDVR_SET_LOCH_STATUE						0x6F   //���ñ���ͨ������״̬ 

#define NET_SDVR_PREANDDELRECCFG_GET        0x24   //��ȡͨ��¼��Ԥ¼����¼ʱ�� 
#define NET_SDVR_PREANDDELRECCFG_SET        0x25   //����ͨ��¼��Ԥ¼����¼ʱ��

#define NETCOM_QUERY_SNAPSHOT                0xE0   //Զ��ͼƬ��ѯ���� 
#define NETCOM_BACKUP_SNAPSHOT_REQ           0xE1   //Զ��ͼƬ�������� 
#define NETCOM_BACKUP_SNAPSHOT_FILEHEAD      0xE2   //Զ��ͼƬ�ļ�ͷ��־ 
#define NETCOM_BACKUP_SNAPSHOT_END           0xE3   //Զ��ͼƬ���ݽ��� 
#define NET_SDVR_SNAPSHOT_GET                0xE4   //��ȡԶ��ͼƬ������Ϣ 
#define NET_SDVR_SNAPSHOT_SET                0xE5   //����Զ��ͼƬ������Ϣ

#define NET_SDVR_SEARCH_IPDEV                 0x8A    //����IP �豸 
#define NET_SDVR_NETINFO_6_GET                0x8B    //��ȡ����IPV6 ������� 
#define NET_SDVR_NETINFO_6_SET                0x8C    //��������IPV6 ������� 
#define NET_SDVR_OSD_CONFIG_GET               0x8D    //��ȡ��չOSD 
#define NET_SDVR_OSD_CONFIG_SET               0x8E    //������չOSD 
#define NET_SDVR_FRAMERATE_GET                0x8F    //��ȡ��֡�ʺ�ʣ��֡��

#define   NET_SDVR_HB_YDT_GET                 0x9A   //��ȡ����һ��ͨ���� 

#define   NET_SDVR_HB_YDT_SET                 0x9B   //���ú���һ��ͨ���� 
#define   NET_SDVR_TELEADJUSTING_CAMERA   0xED     //���������Զ�̵���

#define   NET_SDVR_GET_TIMING_REBOOTINFO      0xEB    //��ȡ������ʱ��������
#define   NET_SDVR_SET_TIMING_REBOOTINFO      0xEC    //����������ʱ�������� 

#define NET_SDVR_REAL_PLAY_MULTI  0xF004//������socket��ͨ��ʵʱ��
#define NET_SDVR_REAL_STOP_MULTI  0xF005//�رյ�socket��ͨ��ʵʱ��

//��̨��������
#define SDVR_PAN_AUTO				0x0001001c 	//�Զ�ɨ��
#define SDVR_STOP_PTZ				0x00010028	//ֹͣ��̨
#define SDVR_BRUSH					0x0001002e  //��ˢ
#define SDVR_UP						0x0001000c	//������ת
#define SDVR_DOWN					0x0001000d	//������ת
#define SDVR_LEFT					0x0001000e 	//����ת
#define SDVR_RIGHT					0x0001000f	//����ת
#define SDVR_ZOOM_IN				0x00010016	//������(���ٶ�SS���ʱ��)
#define SDVR_ZOOM_OUT				0x00010017 	//�����С(���ٶ�SS���ʱ�С)
#define SDVR_IRIS_OPEN				0x00010018 	//��Ȧ����(���ٶ�SS����)
#define SDVR_IRIS_CLOSE				0x00010019	//��Ȧ��С(���ٶ�SS��С)
#define SDVR_FOCUS_FAR				0x00010015 	//������(���ٶ�SS���)
#define SDVR_FOCUS_NEAR				0x00010014  //����ǰ��(���ٶ�SSǰ��)

#define SDVR_LIGHT_PWRON			0x00010024	//��ͨ�ƹ��Դ
#define SDVR_WIPER_PWRON			0x00010025	//��ͨ��ˢ����
#define SDVR_SET_PRESET				0x0001001a 	//����Ԥ�õ�
#define SDVR_GOTO_PRESET			0x0001001b 	//����ת��Ԥ�õ�
#define SDVR_CLE_PRESET				18			//���Ԥ�õ�

#define SDVR_FAN_PWRON				19			//��ͨ���ȿ���
#define SDVR_HEATER_PWRON			20			//��ͨ����������
#define SDVR_AUX_PWRON				21			//��ͨ�����豸����

#define SDVR_FILL_PRE_SEQ			30			//��Ԥ�õ����Ѳ������
#define SDVR_SET_SEQ_DWELL			31			//����Ѳ����ͣ��ʱ��
#define SDVR_SET_SEQ_SPEED			32			//����Ѳ���ٶ�
#define SDVR_CLE_PRE_SEQ			33			//��Ԥ�õ��Ѳ��������ɾ��
#define SDVR_STA_MEM_CRUISE			34			//��ʼ��¼�켣
#define SDVR_STO_MEM_CRUISE			35			//ֹͣ��¼�켣
#define SDVR_RUN_CRUISE				36			//��ʼ�켣
#define SDVR_RUN_SEQ				37			//��ʼѲ��
#define SDVR_STOP_SEQ				38			//ֹͣѲ��
#define SDVR_SYSTEM_RESET			40			//ϵͳ��λ

//���������л��������󷵻���
#define INITIATIVE_REPONSE_RESULT	9000		

///////////////////////////////////////////////////////////////
//�豸����
#define SDVR_TYPE			-1					//δ֪�豸
#define SDVR_TYPE_IPC		103					//IPC(��:���������)
//#define SDVR_TYPE_7000T		7					//7000T(��:7000TDVR)
//#define SDVR_TYPE_8000		8					//8000(��:8000DVR)
//#define SDVR_TYPE_8000T		9					//8000T(��:8000TDVR)
//#define SDVR_TYPE_8024T		10					//8024T(��:8000Tϵ��24·DVR)

#define SDVR_TYPE_7000T		0					//7000T(��:7000TDVR)
#define SDVR_TYPE_8000T		1					//8000T(��:8000TDVR)
#define SDVR_TYPE_8000		80					//8000(��:8000DVR)
#define SDVR_TYPE_8024T		81					//8024T(��:8000Tϵ��24·DVR)


//�豸����2(�����ֶ�)
#define DEVTYPE_7200H       9                   //7200Hϵ��
#define DEVTYPE_7200L       10                  //7200Lϵ��
#define DEVTYPE_7000M       12                  //7000Mϵ��
#define DEVTYPE_7000L       15                  //7000Lϵ��
#define DEVTYPE_2201TL		16					//2201�豸
#define DEVTYPE_2600T       17                  //2600T�豸  
#define DEVTYPE_2600B		18					//2600B�豸  

#define DEVTYPE_7024M       70                  //7024M(��7000M�Ļ����ϸ���ͨ��������)
#define DEVTYPE_7024T       71                  //7024T(��7000T�Ļ����ϸ���ͨ��������)
#define DEVTYPE_7032X       72					//7032X(��7000M�Ļ����ϸ���ͨ��������)

#define SDVR_TYPE_9000		90					//9000ϵ�и����豸
#define SDVR_TYPE_NVR		100					//NVR

#define NET_SDVR_UPNPCFG_GET        0x3C       //��ȡUPNP���ò���
#define NET_SDVR_UPNPCFG_SET        0x3D        //����UPNP���ò��� 


#endif