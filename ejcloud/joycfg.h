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
 * 定义AIRKISS_ENABLE_CRYPT为1以启用AirKiss加密功能
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
 * 对AirKiss库进行配置，目前仅定义了一些回调函数
 */
typedef struct
{
	/*
	 * 为尽量减少库文件体积，如下c标准库函数需要上层使用者提供
	 * 其中printf可以为NULL
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
 * joycfg  API工作需要的结构体，必须为全局变量或者通过malloc动态分配
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

  uint16_t multicastMaxNum;//组播地址第四字节的最大值

  uint16_t multicastOffset;//指向目的地址的偏移?4byte :DES :BSSID:SOURCE

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
	uint8_t syncFirst;		// 首次同步字节
	uint8_t syncCount;		// 同步状态表征连续合法包次数, 已同步时表征是否可以持续获得包
	uint8_t syncStepPoint;		// 同步过程中阶跃点的位置
	uint8_t directTimerSkip;		// 直接跳过Timer的次数

	uint8_t broadcastVersion;		// 广播数据的版本
	uint8_t muticastVersion;		// 组播数据的版本

	uint8_t  mutiSeriesCount;		// 组播连续同步次数
	int8_t   mutiLastIndex;		// 上次组播地址索引
	uint8_t  broadIndex;		// 广播帧
	uint8_t  broadBuffer[5];		// 广播帧缓冲区

    uint8_t  lastIsUplink;		  // 上次合法数据是否是上行数据包
    uint16_t lastLength;		  // 上一个包的包长
    uint16_t lastUploadSeq; 	  // 最近的上行SEQ
    uint16_t lastDownSeq;	  // 最近的下行SEQ
  
    uint8_t  syncAppMac[6]; 	  // 锁定住的手机MAC地址
    uint8_t  syncBssid[6];		  // 锁定住的AP的SSID
    uint16_t syncUploadSeq; 	  // 锁定时的上行SEQ
    uint16_t syncDownSeq;		  // 锁定时的下行SEQ
    uint16_t syncIsUplink;		  // 是否锁定为上行数据包
  
    uint8_t chCurrentIndex; 	  // 当前处理的CH的Index
    uint8_t chCurrentProbability; // 当前信道是否有必要继续锁定
  	uint8_t isProbeReceived;		// 是否收到过合法的Probe包
	uint8_t flagProbe;		// bit0=passwd, bit1=ssid

	uint8_t crcNoEncode;
	uint8_t indexCrcNoEncode;
	uint8_t payload[128];		// 接收到的Payload数据
	joylinkResult_t result;		// 最终结论

}joycfg_context_t;

#define MAX_PWD_SSID (30)
/*
 * joycfg解码成功后的结果
 */
typedef struct
{
	char pwd[MAX_PWD_SSID];						/* wifi密码，以'\0'结尾 */
	char ssid[MAX_PWD_SSID];						/* wifi ssid，以'\0'结尾 */
	unsigned char pwd_length;		/* wifi密码长度 */
	unsigned char ssid_length;		/* wifi ssid长度 */
	unsigned char random;			/* 随机值，根据AirKiss协议，当wifi连接成功后，需要通过udp向10000端口广播这个随机值，这样AirKiss发送端（微信客户端或者AirKissDebugger）就能知道AirKiss已配置成功 */
	unsigned char reserved;			/* 保留值 */
} joycfg_result_t;



/*
 * joycfg_recv()正常情况下的返回值
 */
typedef enum
{
	/* 解码正常，无需特殊处理，继续调用airkiss_recv()直到解码成功 */
	JOYCFG_STATUS_CONTINUE = 0,

	/* wifi信道已经锁定，上层应该立即停止切换信道 */
	JOYCFG_STATUS_CHANNEL_LOCKED = 1,

	/* 解码成功，可以调用joycfg_get_result()取得结果 */
	JOYCFG_STATUS_COMPLETE = 2

} joycfg_status_t;



#if JOYCFG_ENABLE_CRYPT

/*
 * 设置解密key，最长可以为128bit，若传入的key不足128bit，则默认用0填充
 *
 * 返回值
 * 		< 0：出错，通常是参数错误
 * 		  0：成功
 */
int joycfg_set_key(joycfg_context_t* context, const unsigned char* key, unsigned int length);

#endif



/*
 * 获取AirKiss库版本信息
 */
const char* joycfg_version(void);



/*
 * 初始化AirKiss库，如要复用context，可以多次调用
 *
 * 返回值
 * 		< 0：出错，通常是参数错误
 * 		  0：成功
 */





/*
 * 开启WiFi Promiscuous Mode后，将收到的包传给airkiss_recv以进行解码
 *
 * 参数说明
 * 		frame：802.11 frame mac header(must contain at least first 24 bytes)
 * 		length：total frame length
 *
 * 返回值
 * 		 < 0：出错，通常是参数错误
 * 		>= 0：成功，请参考airkiss_status_t
 */



/*
 * 当airkiss_recv()返回AIRKISS_STATUS_COMPLETE后，调用此函数来获取AirKiss解码结果
 *
 * 返回值
 * 		< 0：出错，解码状态还不是AIRKISS_STATUS_COMPLETE
 * 		  0：成功
 */
//int joycfg_get_result(joycfg_context_t* context, joycfg_result_t* result);


/*
 * 上层切换信道以后，可以调用一下本接口清缓存，降低锁定错信道的概率，注意调用的逻辑是在airkiss_init之后
 *
 * 返回值
 * 		< 0：出错，通常是参数错误
 * 		  0：成功
 */
int joycfg_change_channel(joycfg_context_t* context);

/*
 *
 * 以上是实现智能配置网络的相关API，以下是微信内网发现相关API
 *
 */

/*
 * airkiss_lan_recv()的返回值
 */
typedef enum
{
	/* 提供的数据缓冲区长度不足 */
	JOYCFG_LAN_ERR_OVERFLOW = -5,

	/* 当前版本不支持的指令类型 */
	JOYCFG_LAN_ERR_CMD = -4,

	/* 打包数据出错 */
	JOYCFG_LAN_ERR_PAKE = -3,

	/* 函数传递参数出错 */
	JOYCFG_LAN_ERR_PARA = -2,

	/* 报文数据错误 */
	JOYCFG_LAN_ERR_PKG = -1,

	/* 报文格式正确，但是不需要设备处理的数据包 */
	JOYCFG_LAN_CONTINUE = 0,

	/* 接收到发现设备请求数据包 */
	JOYCFG_LAN_SSDP_REQ = 1,

	/* 数据包打包完成 */
	JOYCFG_LAN_PAKE_READY = 2


} joycfg_lan_ret_t;


typedef enum
{
	JOYCFG_LAN_SSDP_REQ_CMD = 0x1,
	JOYCFG_LAN_SSDP_RESP_CMD = 0x1001,
	JOYCFG_LAN_SSDP_NOTIFY_CMD = 0x1002
} joycfg_lan_cmdid_t;

/*
 * 设备进入内网发现模式后，将收到的包传给airkiss_lan_recv以进行解析
 *
 * 参数说明
 * 		body：802.11 frame mac header(must contain at least first 24 bytes)
 * 		length：total frame length
 * 		config：AirKiss回调函数
 *
 * 返回值
 * 		 < 0：出错，请参考airkiss_lan_ret_t，通常是报文数据出错
 * 		>= 0：成功，请参考airkiss_lan_ret_t
 */
//int joycfg_lan_recv(const void* body, unsigned short length, const joycfg_config_t* config);

/*
 * 设备要发送内网协议包时，调用本接口完成数据包打包
 *
 * 参数说明
 * 		body：802.11 frame mac header(must contain at least first 24 bytes)
 * 		length：total frame length
 * 		config：AirKiss回调函数
 *
 * 返回值
 * 		 < 0：出错，请参考airkiss_lan_ret_t，通常是报文数据出错
 * 		>= 0：成功，请参考airkiss_lan_ret_t
 */
//int joycfg_lan_pack(joycfg_lan_cmdid_t ak_lan_cmdid, void* appid, void* deviceid, void* _datain, unsigned short inlength, void* _dataout, unsigned short* outlength, const joycfg_config_t* config);

#ifdef __cplusplus
}
#endif

#endif 

