////////////////////////////////////////////////////////////////////////////////
// ��Ȩ���У�2009-2011����������߿����ּ������޹�˾��
// ���ļ���δ�����ģ������˺���߿ƻ��ܺ�ר�����ݡ�
////////////////////////////////////////////////////////////////////////////////
// �ļ�����HBStreamHeaderV30.h
// ���ߣ�deng feng
// �汾��1.2
// ���ڣ�2012-07-25
// ���������庺��߿Ƶ������ṹ���ġ�����չ������Ƶ֡ͷ��
// ��ʷ��¼��
//  2011-04-20  ��ɺ���߿Ƶ���������Ƶ֡ͷ�Ķ��塣
//  2011-06-15  ����Stream_GetSegmentHeaderAddress����bug��
//  2011-08-03  ��дStream_IsValidFrame������
//  2012-07-22  ����Stream_GenerateChecksum����������
//              STREAM_HEADER.nCheckedEncodeDataSize * 2 >= STREAM_HEADER.nEncodeDataSize
//              �������
//  2012-07-25  ΪNVR����STREAM_ALGORITHM_VIDEO_H264_GENERAL��ʽ��
////////////////////////////////////////////////////////////////////////////////
#ifndef HB_STREAM_HEADER_H
#define HB_STREAM_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif


//֡ͷ�Ĵ洢�ṹ��
// +----------------------------+
// |         STREAM_HEADER      |
// +----------------------------+
// |    STREAM_VIDEO_HEADER     |
// +----------------------------+
// |  STREAM_SEGMENT_HEADER[0]  |
// +----------------------------+
// |          ......            |
// +----------------------------+
// | STREAM_SEGMENT_HEADER[n-1] |
// +----------------------------+
// |                            |
// |  Audio Data or Video Data  |
// |                            |
// +----------------------------+
//һ�����͵���Ƶ֡������STREAM_HEADER��Audio Data��
//һ�����͵���ƵI֡������STREAM_HEADER��STREAM_VIDEO_HEADER������STREAM_SEGMENT_HEADER��Video Data��
//һ�����͵���ƵP֡����E֡��������STREAM_HEADER��STREAM_VIDEO_HEADER��Video Data��


//֡ͷ��־��
#define STREAM_HEADER_FLAG              0xFF620068


//֡���͡�
#define STREAM_FRAME_AUDIO              0           //��Ƶ֡
#define STREAM_FRAME_VIDEO_I            1           //��ƵI֡
#define STREAM_FRAME_VIDEO_P            2           //��ƵP֡
#define STREAM_FRAME_VIDEO_B            3           //��ƵB֡
#define STREAM_FRAME_VIDEO_E            4           //��ƵE֡


//16-bit PCMֻ������8λ��
//�����Ϊ��1ͨ����8000Hz�����ʡ�16-bit��PCM���ݡ�
#define STREAM_ALGORITHM_AUDIO_PCM8_16  0

//G.711 Alaw
////�����Ϊ��1ͨ����8000Hz�����ʡ�16-bit��PCM���ݡ�
#define STREAM_ALGORITHM_AUDIO_G711A    1

//G.722
//�����Ϊ��1ͨ����8000Hz�����ʡ�16-bit��PCM���ݡ�
#define STREAM_ALGORITHM_AUDIO_G722     2

//G.726 16kbps
//�����Ϊ��1ͨ����8000Hz�����ʡ�16-bit��PCM���ݡ�
#define STREAM_ALGORITHM_AUDIO_G726     3

//16-bit PCM
//����Ҫ���룬1ͨ����8000Hz�����ʡ�16-bit��PCM���ݡ�
#define STREAM_ALGORITHM_AUDIO_PCM16    4


//��Ƶ�����㷨��
#define STREAM_ALGORITHM_VIDEO_H264_HISILICON   0   //Hisilicon H.264
#define STREAM_ALGORITHM_VIDEO_H264_AMBARELLA   1   //Ambarella H.264
#define STREAM_ALGORITHM_VIDEO_H264_TECHWELL    2   //Techwell H.264
#define STREAM_ALGORITHM_VIDEO_H264_GENERAL     3   //ͨ�� H.264
#define STREAM_ALGORITHM_VIDEO_MPEG4            100 //MPEG4
#define STREAM_ALGORITHM_VIDEO_MPEG4_ISO        101 //MPEG4 ISO


