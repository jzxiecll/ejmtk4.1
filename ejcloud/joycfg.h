#ifndef __JOYCFG_H_
#define __JOYCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

//#include "joycfg_wlan.h"

/** WLAN Beacon info
 *
 *  This data structure represents a WLAN Beacon info.
 */

#ifndef PACK_START
#define PACK_START
#endif
#ifndef PACK_END
#define PACK_END __attribute__((packed))
#endif

#define MLAN_MAC_ADDR_LENGTH (6)
#define MLAN_MAX_SSID_LENGTH (32)

/** WLAN Beacon info
 *
 *  This data structure represents a WLAN Beacon info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** Time stamp */
	uint8_t timestamp[8];
	/** Beacon Interval */
	uint16_t beacon_interval;
	/** Capability Information */
	uint16_t cap_info;
	/** SSID Element ID = 00 */
	uint8_t ssid_element_id;
	/** SSID Length */
	uint8_t ssid_len;
	/** SSID */
	char ssid[MLAN_MAX_SSID_LENGTH];
}  PACK_END wlan_beacon_info_t;

/** WLAN Probe Request info
 *
 *  This data structure represents a WLAN Probe Request info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** SSID Element ID = 00 */
	uint8_t ssid_element_id;
	/** SSID Length */
	uint8_t ssid_len;
	/** SSID */
	char ssid[MLAN_MAX_SSID_LENGTH];
}  PACK_END wlan_probe_req_info_t;

