
httpclient_t _s_dev_ota_httpclient = {0};


#define DEV_OTA_BUF_SIZE  1024

int device_download_by_http(char *url)
{

		int32_t ret = HTTPCLIENT_ERROR_CONN;//-1
		
		//1.
		char* buf = pvPortMalloc(DEV_OTA_BUF_SIZE);
		if (buf == NULL) {
		    LOG_E(fota_dl_api, "[FOTA DL] buf malloc failed.\r\n");
		    return -3;
		}

		//2.
		ret = httpclient_connect(&_s_dev_ota_httpclient, url);
	    if (!ret) {
	        ret = _dev_ota_http_retrieve_get(url, buf, DEV_OTA_BUF_SIZE);
	    }else {
	        LOG_E(fota_dl_api, "[FOTA DL] http client connect error. \r");
	    }


		//3.
		httpclient_close(&_s_dev_ota_httpclient);
	    vPortFree(buf);
	    buf = NULL;
	    if ( HTTPCLIENT_OK == ret) {
	        return 0;
	    } else {
	        return -1;
	    }


}

static int32_t _dev_ota_http_retrieve_get(char* _url, char* buf, uint32_t len)
{
	int32_t ret = HTTPCLIENT_ERROR_CONN;//-1
	httpclient_data_t client_data = {0};
	client_data.response_buf = buf;
    client_data.response_buf_len = len;


	uint32_t recv_temp = 0;
	uint32_t data_len = 0;
	
	//1. 发送下载请求
	ret = httpclient_send_request(&_s_dev_ota_httpclient, _url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        
        LOG_E(fota_dl_api, "[FOTA DL] http client fail to send request \n");
        return ret;
    }


	//2. 分次接收数据
	do {
        ret = httpclient_recv_response(&_s_dev_ota_httpclient, &client_data);
        if (ret < 0) {
            LOG_E(fota_dl_api, "[FOTA DL] http client recv response error, ret = %d \n", ret);
            return ret;
        }

        if (recv_temp == 0)
        {
            recv_temp = client_data.response_content_len;//第一次记录将接收的总包长
        }

        LOG_I(fota_dl_api, "[FOTA DL] retrieve_len = %d \n", client_data.retrieve_len);
        
        data_len = recv_temp - client_data.retrieve_len;  // 本次收到后剩余的数据长度，即得到本次接收到的数据长度data_len
        LOG_I(fota_dl_api, "[FOTA DL] data_len = %u \n", data_len);
        
        count += data_len;   //count 总的接收长度
        recv_temp = client_data.retrieve_len;// recv_temp  剩余接收长度
        
        //vTaskDelay(100);/* Print log may block other task, so sleep some ticks */
        LOG_I(fota_dl_api, "[FOTA DL] total data received %u \n", count);


		//将本次接收到的数据写入到flash
        write_ret = fota_write(FOTA_PARITION_TMP, (const uint8_t*)client_data.response_buf, data_len);
        if (FOTA_STATUS_OK != write_ret) {
            LOG_E(fota_dl_api, "[FOTA DL] fail to write flash, write_ret = %d \n", write_ret);
            return ret;
        }

        LOG_I(fota_dl_api, "[FOTA DL] download progrses = %u \n", count * 100 / client_data.response_content_len);
        
    } while (ret == HTTPCLIENT_RETRIEVE_MORE_DATA);// 1



	//3.检验接收长度和会话结果验证是否接收完成
	LOG_I(fota_dl_api, "[FOTA DL] total length: %d \n", client_data.response_content_len);
    if (count != client_data.response_content_len || httpclient_get_response_code(&_s_dev_ota_httpclient) != 200) {
        LOG_E(fota_dl_api, "[FOTA DL] data received not completed, or invalid error code \r\n");
        return -1;
    }
    else if (count == 0) {
        LOG_E(fota_dl_api, "[FOTA DL] receive length is zero, file not found \n");
        return -2;
    }
    else {
        LOG_I(fota_dl_api, "[FOTA DL] download success \n");
        return ret;
    }
	
}

