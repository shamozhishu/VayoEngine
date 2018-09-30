//#include "stdafx.h"
//
#ifndef HEADER_RC5_HPP
#define HEADER_RC5_HPP

#include <string.h>
#include "rc5.h"

#define RC5_MAX_BUFF_LEN 1024*64 //64kb

static int RC5Encode(unsigned char* key,unsigned int keyLen,unsigned int rounds,unsigned char* srcData,unsigned int srcLen,unsigned char* dstData,unsigned int& dstLen)
{
	if (!srcData || !dstData)
	{
		return -1;
	}
	unsigned char pSrcData[RC5_MAX_BUFF_LEN] = {};
	unsigned int len = srcLen + 4;
	memcpy(pSrcData+4,srcData,srcLen);
	memcpy(pSrcData,&srcLen,4);
	RC5_32_KEY rc5_key;
	RC5_32_set_key(&rc5_key,keyLen,key,rounds);
	unsigned int rc5_size = 8;
	unsigned char input[8],output[8];
	dstLen = 0;
	while (dstLen < len)
	{
		memset(input,0,rc5_size);
		memset(output,0,rc5_size);
		memcpy(input,pSrcData+dstLen,rc5_size);
		RC5_32_ecb_encrypt(input,output,&rc5_key,RC5_ENCRYPT);
		memcpy(dstData+dstLen,output,rc5_size);
		dstLen += rc5_size;
	}
	return 0;
}


static int RC5Decode(unsigned char* key,unsigned int keyLen,unsigned int rounds,unsigned char* srcData,unsigned int srcLen,unsigned char* dstData,unsigned int& dstLen)
{
	if (!srcData || !dstData)
	{
		return -1;
	}
	unsigned char pDstData[RC5_MAX_BUFF_LEN] = {};
	unsigned int len = 0;
	RC5_32_KEY rc5_key;
	RC5_32_set_key(&rc5_key,keyLen,key,rounds);
	unsigned int rc5_size = 8;
	unsigned char input[8],output[8];
	while (len < srcLen)
	{
		memset(input,0,rc5_size);
		memset(output,0,rc5_size);
		memcpy(input,srcData+len,rc5_size);
		RC5_32_ecb_encrypt(input,output,&rc5_key,RC5_DECRYPT);
		memcpy(pDstData+len,output,rc5_size);
		len += rc5_size;
	}
	memcpy(&dstLen,pDstData,4);
	memcpy(dstData,pDstData+4,dstLen);
	return 0;
}

static int RC5_EncDec(unsigned char* key,unsigned int keyLen,unsigned int rounds,unsigned char* data,unsigned int dataLen,bool bEncDec)
{
	if (!data)
	{
		return -1;
	}
	RC5_32_KEY rc5_key;
	RC5_32_set_key(&rc5_key,keyLen,key,rounds);
	unsigned int rc5_size = 8;
	unsigned int len = 0;
	while (len < dataLen)
	{
		if (bEncDec)
		{
			RC5_32_encrypt((unsigned int*)&data[len],&rc5_key);
		}else
		{
			RC5_32_decrypt((unsigned int*)&data[len],&rc5_key);
		}
		len += rc5_size;
	}
	return len;
}

#endif