/** WLAN Probe Request info
 *
 *  This data structure represents a WLAN Probe Request info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char dest[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char src[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char bssid[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
}  PACK_END wlan_auth_info_t;

/** WLAN Data info
 *
 *  This data structure represents a WLAN Data info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** Destination MAC Address */
	char Addr1[MLAN_MAC_ADDR_LENGTH];
	/** BSSID MAC Address */
	char Addr2[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char Addr3[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** QoS Control bits */
	uint16_t qos_ctrl;
} PACK_END wlan_data_info_t;


/** WLAN Qos Data info
 *
 *  This data structure represents a WLAN Data info.
 */
typedef PACK_START struct {
	/** Frame Control flags */
	uint8_t frame_ctrl_flags;
	/** Duration */
	uint16_t duration;
	/** BSSID MAC Address */
	char Addr1[MLAN_MAC_ADDR_LENGTH];
	/** Source MAC Address */
	char Addr2[MLAN_MAC_ADDR_LENGTH];
	/** Destination MAC Address */
	char Addr3[MLAN_MAC_ADDR_LENGTH];
	/** Sequence and Fragmentation number */
	uint16_t seq_frag_num;
	/** QoS Control bits */
	uint16_t qos_ctrl;
} PACK_END wlan_qos_data_info_t;

/** WLAN Frame type */
typedef enum {
	/** Beacon */
	BEACON = 0x80,
	/** Probe Request */
	PROBE_REQ = 0x40,
	/** Auth Request */
	AUTH = 0xB0,
	/** Data */
	DATA = 0x08,
	/** QOS Data */
	QOS_DATA = 0x88,
} wlan_frame_type_t;

/** WLAN Frame info
 *
 *  This data structure represents a WLAN Frame info.
 */
typedef  struct {
	/** WLAN Frame type */
	wlan_frame_type_t frame_type;
	union {
		/** WLAN Beacon info */
		wlan_beacon_info_t beacon_info;
		/** WLAN Probe Request info */
		wlan_probe_req_info_t probe_req_info;
		/** WLAN Auth info */
		wlan_auth_info_t auth_info;
		/** WLAN Data info */
		wlan_data_info_t data_info;
		/** WLAN Data info */
		wlan_qos_data_info_t qos_data_info;
	} frame_data;
} PACK_END wlan_frame_t;


//typedef wifi_cal_data_t wlan_cal_data_t;

//typedef wifi_auto_reconnect_config_t wlan_auto_reconnect_config_t;





/*
 * ����AIRKISS_ENABLE_CRYPTΪ1������AirKiss���ܹ���
 */
#ifndef JOYCFG_ENABLE_CRYPT
#define JOYCFG_ENABLE_CRYPT 0
#endif

typedef void* (*joycfg_memfree_fn) (void *pv );
typedef void* (*joycfg_memset_fn) (void* ptr, int value, unsigned int num);
typedef void* (*joycfg_memcpy_fn) (void* dst, const void* src, unsigned int num);
typedef int (*joycfg_memcmp_fn) (const void* ptr1, const void* ptr2, unsigned int num);
typedef int (*joycfg_printf_fn) (const char* format, ...);


extern TimerHandle_t joy_lock_timer;


/*
 * ��AirKiss��������ã�Ŀǰ��������һЩ�ص�����
 */
typedef struct
{
	/*
	 * Ϊ�������ٿ��ļ����������c��׼�⺯����Ҫ�ϲ�ʹ�����ṩ
	 * ����printf����ΪNULL
	 */
	joycfg_memset_fn memset;
	joycfg_memcpy_fn memcpy;
	joycfg_memcmp_fn memcmp;
	joycfg_memfree_fn   memfree;
	joycfg_printf_fn printf;

} joycfg_config_t;

typedef enum JY_STATUS{
    JY_RCV_PRE,
    JY_RCV_DATA,
    JY_FIN,
}jy_status_t;


/*
 * joycfg  API������Ҫ�Ľṹ�壬����Ϊȫ�ֱ�������ͨ��malloc��̬����
 */
//typedef struct
//{
//	int dummyap[26];
//	int dummy[32];
//} joycfg_context_t;

//typedef struct {

//  uint8_t state;

//  uint8_t dsnCrc;

//  uint8_t lockedState;

//  uint8_t syncFirst;

//  uint8_t syncCount;

//  uint8_t syncStepPoint;

//  unsigned short lastLength;

//  SequenceList	sequenceList;

//  MagicCode	 magicCodeData;

//  uint8_t lockedBSSID[6];
//  
//}ejConfig_broadcast_context_t;


typedef struct {

  //uint8_t state;

  uint8_t mutiLastIndex;

  uint8_t ssidCrc;

  uint8_t dsnCrc;

  uint16_t multicastMaxNum;//�鲥��ַ�����ֽڵ����ֵ

  uint16_t multicastOffset;//ָ��Ŀ�ĵ�ַ��ƫ��?4byte :DES :BSSID:SOURCE

  uint16_t multicastAdd5[4];

  uint8_t lockedBSSID[6];

  uint8_t lockedSRC[6];

  uint8_t dataBuf[128];

  uint64_t multicastDataFlag;

}joycfg_multicast_context_t;



typedef struct {

  char ssid[32 + 1];

  char bssid[6 + 1];

  uint8_t channel;

}SSID_Desc;

typedef struct _joylinkResult
{
	unsigned char type;	                        // 0:NotReady, 1:ControlPacketOK, 2:BroadcastOK, 3:MulticastOK
    unsigned char encData[1+1+32+6+32];            // length + EncodeData
}joylinkResult_t;


typedef struct {

//  ejConfig_broadcast_context_t broadcastContext;
    uint8_t state;
	uint8_t syncFirst;		// �״�ͬ���ֽ�
	uint8_t syncCount;		// ͬ��״̬���������Ϸ�������, ��ͬ��ʱ�����Ƿ���Գ�����ð�
	uint8_t syncStepPoint;		// ͬ�������н�Ծ���λ��
	uint8_t directTimerSkip;		// ֱ������Timer�Ĵ���

	uint8_t broadcastVersion;		// �㲥���ݵİ汾
	uint8_t muticastVersion;		// �鲥���ݵİ汾

	uint8_t  mutiSeriesCount;		// �鲥����ͬ������
	int8_t   mutiLastIndex;		// �ϴ��鲥��ַ����
	uint8_t  broadIndex;		// �㲥֡
	uint8_t  broadBuffer[5];		// �㲥֡������

    uint8_t  lastIsUplink;		  // �ϴκϷ������Ƿ����������ݰ�
    uint16_t lastLength;		  // ��һ�����İ���
    uint16_t lastUploadSeq; 	  // ���������SEQ
    uint16_t lastDownSeq;	  // ���������SEQ
  
    uint8_t  syncAppMac[6]; 	  // ����ס���ֻ�MAC��ַ
    uint8_t  syncBssid[6];		  // ����ס��AP��SSID
    uint16_t syncUploadSeq; 	  // ����ʱ������SEQ
    uint16_t syncDownSeq;		  // ����ʱ������SEQ
    uint16_t syncIsUplink;		  // �Ƿ�����Ϊ�������ݰ�
  
    uint8_t chCurrentIndex; 	  // ��ǰ�����CH��Index
    uint8_t chCurrentProbability; // ��ǰ�ŵ��Ƿ��б�Ҫ��������
  	uint8_t isProbeReceived;		// �Ƿ��յ����Ϸ���Probe��
	uint8_t flagProbe;		// bit0=passwd, bit1=ssid

	uint8_t crcNoEncode;
	uint8_t indexCrcNoEncode;
	uint8_t payload[128];		// ���յ���Payload����
	joylinkResult_t result;		// ���ս���

}joycfg_context_t;

#define MAX_PWD_SSID (30)
/*
 * joycfg����ɹ���Ľ��
 */
typedef struct
{
	char pwd[MAX_PWD_SSID];						/* wifi���룬��'\0'��β */
	char ssid[MAX_PWD_SSID];						/* wifi ssid����'\0'��β */
	unsigned char pwd_length;		/* wifi���볤�� */
	unsigned char ssid_length;		/* wifi ssid���� */
	unsigned char random;			/* ���ֵ������AirKissЭ�飬��wifi���ӳɹ�����Ҫͨ��udp��10000�˿ڹ㲥������ֵ������AirKiss���Ͷˣ�΢�ſͻ��˻���AirKissDebugger������֪��AirKiss�����óɹ� */
	unsigned char reserved;			/* ����ֵ */
} joycfg_result_t;



/*
 * joycfg_recv()��������µķ���ֵ
 */
typedef enum
{
	/* �����������������⴦����������airkiss_recv()ֱ������ɹ� */
	JOYCFG_STATUS_CONTINUE = 0,

	/* wifi�ŵ��Ѿ��������ϲ�Ӧ������ֹͣ�л��ŵ� */
	JOYCFG_STATUS_CHANNEL_LOCKED = 1,

	/* ����ɹ������Ե���joycfg_get_result()ȡ�ý�� */
	JOYCFG_STATUS_COMPLETE = 2

} joycfg_status_t;



#if JOYCFG_ENABLE_CRYPT

/*
 * ���ý���key�������Ϊ128bit���������key����128bit����Ĭ����0���
 *
 * ����ֵ
 * 		< 0������ͨ���ǲ�������
 * 		  0���ɹ�
 */
int joycfg_set_key(joycfg_context_t* context, const unsigned char* key, unsigned int length);

#endif



/*
 * ��ȡAirKiss��汾��Ϣ
 */
const char* joycfg_version(void);



/*
 * ��ʼ��AirKiss�⣬��Ҫ����context�����Զ�ε���
 *
 * ����ֵ
 * 		< 0������ͨ���ǲ�������
 * 		  0���ɹ�
 */





/*
 * ����WiFi Promiscuous Mode�󣬽��յ��İ�����airkiss_recv�Խ��н���
 *
 * ����˵��
 * 		frame��802.11 frame mac header(must contain at least first 24 bytes)
 * 		length��total frame length
 *
 * ����ֵ
 * 		 < 0������ͨ���ǲ�������
 * 		>= 0���ɹ�����ο�airkiss_status_t
 */



/*
 * ��airkiss_recv()����AIRKISS_STATUS_COMPLETE�󣬵��ô˺�������ȡAirKiss������
 *
 * ����ֵ
 * 		< 0����������״̬������AIRKISS_STATUS_COMPLETE
 * 		  0���ɹ�
 */
//int joycfg_get_result(joycfg_context_t* context, joycfg_result_t* result);


/*
 * �ϲ��л��ŵ��Ժ󣬿��Ե���һ�±��ӿ��建�棬�����������ŵ��ĸ��ʣ�ע����õ��߼�����airkiss_init֮��
 *
 * ����ֵ
 * 		< 0������ͨ���ǲ�������
 * 		  0���ɹ�
 */
int joycfg_change_channel(joycfg_context_t* context);

/*
 *
 * ������ʵ������������������API��������΢�������������API
 *
 */

/*
 * airkiss_lan_recv()�ķ���ֵ
 */
typedef enum
{
	/* �ṩ�����ݻ��������Ȳ��� */
	JOYCFG_LAN_ERR_OVERFLOW = -5,

	/* ��ǰ�汾��֧�ֵ�ָ������ */
	JOYCFG_LAN_ERR_CMD = -4,

	/* ������ݳ��� */
	JOYCFG_LAN_ERR_PAKE = -3,

	/* �������ݲ������� */
	JOYCFG_LAN_ERR_PARA = -2,

	/* �������ݴ��� */
	JOYCFG_LAN_ERR_PKG = -1,

	/* ���ĸ�ʽ��ȷ�����ǲ���Ҫ�豸��������ݰ� */
	JOYCFG_LAN_CONTINUE = 0,

	/* ���յ������豸�������ݰ� */
	JOYCFG_LAN_SSDP_REQ = 1,

	/* ���ݰ������� */
	JOYCFG_LAN_PAKE_READY = 2


} joycfg_lan_ret_t;


typedef enum
{
	JOYCFG_LAN_SSDP_REQ_CMD = 0x1,
	JOYCFG_LAN_SSDP_RESP_CMD = 0x1001,
	JOYCFG_LAN_SSDP_NOTIFY_CMD = 0x1002
} joycfg_lan_cmdid_t;

/*
 * �豸������������ģʽ�󣬽��յ��İ�����airkiss_lan_recv�Խ��н���
 *
 * ����˵��
 * 		body��802.11 frame mac header(must contain at least first 24 bytes)
 * 		length��total frame length
 * 		config��AirKiss�ص�����
 *
 * ����ֵ
 * 		 < 0��������ο�airkiss_lan_ret_t��ͨ���Ǳ������ݳ���
 * 		>= 0���ɹ�����ο�airkiss_lan_ret_t
 */
//int joycfg_lan_recv(const void* body, unsigned short length, const joycfg_config_t* config);

/*
 * �豸Ҫ��������Э���ʱ�����ñ��ӿ�������ݰ����
 *
 * ����˵��
 * 		body��802.11 frame mac header(must contain at least first 24 bytes)
 * 		length��total frame length
 * 		config��AirKiss�ص�����
 *
 * ����ֵ
 * 		 < 0��������ο�airkiss_lan_ret_t��ͨ���Ǳ������ݳ���
 * 		>= 0���ɹ�����ο�airkiss_lan_ret_t
 */
//int joycfg_lan_pack(joycfg_lan_cmdid_t ak_lan_cmdid, void* appid, void* deviceid, void* _datain, unsigned short inlength, void* _dataout, unsigned short* outlength, const joycfg_config_t* config);

#ifdef __cplusplus
}
#endif

#endif 

