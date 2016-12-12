#ifndef __EJ_WIFICONFIG_H__
#define __EJ_WIFICONFIG_H__

#include "ej_porting_layer.h"



#include <stdint.h>
#include "wifi_api.h"

#ifdef __cplusplus
extern "C" {
#endif




typedef enum {

	WIFICONFIG_START,
	WIFICONFIG_WAIT,
	WIFICONFIG_END,

};

typedef enum {

	WIFICONFIG_EZCONNECT_MODE,
	WIFICONFIG_AP_MODE,
	WIFICONFIG_AIRKISS_MODE,
	WIFICONFIG_NULL_MODE,

};


typedef enum _ENUM_MAC_RX_GROUP_VLD_T {
    RX_GROUP_VLD_1 = 0,
    RX_GROUP_VLD_2,
    RX_GROUP_VLD_3,
    RX_GROUP_VLD_4,
    RX_GROUP_VLD_NUM
} ENUM_MAC_RX_GROUP_VLD_T;

/* Word 1 */
#define RX_STATUS_GROUP_VLD_MASK        BITS(9,12)
#define RX_STATUS_GROUP_VLD_OFFSET      9
#define RX_STATUS_PKT_TYPE_MASK         BITS(13,15)
#define RX_STATUS_PKT_TYPE_OFFSET       13

/* Byte 2 */
#define RX_STATUS_HEADER_LEN_MASK           BITS(0,5)
#define RX_STATUS_HEADER_OFFSET             BIT(6)
#define RX_STATUS_HEADER_TRAN               BIT(7)

/* RX Vector, 3rd Cycle */
#define RX_VT_RCPI0_MASK       BITS(8,15)
#define RX_VT_RCPI0_OFFSET     8

/* DW 0 */
#define HAL_RX_STATUS_GET_RX_BYTE_CNT(_prHwMacRxDesc) ((_prHwMacRxDesc)->u2RxByteCount)
#define HAL_RX_STATUS_GET_GROUP_VLD(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_GROUP_VLD_MASK) >> RX_STATUS_GROUP_VLD_OFFSET)
#define HAL_RX_STATUS_GET_PKT_TYPE(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_PKT_TYPE_MASK) >> RX_STATUS_PKT_TYPE_OFFSET)

/* DW 1 */
#define HAL_RX_STATUS_GET_HEADER_LEN(_prHwMacRxDesc) ((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_LEN_MASK)
#define HAL_RX_STATUS_IS_HEADER_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_HEADER_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET)? 2 : 0)

/* DW 14~19 */
#define HAL_RX_STATUS_GET_RCPI(_prHwMacRxStsGroup3) (((_prHwMacRxStsGroup3)->u4RxVector[2] & RX_VT_RCPI0_MASK) >> RX_VT_RCPI0_OFFSET)

typedef struct _HW_MAC_RX_DESC_T {
    uint16_t    u2RxByteCount;       /* DW 0 */
    uint16_t    u2PktTYpe;
    uint8_t     ucMatchPacket;       /* DW 1 */
    uint8_t     ucChanFreq;
    uint8_t     ucHeaderLen;
    uint8_t     ucBssid;
    uint8_t     ucWlanIdx;           /* DW 2 */
    uint8_t     ucTidSecMode;
    uint16_t    u2StatusFlag;
    uint32_t    u4PatternFilterInfo; /* DW 3 */
}  HW_MAC_RX_DESC_T, *P_HW_MAC_RX_DESC_T;
typedef struct _HW_MAC_RX_STS_GROUP_1_T {
    uint8_t     aucPN[16];
} HW_MAC_RX_STS_GROUP_1_T, *P_HW_MAC_RX_STS_GROUP_1_T;
typedef struct _HW_MAC_RX_STS_GROUP_2_T {
    uint32_t    u4Timestamp;      /* DW 12 */
    uint32_t    u4CRC;            /* DW 13 */
} HW_MAC_RX_STS_GROUP_2_T, *P_HW_MAC_RX_STS_GROUP_2_T;
typedef struct _HW_MAC_RX_STS_GROUP_4_T {
    /* For HDR_TRAN */
    uint16_t    u2FrameCtl;          /* DW 4 */
    uint8_t     aucTA[6];            /* DW 4~5 */
    uint16_t    u2SeqFrag;           /* DW 6 */
    uint16_t    u2Qos;               /* DW 6 */
    uint32_t    u4HTC;               /* DW 7 */
} HW_MAC_RX_STS_GROUP_4_T, *P_HW_MAC_RX_STS_GROUP_4_T;
typedef struct _HW_MAC_RX_STS_GROUP_3_T {
    /*!  RX Vector Info*/
    uint32_t    u4RxVector[6];       /* DW 14~19 */
} HW_MAC_RX_STS_GROUP_3_T, *P_HW_MAC_RX_STS_GROUP_3_T;
/* Used for MAC RX */
typedef enum _ENUM_MAC_RX_PKT_TYPE_T {
    RX_PKT_TYPE_TX_STATUS = 0,
    RX_PKT_TYPE_RX_VECTOR,
    RX_PKT_TYPE_RX_DATA,
    RX_PKT_TYPE_DUP_RFB,
    RX_PKT_TYPE_TM_REPORT,
    RX_PKT_TYPE_SW_DEFINED = 7
} ENUM_MAC_RX_PKT_TYPE_T;

/**@defgroup wifi_smtcn_enum Enum
 *@{
 */

/** @brief Smart Connection event.
 */
typedef enum {
    WIFI_SMART_CONNECTION_EVENT_CHANNEL_LOCKED,  /**< Locked channel event. */
    WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED,  /**< The Smart Connection has finished. */
    WIFI_SMART_CONNECTION_EVENT_TIMEOUT,         /**< Smart Connection timeout. */
} wifi_smart_connect_event_t ;

/** @brief Return values for the Smart Connection API.
 */
typedef enum {
    WIFI_SMART_CONNECTION_ERROR = -1,      /**< The operation failed. */
    WIFI_SMART_CONNECTION_OK = 0           /**< The operation is successful. */
} wifi_smart_connect_status_t;

/**
 *@}
 */

/**
* @brief This callback function should be registered through the #wifi_smart_connection_init(), and will be called when a Smart Connection event is received.
* @param[in] event  is the Smart Connection event.
* @param[in] data is the reserved data.
*
* @return  None
*/
typedef void (* wifi_smart_connect_callback_t) (wifi_smart_connect_event_t event, void *data);


/**
* @brief This function registers a callback function to establish the Smart Connection.
*
* @param[in]  key is the user-defined security key, to decrypt the information received through the Smart Connection.
* @param[in]  key_length is the length of the security key.
* @param[in]  callback is a user-defined callback to handle the Smart Connection events.
*
* @return #WIFI_SMART_CONNECTION_OK, if the operation completed successfully.
*/
wifi_smart_connect_status_t wifi_smart_connect_init (const uint8_t *key, const uint8_t key_length, wifi_smart_connect_callback_t  callback);

/**
* @brief This function de-initializes the Smart Connection and releases the resources.
*
* @return  None
*/
void wifi_smart_connect_deinit (void);

/**
* @brief This function starts the Smart Connection. It should be called after #wifi_smart_connection_init() for pre-allocated resources.
*
* @param[in]  timeout_seconds is a user-defined timeout value in seconds.
* It implies smart connection failed once timeout is reached.
*
* @return #WIFI_SMART_CONNECTION_OK, if the operation completed successfully.
*/
wifi_smart_connect_status_t wifi_smart_connect_start (uint16_t timeout_seconds);

/**
* @brief This function gets the Smart Connection result. Call it once #WIFI_SMART_CONNECTION_EVENT_INFO_COLLECTED is finished, or it will return an error.
*
* @param[out]  ssid is a pointer to the memory where the SSID is stored.
* @param[out]  ssid_length is a pointer to the memory where the length of SSID is stored.
* @param[out]  password is a pointer to the memory where the password is stored.
* @param[out]  password_length is a pointer to the memory where the length of password length is stored.
* @param[out]  tlv_data is a pointer to the memory where the tlv data is stored.
* @param[out]  tlv_data_length is a pointer to the memory where the length of tlv data is stored.
*
* @return #WIFI_SMART_CONNECTION_OK, if the operation completed successfully.
*/
wifi_smart_connect_status_t wifi_smart_connect_get_result (uint8_t *ssid, uint8_t *ssid_length, 
                                            uint8_t *password, uint8_t *password_length, 
                                            uint8_t *tlv_data, uint8_t *tlv_data_length);


/**
* @brief This function is called to inform smart connection task of exiting flag and then the task will automatically release the resources.
*
* @return #WIFI_SMART_CONNECTION_OK, if the operation completed successfully.
*/
wifi_smart_connect_status_t wifi_smart_connect_stop(void);

int32_t wifi_smart_save_info(void);

int32_t joycfg_profile_get_configfree(uint8_t *config_ready);

int32_t joycfg_profile_set_configfree(uint8_t config_ready);





/**
 * @addtogroup Smart_Connection
 * @{
 * This document introduces MTK smart connection interfaces for upper applications.
 * It has wrapped the base interfaces of core.h, which supplies core functionality
 * of MTK elian protocol. MTK elian protocol is confidential and it doesn't
 * release its source code. The file smt_conn.c shows how to use these elian
 * APIs and WiFi APIs, such as switch channel, set rx filter, set raw packet
 * handler and so on.
 * You can replace MTK elian protocol and develop your own smart
 * config protocol easily.
 */


/**@defgroup SMTCN_DEFINE Define
 *@{
 */

/**@brief The value 2 specifies using MTK smart connection.
 *      Value 3 specifies using airkiss quick connect.
 */
#define CFG_SUPPORT_SMNT_PROTO    4

#if (CFG_SUPPORT_SMNT_PROTO == 2)              /* elian protocol*/
/**@brief Specifies timeout value that can remain in each channel to listen
 * packets from smart phone.
 */
#define switch_channel_timems   (200)          /* elian 200ms */
/**@brief Specifies timeout value that can receive data packets in which
 * channel has sniffered sync packets.
 */
#define locked_channel_timems   (5 * 1000)     /* 5s */
#define use_smart_connection_filter  1

#elif (CFG_SUPPORT_SMNT_PROTO == 3)            /* airkiss protocol */
#define switch_channel_timems   (100)          /* airkiss 100ms */
#define locked_channel_timems   (10 * 1000)    /* 10s */
#define use_smart_connection_filter  1
#elif (CFG_SUPPORT_SMNT_PROTO == 4)            /* airkiss protocol */
#define switch_channel_timems   (100)          /* airkiss 100ms */
#define locked_channel_timems   (10 * 1000)    /* 10s */
#define use_smart_connection_filter  1

#endif


/**@brief For FreeRTOS compatibility.
 */
#define tmr_nodelay         ( TickType_t ) 0

/**@brief After gained smart connection information, it will start a scan
 * process, this value specifies the maximum times to scan.
 */
#define max_scan_loop_count (5)

/**@brief Smart connection finish flag.
 */
#define SMT_FLAG_FIN  (0xAA)

/**@brief Smart connection fail flag.
 */
#define SMT_FLAG_FAIL  (0xAE)

/**@brief Smart connection stop flag.
 */
#define SMT_FLAG_EXIT  (0xAF)

/**@brief Scan finished flag, which means successfully scan the target AP
 * information.
 */
#define SCAN_FLAG_FIN (0xBB)

/**@brief Scan finish flag, which means not found the target AP information.
 */
#define SCAN_FLAG_NOT_FOUND (0xCC)

/**@brief for init purpose
 */
#define SCAN_FLAG_NONE   (0x00)

/**
 *@}
 */


/**@defgroup SMTCN_STRUCT Structure
*@{
*/

/** @brief smart connection result information.
*/
typedef struct {
    unsigned char                   pwd[WIFI_LENGTH_PASSPHRASE]; /**< store the password you got */
    unsigned char                   ssid[WIFI_MAX_LENGTH_OF_SSID]; /**< store the ssid you got */
    unsigned char                   pmk[WIFI_LENGTH_PMK]; /**< store the PMK if you have any */
    unsigned char                   *tlv_data;
    int                             tlv_data_len;
    wifi_auth_mode_t                auth_mode; /**< deprecated */
    unsigned char                   ssid_len; /**< ssid length */
    unsigned char                   pwd_len;  /**< password length */

    wifi_encrypt_type_t             encrypt_type; /**< Not used */
    unsigned char                   channel;      /**< Not used */
    unsigned char                   smtcn_flag;   /**< Flag to spicify whether smart connection finished. */
    unsigned char                   scan_flag;    /**< Flag to spicify whether scan finished. */
} smt_info;

/** @brief smart connection operations to adapt to specific protocol.
*/
typedef struct {
    int (*init)(const unsigned char *key, const unsigned char key_length);     /**< protocol specific initialize */
    void (*cleanup)(void); /**< protocol specific cleanup */
    int (*switch_channel_rst)(void);  /**< protocol specific reset operation when switch to next channel */
    int (*rx_handler)(char *, int);   /**< protocol specific packet handler */
} smt_proto_ops;

/**
 *@}
 */

/**
* @brief When have received the sync packets, you can use this function to stop
* switching channel. It means locked at the channel.
*
* @param[in]  None
*
* @return  None
*/
void smtcn_stop_switch(void);

/**
* @brief When locked channel timeout, you can use this function to continue
* to switch channel.
*
* @param[in]  None
*
* @return  None
*/
void smtcn_continue_switch(void);

/**
* @brief This function can be called to create a thread to process smart connection..
*
* @param[in]  None
*
* @return  0 means success, <0 means fail
*/
int32_t joylink_smart_connect(void);

/**
* @brief This function can be called to stop smart connection.
*
* @param[in]  None
*
* @return  0 means success, <0 means fail
*/
int32_t joylink_smart_exit(void);

int32_t joylink_smart_stop(void);

int32_t joylink_connection_disconnect_ap(void);


void EJ_WifiConfigProcess(unsigned char confgiMode);


#ifdef __cplusplus
}
#endif

/**
 * @}
 */
#endif //_H_

