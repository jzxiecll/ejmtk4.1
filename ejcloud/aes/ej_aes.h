#ifndef __EJ_AES_H__
#define __EJ_AES_H__


#include "ej_porting_layer.h"

int EJ_Aes_Encrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt);
int EJ_Aes_Decrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt);

int EJ_Aes_udpBroadcast_Decrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt);
int EJ_Aes_udpBroadcast_Encrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt);

int EJ_Aes_lanMessageEncrypt(unsigned char *pPlainTxt, unsigned int TextLen, unsigned char *pCipTxt);
int EJ_Aes_lanMessageDecrypt(unsigned char *pCipTxt, unsigned int CipTxtLen, unsigned char *pPlainTxt);

void EJ_hash_md5(uint8_t *input, int len, uint8_t *hash, int hlen);

int  EJ_Aes_init();
#endif 