//����֡ͷ��
//sizeof(STREAM_HEADER) == 16
typedef struct STREAM_HEADER
{
    //֡ͷ��־��������STREAM_HEADER_FLAG��
    unsigned int                nHeaderFlag;

    //֡ͷ�Ĵ�С�����ֽ�Ϊ��λ��ȡֵ��Χ[sizeof(STREAM_HEADER), 65535]��
    // nHeaderSize = sizeof(STREAM_HEADER)
    // + IsStreamVideoFrame() ? STREAM_VIDEO_HEADER.nVideoHeaderSize : 0
    // + STREAM_SEGMENT_HEADER[0].nSegmentSize + ... 
    // + STREAM_SEGMENT_HEADER[nSegmentCount-1].nSegmentSize;
    unsigned int                nHeaderSize : 16;

    //֡���ͣ�ʹ��STREAM_FRAME_*��
    //��֡������STREAM_FRAME_VIDEO_*ʱ��STREAM_HEADER�ṹ�������STREAM_VIDEO_HEADER��
    unsigned int                nFrameType : 3;

    //��Ƶ����Ƶ�����㷨��
    //��nFrameType==STREAM_FRAME_AUDIOʱ��ʹ��STREAM_ALGORITHM_AUDIO_*��
    //��nFrameType==STREAM_FRAME_VIDEO_*ʱ��ʹ��STREAM_ALGORITHM_VIDEO_*��
    unsigned int                nAlgorithm : 5;

    //STREAM_SEGMENT_HEADER�ṹ���������ȡֵ��Χ[0, 255]��
    //��nSegmentCount==0ʱ����ʾû��STREAM_SEGMENT_HEADER��
    //һ��ģ�ֻ��nFrameType == STREAM_FRAME_VIDEO_Iʱ��������nSegmentCount��
    unsigned int                nSegmentCount : 8;

    //��ҪУ��ı������ݵĴ�С��ȡֵ��Χ[0, 255]��
    //һ��ֻУ��������ݵ�ǰ���󲿷��ֽڡ�
    //��nCheckedDataSize==0ʱ����У��������ݡ�
    //���飺����У��ǰ���16���ֽڵı������ݡ�
    unsigned int                nCheckedEncodeDataSize : 8;

    //У��ͣ����ܵ���0��
    //����STREAM_VIDEO_HEADER��STREAM_HEADER_SEGMENT[0 : n-1]�ͱ������ݵ�ǰ����
    //�����ֽڡ�
    unsigned int                nChecksum : 24;

    //�������ݵĴ�С�����ֽ�Ϊ��λ��ȡֵ��Χ[0, 16777215]��
    unsigned int                nEncodeDataSize : 24;

    //������
    unsigned int                nReserved : 8;

} STREAM_HEADER, *PSTREAM_HEADER;



//��Ƶ֡ͷ��Ϣ��
//sizeof(STREAM_VIDEO_HEADER) == 24
typedef struct STREAM_VIDEO_HEADER
{
    //��Ƶ֡ͷ�Ĵ�С�����ֽ�Ϊ��λ��ȡֵ��Χ[sizeof(STREAM_VIDEO_HEADER), 255]��
    unsigned int                nVideoHeaderSize : 8;

    //��Ƶ֡����ʱ��ĺ���ֵ��ȡֵ��Χ[0, 999]��
    //����ͨ��_ftime64��_ftime64_s��������ȡnTimeStampLow��nTimeStampHight
    //��nTimeStampMillisecond��
    //����ʹ��nTimeStampHight��nTimeStampMillisecond������ͨ��time��_time32����
    //����ȡnTimeStampLow��
    unsigned int                nTimeStampMillisecond : 10;

    //ͼ���ȣ���4����Ϊ��λ��ȡֵ��Χ[0, 16383]��
    //ʵ��ͼ����= nWidth * 4��
    unsigned int                nWidth : 14;

    //ͼ��߶ȣ���4����Ϊ��λ��ȡֵ��Χ[0, 8191]��
    //ʵ��ͼ��߶�= nHeight * 4��
    unsigned int                nHeight : 13;

    //��Ƶ֡�ʣ��Ժ��ȣ�Hz��Ϊ��λ��ȡֵ��Χ[1, 127]��
    unsigned int                nFrameRate : 7;

    //������
    unsigned int                nReserved : 12;

    //֡��š�
    //֡������𽥵����ģ������ж���Ƶ֡�������ԡ�
    unsigned int                nIndex;

    //��Ƶ֡���ʱ�䣬�Ժ��루millisecond��Ϊ��λ��
    //����ʱ�����ڿ���2֮֡���ʱ������
    unsigned int                nTickCount;

    //��Ƶ֡����ʱ�䣬���루second��Ϊ��λ��
    //nTimeStampLowΪ64λ�����ĵ�32λ��nTimeStampHightΪ64λ�����ĸ�32λ��
    //������ʱ�䣨UTC��ʱ����㣬�ӹ�Ԫ1970-01-01 00:00:00�𾭹���������
    //nTimeStampLow����ʾ����Ԫ2038-01-19 03:14:07, UTC��
    //nTimeStampLow��nTimeStampHight��ͬ����ʾ����Ԫ3000-12-31 23:59:59, UTC��
    unsigned int                nTimeStampLow;
    int                         nTimeStampHight;

} STREAM_VIDEO_HEADER, *PSTREAM_VIDEO_HEADER;


