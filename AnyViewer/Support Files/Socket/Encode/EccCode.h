//
//  EccCode.h
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#ifndef EccCode_h
#define EccCode_h

#include <string>
#include "CDataPacket.h"

#ifndef BYTE
typedef unsigned char       BYTE;
#endif

#ifndef custom_min
#define custom_min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define ECC_KEY_LEN   24

/**************************  AES ***********************************/
#define AES_MIN_KEY_SIZE    16
#define AES_MAX_KEY_SIZE    32
#define AES_BLOCK_SIZE        16

#define AES_KEY_LEN            16
#define AES_ENCRYPT            0
#define AES_DECRYPT            1

struct aes_ctx{
    int key_length;
    unsigned int E[60];
    unsigned int D[60];
};

int aes_set_key(struct aes_ctx *ctx_arg, const unsigned char *in_key, unsigned int key_len);
int aes_aes(const struct aes_ctx *ctx, const unsigned char *in, int in_len, unsigned char *out, int type);

/*************************** AES end *******************************/

/*************************** ECC ***********************************/
#define ECC_BN_ENC_LEN    23
#define ECC_BN_DEC_LEN    24
#define ECC_ENC_ALIGN    (ECC_BN_ENC_LEN*2)
#define ECC_DEC_ALIGN    (ECC_BN_DEC_LEN*2)

int ecc_enc(unsigned char *p_x, unsigned char *p_y, unsigned char *key,
                unsigned char *in_buf, int in_len, unsigned char *out_buf, int out_len);
int ecc_dec(unsigned char *R_x, unsigned char *R_y, unsigned char *Private,
                        unsigned char *in_buf, int in_len, unsigned char *out_buf, int out_len);


BYTE ConverHexCharToByte(BYTE chHexChar);

int ConvertHexTextToBin(const std::string& strSource, char* pOut, const int nLen);

char* Ax3_stristr_ansi(const char *str1, const char *str2);
int strupos(const char *pSource, const char *sPat);
int striupos(const char * pSource, const char * sPat);

/**************************** ECC end *********************************/


class EccEncrypt {
public:
    EccEncrypt();
    
//    void InitECCKey(void);
//
//    void OnAESEncipherKey(CDataPacket* pDataPacket);
    
private:
    char                        m_szPublicKeyX[ECC_KEY_LEN];
    char                        m_szPublicKeyY[ECC_KEY_LEN];
    char                        m_szLittleKey[ECC_KEY_LEN];

    char                        m_szRKeyX[ECC_KEY_LEN];
    char                        m_szRKeyY[ECC_KEY_LEN];
    char                        m_szPrivateKey[ECC_KEY_LEN];
};

class EccCode {
  
public:
    int age;
};

extern unsigned int CalcAlign(unsigned int n, unsigned align);

#endif
