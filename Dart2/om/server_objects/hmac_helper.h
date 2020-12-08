/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/

#ifndef _HMAC_HELPER_H__
#define _HMAC_HELPER_H__

#if defined(__cplusplus)
extern "C" {
#endif

uint8_t* dart_hmac_sha256(const void* key, int keylen,
        const uint8_t* data, size_t datalen, uint8_t* result,
        uint32_t* resultlen);
int decode_base64(const uint8_t* b64message, int blen, char** buffer);
int encode_base64(const uint8_t* message, int mlen, char** buffer) ;
#if defined(__cplusplus)
} 
#endif
#endif  //HMAC_HELPER_H__