//�����͡�
#define STREAM_SEGMENT_OSD_STRING   0               //�Զ����ַ���
#define STREAM_SEGMENT_OSD_TIME     1               //ʱ����Ϣ
#define STREAM_SEGMENT_MOSAIC       2               //������


//ʱ���ʽ��
//�����Զ�����ָ�ʽ��
#define STREAM_TIME_FORMAT_DEFAULT  0   //yyyy-MM-dd HH:mm:ss ==> %04d-%02d-%02d %02d:%02d:%02d
#define STREAM_TIME_FORMAT_1        1   //yyyy-MM-dd HH:mm:ss.mmm ==> %04d-%02d-%02d %02d:%02d:%02d.%03d
#define STREAM_TIME_FORMAT_2        2   //HH:mm:ss ==> %02d:%02d:%02d
#define STREAM_TIME_FORMAT_3        3   //dddd, MMMM d, yyyy, HH:mm:ss ==> %s, %s %d, %04d, %02d:%02d:%02d
#define STREAM_TIME_FORMAT_4        4   //M/d/yy h:mm:ss tt ==> %d/%d/%02d %d:%02d:%02d %s
#define STREAM_TIME_FORMAT_5        5   //yyyy��M��d�� HHʱmm��ss�� ==> %04d��%d��%d�� %02dʱ%02d��%02d��
#define STREAM_TIME_FORMAT_6        6   //yyyy��M��d�� ���� HHʱmm��ss�� ==> %04d��%d��%d�� %s %02dʱ%02d��%02d��
#define STREAM_TIME_FORMAT_7        7   //yyyy��M��d�� tt h:mm:ss ==> %04d��%d��%d�� %s %d:%02d:%02d
#define STREAM_TIME_FORMAT_8        8   //�����Զ����ʽ


//���������ģʽ��
#define STREAM_MOSAIC_FILL_BLACK    0               //����ɫ
#define STREAM_MOSAIC_FILL_REALTIME 1               //���ʵʱ����ɫ


