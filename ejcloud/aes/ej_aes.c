//#include <wm_os.h>
//#include <mdev_aes.h>
#include "ej_aes.h"



 uint8_t IV[] = 
{
    0x12,0x34,0x56,0x78,0x90,0xab,0xcd,0xef,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x11,0x21,0x31,0x41,0x51,0x61,0x71,0x81
    
};

 uint8_t KEK[16] = {0x97, 0xC4, 0x4A, 0xEF, 
				0xAA, 0x34, 0x02, 0x5A,
				0x4E, 0x41, 0x11, 0xE8, 
				0x44, 0x4A, 0x9F, 0xBB};

 uint8_t LANKEK[16] = {0x5E, 0x78, 0xB5, 0x58, 
				0x06, 0x26, 0xD6, 0x65,
				0x83, 0x4E, 0xBB, 0x4D, 
				0xAA, 0x78, 0xE7, 0x1C};


 uint8_t UDPKEK[16] = {0x37,0x57,0x25,0x39,
				0x32,0x76,0x36,0x41,
				0x63,0x47,0x33,0x21,
				0x2A,0x65,0x24,0x5E};
 

 hal_aes_buffer_t key = {
			.buffer = KEK,
			.length = sizeof(KEK)
};


static hal_aes_buffer_t lankey = {
	.buffer = LANKEK,
	.length = sizeof(LANKEK)
};

static void aes_result_dump(uint8_t *result, uint8_t length)
{
    uint8_t i;
	printf("aes_result_dump:\r\n");
    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf(" %02x ", result[i]);
    }
    printf("\r\n");

}
static void md5_result_dump(uint8_t *result, uint8_t length)
{
    uint8_t i;
	printf("MD5 result:");
    for (i = 0; i < length; i++) {
        if (i % 16 == 0) {
            printf("\r\n");
        }

        printf(" %02x ", result[i]);
    }
    printf("\r\n");

}



int EJ_Aes_Encrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt)
{

	hal_aes_buffer_t encrypted_text = {
			.buffer = pCipTxt,
			.length = (TextLen/16 + 1)*16
//			.length = sizeof(pCipTxt)
	};
	
    hal_aes_buffer_t plain_text = {
            .buffer = pPlainTxt,
            .length = TextLen
    };	
	
	if( -1==hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key))
		return -1;
	//aes_result_dump(encrypted_text.buffer, encrypted_text.length);
	return encrypted_text.length;
	

	
}

int EJ_Aes_Decrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt)
{

	printf("EJ_Aes_Decrypt1: CipTxtLen = %d\r\n",CipTxtLen);
	aes_result_dump(pCipTxt, CipTxtLen);

	uint8_t encrypted_buffer[256] = {0};
    //uint8_t decrypted_buffer[256] = {0};
	memcpy(encrypted_buffer,pCipTxt,CipTxtLen);
	
	hal_aes_buffer_t encrypted_text = {
			.buffer = encrypted_buffer,
			.length = CipTxtLen
	};
	
    hal_aes_buffer_t decrypted_text = {
            .buffer = pPlainTxt,
            .length = CipTxtLen
    };

	printf("EJ_Aes_Decrypt2: enc_len = %d, dec_len =%d\r\n",encrypted_text.length,decrypted_text.length);
	
	
	if (-1 == hal_aes_ecb_decrypt(&decrypted_text, &encrypted_text, &key)) {

			return -1;
		}

	CipTxtLen = decrypted_text.length;
	printf("EJ_Aes_Decrypt3: enc_len = %d, dec_len =%d\r\n",encrypted_text.length,decrypted_text.length);

	//memcpy(pPlainTxt,decrypted_buffer,decrypted_text.length);
	return 0;

}


int EJ_Aes_udpBroadcast_Decrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt)
{

	return 0;

	
}


int EJ_Aes_udpBroadcast_Encrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt)
{
	
	
	return 0;
	
}


int EJ_Aes_lanMessageEncrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt)
{
	return 0;

}

int EJ_Aes_lanMessageDecrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt)
{

	
	return 0;
}





void EJ_hash_md5(uint8_t *input, int len, uint8_t *hash, int hlen)
{
	hal_md5_context_t context = {{0}};
    hal_md5_init(&context);
    hal_md5_append(&context, input, len);
    hal_md5_end(&context, hash);
   // md5_result_dump(hash, hlen);
	return ;
}



int  EJ_Aes_init()
{
	return 0;
}


