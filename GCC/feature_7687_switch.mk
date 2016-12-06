IC_CONFIG                           = mt7687
BOARD_CONFIG                        = mt7687_hdk
MTK_FW_VERSION                      = mt7687_fw

##############################################
#           Custom feature option            #
##############################################

## part1: configure y/n

MTK_SYS_TRNG_ENABLE                 = y
MTK_LWIP_ENABLE                     = n
MTK_WIFI_API_TEST_CLI_ENABLE        =
MTK_WIFI_CONFIGURE_FREE_ENABLE      = n
MTK_WIFI_TGN_VERIFY_ENABLE          = n
MTK_SUPPORT_APP_TEST_ENABLE         =
MTK_LWIP_DYNAMIC_DEBUG_ENABLE       =
MTK_LWIP_STATISTICS_ENABLE          =
MTK_BSP_LOOPBACK_ENABLE             =
MTK_OS_CPU_UTILIZATION_ENABLE       = n
MTK_HEAP_GUARD_ENABLE               =
MTK_LED_ENABLE                      = y
MTK_AP_SNIFFER_ENABLE               =
MTK_SMTCN_ENABLE                    = y
MTK_BLE_SMTCN_ENABLE                =
MTK_MINICLI_ENABLE                  = y
MTK_WIFI_AP_ENABLE                  =
MTK_PING_OUT_ENABLE                 = y
MTK_IPERF_ENABLE                    = y
MTK_IPERF_DEBUG_ENABLE              =
MTK_BSPEXT_ENABLE                   = y
MTK_FOTA_ENABLE                     = y
MTK_TFTP_ENABLE                     = y
MTK_HAL_PLAIN_LOG_ENABLE            =
MTK_HAL_LOWPOWER_ENABLE             = y
MTK_HOMEKIT_ENABLE                  =
MTK_HOMEKIT_HAP_MOCK                =
MTK_HIF_GDMA_ENABLE                 =
MTK_HCI_CONSOLE_MIX_ENABLE          =
MTK_WIFI_FORCE_AUTOBA_DISABLE       =
MTK_WIFI_DIRECT_ENABLE              = n
MTK_WIFI_WPS_ENABLE                 = n
MTK_WIFI_REPEATER_ENABLE            = y
MTK_WIFI_PROFILE_ENABLE             = y
MTK_BLE_CLI_ENABLE                  =
MTK_BLE_BQB_CLI_ENABLE              =
MTK_BLE_BQB_TEST_ENABLE             =
MTK_LOAD_MAC_ADDR_FROM_EFUSE        = y
MTK_HTTPCLIENT_SSL_ENABLE           = y

## part2: need to set specified value

MTK_DEBUG_LEVEL                     = info    # info, warning, error, none
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-basic.h    # 3rd party option, no need to pay attention to verify it.

############################################
#               Internal use               #
############################################
MTK_MAIN_CONSOLE_UART2_ENABLE       = n
MTK_BSP_LOOPBACK_ENABLE             = n
MTK_SUPPORT_HEAP_DEBUG              = n
MTK_HEAP_SIZE_GUARD_ENABLE          = n
MTK_MPERF_ENABLE                    = n
MTK_MET_TRACE_ENABLE                = n
MTK_CODE_COVERAGE_ENABLE            = n
MTK_CLI_TEST_MODE_ENABLE            = y

MTK_MINICLI_ENABLE                  = y