//����Ϣ��
//sizeof(STREAM_SEGMENT_HEADER) == 16
typedef struct STREAM_SEGMENT_HEADER
{
    //�δ�С�����ֽ�Ϊ��λ��ȡֵ��Χ[sizeof(STREAM_SEGMENT_HEADER), 2047]��
    //����������u����չ���ݵĳ��ȡ�
    //����չ����Ϊ�ַ���ʱ������������'\0'��
    unsigned int                nSegmentSize : 11;

    //�����ͣ�ָ��������u����Ч�ĳ�Ա��
    //����������STREAM_SEGMENT_OSD_STRINGʱ��u.OsdString��Ч��
    //����������STREAM_SEGMENT_OSD_TIMEʱ��u.OsdTime��Ч��
    //����������STREAM_SEGMENT_MOSAICʱ��u.Mosaic��Ч��
    unsigned int                nSegmentType : 5;

    //������
    unsigned int                nReverved : 16;

    union SEGMENT_TYPE
    {
        //�Զ����ַ�����
        struct OSD_STRING
        {
            unsigned short      x;                  //x���꣬������Ϊ��λ
            unsigned short      y;                  //y���꣬������Ϊ��λ
            unsigned int        nReserved0;         //����
            unsigned short      nReserved1;         //����
            char                str[1];             //�Զ����ַ�����������'\0'����
        } OsdString;

        //ʱ����Ϣ��
        //ʹ��STREAM_VIDEO_HEADER�ṹ���е�nTimeStampLow��nTimeStampHight��
        //nTimeStampMillisecond��Ϊʱ����Ϣ��
        struct OSD_TIME
        {
            unsigned short      x;                  //x���꣬������Ϊ��λ
            unsigned short      y;                  //y���꣬������Ϊ��λ
            unsigned int        nReserved0;         //����
            unsigned short      nReserved1;         //����
            unsigned char       nFormat;            //ʱ���ʽ��ʹ��STREAM_TIME_FORMAT_*
        } OsdTime;

        //�����ˡ�
        struct MOSAIC
        {
            unsigned short      x;                  //x���꣬������Ϊ��λ
            unsigned short      y;                  //y���꣬������Ϊ��λ
            unsigned short      nWidth;             //�����˵Ŀ��
            unsigned short      nHeight;            //�����˵ĸ߶�
            unsigned char       nFillMode;          //���ģʽ��ʹ��STREAM_MOSAIC_FILL_*
        } Mosaic;
    } u;

} STREAM_SEGMENT_HEADER, *PSTREAM_SEGMENT_HEADER;


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_IsHeaderFlag
// �������ж��Ƿ���֡ͷ��־��
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
// ����ֵ��
//  ����0���ǺϷ���֡ͷ��־�����ط�0�ǺϷ���֡ͷ��־��
// ˵����
//  
////////////////////////////////////////////////////////////////////////////////
static int Stream_IsHeaderFlag( const STREAM_HEADER* pStreamHeader )
{
    return (STREAM_HEADER_FLAG == pStreamHeader->nHeaderFlag);
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_IsVideoFrame
// �������ж��Ƿ�����Ƶ֡��
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
// ����ֵ��
//  ����0������Ƶ֡�����ط�0����Ƶ֡��
// ˵����
////////////////////////////////////////////////////////////////////////////////
static int Stream_IsVideoFrame( const STREAM_HEADER* pStreamHeader )
{
    return (STREAM_FRAME_VIDEO_P == pStreamHeader->nFrameType
        || STREAM_FRAME_VIDEO_I == pStreamHeader->nFrameType
        || STREAM_FRAME_VIDEO_B == pStreamHeader->nFrameType
        || STREAM_FRAME_VIDEO_E == pStreamHeader->nFrameType);
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_IsAudioFrame
// �������ж��Ƿ�����Ƶ֡��
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
// ����ֵ��
//  ����0������Ƶ֡�����ط�0����Ƶ֡��
// ˵����
////////////////////////////////////////////////////////////////////////////////
static int Stream_IsAudioFrame( const STREAM_HEADER* pStreamHeader )
{
    return (STREAM_FRAME_AUDIO == pStreamHeader->nFrameType);
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_GetVideoHeaderAddress
// ��������ȡ��Ƶ֡ͷ�ĵ�ַ��
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
// ����ֵ��
//  ������Ƶ֡ͷ�ĵ�ַ��
// ˵����
//  ��֡�����ǺϷ�����Ƶ֡��
////////////////////////////////////////////////////////////////////////////////
static PSTREAM_VIDEO_HEADER Stream_GetVideoHeaderAddress( 
    const STREAM_HEADER* pStreamHeader )
{
    return (PSTREAM_VIDEO_HEADER)((char*)pStreamHeader + sizeof(STREAM_HEADER));
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_GetSegmentHeaderAddress
// ��������ȡ����Ϣ�ĵ�ַ��
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
//  [in]nIndex - ����ţ�Ӧ��С��STREAM_HEADER.nSegmentCount��
// ����ֵ��
//  ���ض���Ϣ�ĵ�ַ��
// ˵����
////////////////////////////////////////////////////////////////////////////////
static PSTREAM_SEGMENT_HEADER Stream_GetSegmentHeaderAddress( 
    const STREAM_HEADER* pStreamHeader, unsigned int nIndex )
{
    if ( nIndex < pStreamHeader->nSegmentCount )
    {
        PSTREAM_SEGMENT_HEADER pSegment;
        char* pAddress;
        unsigned int i;

        if ( Stream_IsVideoFrame( pStreamHeader ) )
        {
            PSTREAM_VIDEO_HEADER pVideoHeader = Stream_GetVideoHeaderAddress( 
                pStreamHeader );
            pAddress = (char*)pVideoHeader + pVideoHeader->nVideoHeaderSize;
        }
        else
        {
            pAddress = (char*)pStreamHeader + sizeof(STREAM_HEADER);
        }

        for ( i = 0; i <= nIndex; ++i )
        {
            pSegment  = (PSTREAM_SEGMENT_HEADER)pAddress;
            pAddress += pSegment->nSegmentSize;     //��һ�ε��׵�ַ
        }

        return pSegment;
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_GetEncodeDataAddress
// ��������ȡ�������ݵĵ�ַ��
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
// ����ֵ��
//  ���ر������ݵĵ�ַ��
// ˵����
////////////////////////////////////////////////////////////////////////////////
static void* Stream_GetEncodeDataAddress( const STREAM_HEADER* pStreamHeader )
{
    return (void*)((char*)pStreamHeader + pStreamHeader->nHeaderSize);
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_GenerateChecksum
// ����������һ֡���ݵ�У��͡�
// ������
//  [in]pStreamHeader - STREAM_HEADER�ṹ���ַ������ΪNULL��
//  [in]pDataBuffer - �������ݵĵ�ַ������ΪNULL��
//  [in]nEncodeDataSize - �������ݵĳ��ȣ����ֽ�Ϊ��λ��
// ����ֵ��
//  ����0ʧ�ܣ����ط�0ֵ�ǺϷ���У��͡�
// ˵����
//  У�����һ��24λ���޷���������
////////////////////////////////////////////////////////////////////////////////
static unsigned int Stream_GenerateChecksum( const STREAM_HEADER* pStreamHeader, 
    const void* pDataBuffer, unsigned int nEncodeDataSize )
{
    const unsigned char* pValue;
    unsigned int nLength, i, nChecksum;

    if ( pStreamHeader->nEncodeDataSize != nEncodeDataSize )
    {
        return 0;
    }

    //��0��ʼ�ۼ�У��͡�
    //��У��͵��㷨�����Ժܵͣ�ֻ�ܽϵ�ǿ����֤���ݵ���ȷ�ԡ�
    nChecksum = 0;

    //����STREAM_VIDEO_HEADER��STREAM_HEADER_SEGMENT[0 : n-1]��У��͡�
    if ( pStreamHeader->nHeaderSize > sizeof(STREAM_HEADER) )
    {
        pValue = (const unsigned char*)pStreamHeader + sizeof(STREAM_HEADER);
        nLength = pStreamHeader->nHeaderSize - sizeof(STREAM_HEADER);
        for ( i = 0; i < nLength; ++i )
        {
            nChecksum += pValue[i];
        }
    }

    //����������ݵ�У��͡�
    nLength = pStreamHeader->nCheckedEncodeDataSize;
    if ( nLength * 2 < nEncodeDataSize )
    {
        //ǰһ���֡�
        pValue = (const unsigned char*)pDataBuffer;
        for ( i = 0; i < nLength; ++i )
        {
            nChecksum += pValue[i];
        }

        //��һ���֡�
        pValue = (const unsigned char*)pDataBuffer + (nEncodeDataSize - nLength);
        for ( i = 0; i < nLength; ++i )
        {
            nChecksum += pValue[i];
        }
    }
    else
    {
        //У��ȫ������
        pValue = (const unsigned char*)pDataBuffer;
        for ( i = 0; i < nEncodeDataSize; ++i )
        {
             nChecksum += pValue[i];
        }
    }

    nChecksum &= 0x00FFFFFF;                        //У��͵�24λ��Ч
    nChecksum |= 0x00800000;                        //��֤У��Ͳ�Ϊ0

    return nChecksum;
}


////////////////////////////////////////////////////////////////////////////////
// ��������Stream_IsValidFrame
// �������ж��Ƿ���������֡��
// ������
//  [in]pBuffer - ��������ַ������ΪNULL��
//  [in]nBufferSize - �������ĳ��ȣ����ֽ�Ϊ��λ��
// ����ֵ��
//  ����0�ǷǷ���֡�����ط�0�ǺϷ���֡��
// ˵����
////////////////////////////////////////////////////////////////////////////////
static int Stream_IsValidFrame( const void* pBuffer, unsigned int nBufferSize )
{
    const STREAM_HEADER* pStreamHeader = (const STREAM_HEADER*)pBuffer;

    if ( nBufferSize < sizeof(STREAM_HEADER) )      //�жϻ����������Ƿ��㹻
    {
        return 0;
    }

    if ( !Stream_IsHeaderFlag( pStreamHeader ) )    //�ж�֡ͷ��־
    {
        return 0;
    }

    if ( pStreamHeader->nHeaderSize < sizeof(STREAM_HEADER) )
    {
        return 0;
    }

    if ( 0 == pStreamHeader->nEncodeDataSize )      //�����б�������
    {
        return 0;
    }

    //�жϻ����������Ƿ��㹻
    if ( nBufferSize < pStreamHeader->nHeaderSize + pStreamHeader->nEncodeDataSize )
    {
        return 0;
    }

    if ( 0 == pStreamHeader->nChecksum )            //У��Ͳ���Ϊ0
    {
        return 0;
    }

    void* pData = Stream_GetEncodeDataAddress( pStreamHeader );
    unsigned int nChecksum = Stream_GenerateChecksum( pStreamHeader, 
        pData, pStreamHeader->nEncodeDataSize );
    if ( nChecksum != pStreamHeader->nChecksum )    //У��ͱ������
    {
        return 0;
    }

    return 1;                                       //�Ϸ�����Ƶ����Ƶ֡
}


#ifdef __cplusplus
}
#endif

#endif  // HB_STREAM_HEADER_H