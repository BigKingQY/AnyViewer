//
//  EccCode.m
//  AnyViewer
//
//  Created by Aomei on 2021/8/31.
//

#import "EccCode.h"
#include <openssl/crypto.h>
#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/objects.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>


/*
*  p_x:        input, public_keyµƒx, 24 ToBytes, bin∑Ω Ω
*  p_y:        input, public_keyµƒy, 24 ToBytes, bin∑Ω Ω
*  key:        input, 24 ToBytes, bin∑Ω Ω
*  in_buf:      input,  √˜Œƒbuf,  »Áπ˚–Ë“™∑÷∂Œ∂‘√˜Œƒº”√‹£¨ ‘Ú√ø¥Œ¥´»Îµƒ◊÷Ω⁄ ˝±ÿ–Î «46µƒ±∂ ˝£¨
*                  ≥˝∑« «◊Ó∫Û“ª∂Œ≤ª◊„46 ToBytes.
*  in_len:      input,  ¥´»Î√˜Œƒµƒ◊÷Ω⁄ ˝
*  out_buf:      output, ”√”⁄¥Ê∑≈º”√‹∫Ûµƒ ˝æ›
*  out_len:      input,  out_buf µƒ¥Û–°(◊÷Ω⁄ ˝)£¨
*                    µ˜”√’ﬂ±ÿ–ÎÃ·π©◊„πª¥Ûµƒbuf,  buf ¥Û–°Œ™(in_len + 45)/46*48
*  return:        < 0  ¥ÌŒÛ
*                    > 0  √‹Œƒµƒ≥§∂»£¨’˝»∑µƒ≥§∂»”¶Œ™(in_len + 45)/46*48
*/
int ecc_enc(unsigned char *p_x, unsigned char *p_y, unsigned char *key,
                unsigned char *in_buf, int in_len, unsigned char *out_buf, int out_len)
{
    EC_KEY *b=nullptr;
    BIGNUM *x_a=nullptr, *y_a=nullptr, *k=nullptr;
    int ret = -1;
    const EC_GROUP *group;
    EC_POINT *Pm=nullptr, *B = nullptr;
    unsigned char mb1[ECC_BN_DEC_LEN+4], mb2[ECC_BN_DEC_LEN+4];
    int len, ret_len = 0;
    unsigned char *tbuf = nullptr;
    BN_CTX *ctx = nullptr;

    if((in_buf == nullptr) || (out_buf == nullptr)
        || (in_len == 0) || (out_len == 0))
        return -1;

    if ((ctx=BN_CTX_new()) == nullptr) goto err;
    
    b = EC_KEY_new_by_curve_name(NID_X9_62_prime192v1);
    if ( b == nullptr) goto err;
    
    if ((x_a=BN_new()) == nullptr) goto err;
    if ((y_a=BN_new()) == nullptr) goto err;
    if ((k=BN_new()) ==nullptr) goto err;

       group = EC_KEY_get0_group(b);
     if(group == nullptr) goto err;
    
    Pm= EC_POINT_new(group);
    B = EC_POINT_new(group);
    if((Pm == nullptr) || (B == nullptr)) goto err;
    
    /* PublicKey */
    if(BN_bin2bn(p_x, 24, x_a) == nullptr) goto err;
    if(BN_bin2bn(p_y, 24, y_a) == nullptr) goto err;
    if(!EC_KEY_set_public_key_affine_coordinates(b,x_a,y_a)) goto err;
    
    /* k */
    if(BN_bin2bn(key, 24, k) == nullptr) goto err;

    while(in_len > 0)
    {
        /* º”√‹≥§∂»: “ª∏ˆBIGNUM Œ™23 ToBytes£¨≤ª◊„µƒƒ©Œ≤≤π0 */
        memset(mb1, 0, sizeof(mb1));
        memset(mb2, 0, sizeof(mb2));
        len = (in_len > ECC_BN_ENC_LEN) ? ECC_BN_ENC_LEN : in_len;
        memcpy(mb1, in_buf, len);
        in_len -= len;
        in_buf += len;
        if(BN_bin2bn(mb1, ECC_BN_ENC_LEN, x_a) == nullptr) goto err;

        if(in_len > 0)
        {
            len = (in_len > ECC_BN_ENC_LEN) ? ECC_BN_ENC_LEN : in_len;
            memcpy(mb2, in_buf, len);

            in_len -= len;
            in_buf += len;
            if(BN_bin2bn(mb2, ECC_BN_ENC_LEN, y_a) == nullptr) goto err;
        }
        else
            if(BN_bin2bn(mb2, 0, y_a) == nullptr) goto err;
        
        if (!EC_POINT_set_affine_coordinates_GF2m(group, Pm, x_a, y_a, ctx)) goto err;
    
        /*  B = k * public_key(b)  */
        if (!EC_POINT_mul(group, B, nullptr, EC_KEY_get0_public_key(b), k, ctx)) goto err;

        /*  B = Pm + k * public_key(b) */
        /** Computes the sum of two EC_POINT
         *  @param  group  underlying EC_GROUP object
         *  @param  r      EC_POINT object for the result (r = a + b)
         *  @param  a      EC_POINT object with the first summand
         *  @param  b      EC_POINT object with the second summand
         *  @param  ctx    BN_CTX object (optional)
         *  @return 1 on success and 0 if an error occured
         *  \int EC_POINT_add(const EC_GROUP *group, EC_POINT *r, const EC_POINT *a, const EC_POINT *b, BN_CTX *ctx);
         */
        if(!EC_POINT_add(group, B, Pm, B, ctx)) goto err;
        if (!EC_POINT_get_affine_coordinates_GF2m(group, B, x_a, y_a, ctx)) goto err;
        
        memset(mb1, 0, sizeof(mb1));
        len = BN_num_bytes(x_a);
        if(len > 0)
        {
            tbuf = (unsigned char *)OPENSSL_malloc(len+4);
            if(tbuf == nullptr) goto err;
            
            len = BN_bn2bin(x_a, tbuf);
            if(len > ECC_BN_DEC_LEN) goto err;
            
            memcpy(&mb1[ECC_BN_DEC_LEN-len], tbuf, len);
            OPENSSL_free(tbuf);
            tbuf = nullptr;
        }

        if(ECC_BN_DEC_LEN > out_len)
        {
            ret = -2;
            goto err;
        }
        memcpy(out_buf, mb1, ECC_BN_DEC_LEN);
        out_buf += ECC_BN_DEC_LEN;
        out_len -= ECC_BN_DEC_LEN;
        ret_len += ECC_BN_DEC_LEN;

        memset(mb1, 0, sizeof(mb1));
        len = BN_num_bytes(y_a);
        if(len > 0)
        {
            tbuf = (unsigned char *)OPENSSL_malloc(len+4);
            if(tbuf == nullptr) goto err;
            
            len = BN_bn2bin(y_a, tbuf);
            if(len > ECC_BN_DEC_LEN) goto err;
            
            memcpy(&mb1[ECC_BN_DEC_LEN-len], tbuf, len);
            OPENSSL_free(tbuf);
            tbuf = nullptr;
        }

        if(ECC_BN_DEC_LEN > out_len)
        {
            ret = -2;
            goto err;
        }
        memcpy(out_buf, mb1, ECC_BN_DEC_LEN);
        out_buf += ECC_BN_DEC_LEN;
        out_len -= ECC_BN_DEC_LEN;
        ret_len += ECC_BN_DEC_LEN;
    }

    ret = ret_len;
err:
    if (x_a) BN_free(x_a);
    if (y_a) BN_free(y_a);
    if (k) BN_free(k);

    if (Pm) EC_POINT_free(Pm);
    if (B) EC_POINT_free(B);
    
    if (b) EC_KEY_free(b);
      if (tbuf) OPENSSL_free(tbuf);
    if (ctx) BN_CTX_free(ctx);

    return(ret);
}

/*
*  R_x:        input,  R key µƒx, 24 ToBytes, bin∑Ω Ω
*  R_y:        input,  R key µƒy, 24 ToBytes, bin∑Ω Ω
*  Private:    input,  Private key, 24 ToBytes, bin∑Ω Ω
*  in_buf:      input,  √‹Œƒbuf,  »Áπ˚–Ë“™∑÷∂Œ∂‘√‹ŒƒΩ‚√‹£¨ ‘Ú√ø¥Œ¥´»Îµƒ◊÷Ω⁄ ˝±ÿ–Î «48 µƒ±∂ ˝£¨
*                  ’˝»∑µƒ“ª∂Œ√‹Œƒµƒ¥Û–°”¶∏√ «48 µƒ±∂ ˝.
*  in_len:      input,  ¥´»Î√‹Œƒµƒ◊÷Ω⁄ ˝
*  out_buf:      output, ”√”⁄¥Ê∑≈Ω‚√‹∫Ûµƒ ˝æ›
*  out_len:      input,  out_buf µƒ¥Û–°(◊÷Ω⁄ ˝)£¨
*                    µ˜”√’ﬂ±ÿ–ÎÃ·π©◊„πª¥Ûµƒbuf,  buf ¥Û–°Œ™in_len/48*46
*  return:        < 0  ¥ÌŒÛ
*                    > 0  √˜Œƒµƒ≥§∂»£¨’˝»∑µƒ≥§∂»”¶Œ™in_len/48*46
*                ”…”⁄º”√‹ ±ª·≤π0 ∂‘∆Î£¨À˘“‘Ω‚√‹∫Ûµƒ√˜Œƒƒ©Œ≤ø…ƒ‹ª·∂‡≥ˆ“ª–©0
*/
int ecc_dec(unsigned char *R_x, unsigned char *R_y, unsigned char *Private,
                        unsigned char *in_buf, int in_len, unsigned char *out_buf, int out_len)
{
    EC_KEY *b=nullptr;
    BIGNUM *x_a=nullptr, *y_a=nullptr;
    int ret = -1;
    const EC_GROUP *group;
    EC_POINT *R = nullptr, *B = nullptr;
    unsigned char mb1[ECC_BN_DEC_LEN+4], mb2[ECC_BN_DEC_LEN+4];
    int len, ret_len = 0;
    unsigned char *tbuf = nullptr;
    BN_CTX *ctx=nullptr;
    
    if((in_buf == nullptr) || (out_buf == nullptr)
        || (in_len == 0) || (in_len%ECC_DEC_ALIGN) || (out_len == 0))
        return -1;
     
    b = EC_KEY_new_by_curve_name(NID_X9_62_prime192v1);
    if ( b == nullptr) goto err;
    
    if ((x_a=BN_new()) == nullptr) goto err;
    if ((y_a=BN_new()) == nullptr) goto err;

    group = EC_KEY_get0_group(b);
    if(group == nullptr) goto err;

    R = EC_POINT_new(group);
    B = EC_POINT_new(group);
    if((R == nullptr) || (B == nullptr))
        goto err;
    
    /* PrivateKey */
    if(BN_bin2bn(Private, 24, x_a) == nullptr) goto err;

    //…˙≥…√‹‘ø
     if(!EC_KEY_set_private_key(b,x_a)) goto err;

    while(in_len)
    {
        /* R */
        if(BN_bin2bn(R_x, 24, x_a) == nullptr) goto err;
        if(BN_bin2bn(R_y, 24, y_a) == nullptr) goto err;
        
        if (!EC_POINT_set_affine_coordinates_GF2m(group, R, x_a, y_a, ctx)) goto err;

        len = (in_len > ECC_DEC_ALIGN) ? ECC_DEC_ALIGN : in_len;
        memcpy(mb1, in_buf, len/2);
        memcpy(mb2, in_buf+len/2, len-len/2);

        in_len -= len;
        in_buf += len;
        
        if(BN_bin2bn(mb1, len/2, x_a) == nullptr) goto err;
        if(BN_bin2bn(mb2, len-len/2, y_a) == nullptr) goto err;
        if (!EC_POINT_set_affine_coordinates_GF2m(group, B, x_a, y_a, ctx)) goto err;

        if(!EC_POINT_mul(group, R, nullptr, R, EC_KEY_get0_private_key(b), ctx)) goto err; //R = k*G*private(b) = k*public(b)
   
        /** Computes the inverse of a EC_POINT
         *  @param  group  underlying EC_GROUP object
         *  @param  a      EC_POINT object to be inverted (it's used for the result as well)
         *  @param  ctx    BN_CTX object (optional)
         *  @return 1 on success and 0 if an error occured
         *  \int EC_POINT_invert(const EC_GROUP *group, EC_POINT *a, BN_CTX *ctx);
         */
        if(!EC_POINT_invert(group, R, ctx)) goto err;

        //B = Pm+k*public_key(b)-R = Pm+k*public_key(b)-k*public_key(b) = Pm
        if(!EC_POINT_add(group, B, B, R, ctx)) goto err;
  
            /*out put plaintext after decryption*/
        if (!EC_POINT_get_affine_coordinates_GF2m(group, B, x_a, y_a, ctx)) goto err;

        memset(mb1, 0, sizeof(mb1));
        len = BN_num_bytes(x_a);
        if(len > 0)
        {
            tbuf = (unsigned char *)OPENSSL_malloc(len+4);
            if(tbuf == nullptr) goto err;
            
            len = BN_bn2bin(x_a, tbuf);
            if(len > ECC_BN_ENC_LEN) goto err;
            
            memcpy(&mb1[ECC_BN_ENC_LEN-len], tbuf, len);
            OPENSSL_free(tbuf);
            tbuf = nullptr;
        }

        if(ECC_BN_ENC_LEN > out_len)
        {
            ret = -2;
            goto err;
        }
        memcpy(out_buf, mb1, ECC_BN_ENC_LEN);
        out_buf += ECC_BN_ENC_LEN;
        out_len -= ECC_BN_ENC_LEN;
        ret_len += ECC_BN_ENC_LEN;

        memset(mb1, 0, sizeof(mb1));
        len = BN_num_bytes(y_a);
        if(len > 0)
        {
            tbuf = (unsigned char *)OPENSSL_malloc(len+4);
            if(tbuf == nullptr) goto err;
            
            len = BN_bn2bin(y_a, tbuf);
            if(len > ECC_BN_ENC_LEN) goto err;
            
            memcpy(&mb1[ECC_BN_ENC_LEN-len], tbuf, len);
            OPENSSL_free(tbuf);
            tbuf = nullptr;
        }

        if(ECC_BN_ENC_LEN > out_len)
        {
            ret = -2;
            goto err;
        }
        memcpy(out_buf, mb1, ECC_BN_ENC_LEN);
        out_buf += ECC_BN_ENC_LEN;
        out_len -= ECC_BN_ENC_LEN;
        ret_len += ECC_BN_ENC_LEN;
    }

    ret = ret_len;
err:
    if (x_a) BN_free(x_a);
    if (y_a) BN_free(y_a);
    if (R) EC_POINT_free(R);
    if (B) EC_POINT_free(B);
    
    if (b) EC_KEY_free(b);
    if (tbuf) OPENSSL_free(tbuf);

    return(ret);
}


/************************* AES ENCRYPT ****************************/

#ifndef BYTE
typedef unsigned char       BYTE;
#endif

#ifndef ULONGLONG
typedef uint64_t ULONGLONG;
#endif

#define __le32     unsigned int

typedef BYTE     u8;
typedef unsigned int     u32;
typedef ULONGLONG     u64;

typedef BYTE     __u8;
typedef unsigned int     __u32;
typedef ULONGLONG     __u64;

#define swab32(x) \
({ \
    __u32 __x = (x); \
    ((__u32)( \
        (((__u32)(__x) & (__u32)0x000000ffUL) << 24) | \
        (((__u32)(__x) & (__u32)0x0000ff00UL) <<  8) | \
        (((__u32)(__x) & (__u32)0x00ff0000UL) >>  8) | \
        (((__u32)(__x) & (__u32)0xff000000UL) >> 24) )); \
})

//#if JHL_SYS_TYPE == SYS_TYPE_CM || JHL_SYS_TYPE == SYS_TYPE_RA
//#define cpu_to_le32(x) swab32(x)
//#define le32_to_cpu(x) swab32(x)
//#else
#define cpu_to_le32(x) (__u32)(x)
#define le32_to_cpu(x) (__u32)(x)
//#endif

static inline u8 ToByte(const u32 x, const unsigned n)
{
    return (BYTE)(x >> (n << 3));
}

#define E_KEY ctx->E
#define D_KEY ctx->D

static u8 pow_tab[256];
static u8 log_tab[256];
static u8 sbx_tab[256];
static u8 isb_tab[256];
static u32 rco_tab[10];
static u32 ft_tab[4][256];
static u32 it_tab[4][256];

static u32 fl_tab[4][256];
static u32 il_tab[4][256];

static inline u8 f_mult (u8 a, u8 b)
{
    u8 aa = log_tab[a], cc = aa + log_tab[b];

    return pow_tab[cc + (cc < aa ? 1 : 0)];
}

static inline u32 rol32(u32 word, unsigned int shift)
{
    return (word << shift) | (word >> (32 - shift));
}

static inline u32 ror32(u32 word, unsigned int shift)
{
    return (word >> shift) | (word << (32 - shift));
}

#define ff_mult(a,b)    (a && b ? f_mult(a, b) : 0)

#define f_rn(bo, bi, n, k)                    \
    bo[n] =  ft_tab[0][ToByte(bi[n],0)] ^                \
             ft_tab[1][ToByte(bi[(n + 1) & 3],1)] ^        \
             ft_tab[2][ToByte(bi[(n + 2) & 3],2)] ^        \
             ft_tab[3][ToByte(bi[(n + 3) & 3],3)] ^ *(k + n)

#define i_rn(bo, bi, n, k)                    \
    bo[n] =  it_tab[0][ToByte(bi[n],0)] ^                \
             it_tab[1][ToByte(bi[(n + 3) & 3],1)] ^        \
             it_tab[2][ToByte(bi[(n + 2) & 3],2)] ^        \
             it_tab[3][ToByte(bi[(n + 1) & 3],3)] ^ *(k + n)

#define ls_box(x)                \
    ( fl_tab[0][ToByte(x, 0)] ^            \
      fl_tab[1][ToByte(x, 1)] ^            \
      fl_tab[2][ToByte(x, 2)] ^            \
      fl_tab[3][ToByte(x, 3)] )

#define f_rl(bo, bi, n, k)                    \
    bo[n] =  fl_tab[0][ToByte(bi[n],0)] ^                \
             fl_tab[1][ToByte(bi[(n + 1) & 3],1)] ^        \
             fl_tab[2][ToByte(bi[(n + 2) & 3],2)] ^        \
             fl_tab[3][ToByte(bi[(n + 3) & 3],3)] ^ *(k + n)

#define i_rl(bo, bi, n, k)                    \
    bo[n] =  il_tab[0][ToByte(bi[n],0)] ^                \
             il_tab[1][ToByte(bi[(n + 3) & 3],1)] ^        \
             il_tab[2][ToByte(bi[(n + 2) & 3],2)] ^        \
             il_tab[3][ToByte(bi[(n + 1) & 3],3)] ^ *(k + n)

static void gen_tabs (void)
{
    
    u32 i, t;
    u8 p, q;

    /* log and power tables for GF(2**8) finite field with
       0x011b as modular polynomial - the simplest primitive
       root is 0x03, used here to generate the tables */

    for (i = 0, p = 1; i < 256; ++i) {
        pow_tab[i] = (u8) p;
        log_tab[p] = (u8) i;

        p ^= (p << 1) ^ (p & 0x80 ? 0x01b : 0);
    }

    log_tab[1] = 0;

    for (i = 0, p = 1; i < 10; ++i) {
        rco_tab[i] = p;

        p = (p << 1) ^ (p & 0x80 ? 0x01b : 0);
    }

    for (i = 0; i < 256; ++i) {
        p = (i ? pow_tab[255 - log_tab[i]] : 0);
        q = ((p >> 7) | (p << 1)) ^ ((p >> 6) | (p << 2));
        p ^= 0x63 ^ q ^ ((q >> 6) | (q << 2));
        sbx_tab[i] = p;
        isb_tab[p] = (u8) i;
    }

    for (i = 0; i < 256; ++i) {
        p = sbx_tab[i];

        t = p;
        fl_tab[0][i] = t;
        fl_tab[1][i] = rol32(t, 8);
        fl_tab[2][i] = rol32(t, 16);
        fl_tab[3][i] = rol32(t, 24);

        t = ((u32) ff_mult (2, p)) |
            ((u32) p << 8) |
            ((u32) p << 16) | ((u32) ff_mult (3, p) << 24);

        ft_tab[0][i] = t;
        ft_tab[1][i] = rol32(t, 8);
        ft_tab[2][i] = rol32(t, 16);
        ft_tab[3][i] = rol32(t, 24);

        p = isb_tab[i];

        t = p;
        il_tab[0][i] = t;
        il_tab[1][i] = rol32(t, 8);
        il_tab[2][i] = rol32(t, 16);
        il_tab[3][i] = rol32(t, 24);

        t = ((u32) ff_mult (14, p)) |
            ((u32) ff_mult (9, p) << 8) |
            ((u32) ff_mult (13, p) << 16) |
            ((u32) ff_mult (11, p) << 24);

        it_tab[0][i] = t;
        it_tab[1][i] = rol32(t, 8);
        it_tab[2][i] = rol32(t, 16);
        it_tab[3][i] = rol32(t, 24);
    }
}

#define star_x(x) (((x) & 0x7f7f7f7f) << 1) ^ ((((x) & 0x80808080) >> 7) * 0x1b)

#define imix_col(y,x)       \
    u   = star_x(x);        \
    v   = star_x(u);        \
    w   = star_x(v);        \
    t   = w ^ (x);          \
   (y)  = u ^ v ^ w;        \
   (y) ^= ror32(u ^ t,  8) ^ \
          ror32(v ^ t, 16) ^ \
          ror32(t,24)

/* initialise the key schedule from the user supplied key */

#define loop4(i)                                    \
{   t = ror32(t,  8); t = ls_box(t) ^ rco_tab[i];    \
    t ^= E_KEY[4 * i];     E_KEY[4 * i + 4] = t;    \
    t ^= E_KEY[4 * i + 1]; E_KEY[4 * i + 5] = t;    \
    t ^= E_KEY[4 * i + 2]; E_KEY[4 * i + 6] = t;    \
    t ^= E_KEY[4 * i + 3]; E_KEY[4 * i + 7] = t;    \
}

#define loop6(i)                                    \
{   t = ror32(t,  8); t = ls_box(t) ^ rco_tab[i];    \
    t ^= E_KEY[6 * i];     E_KEY[6 * i + 6] = t;    \
    t ^= E_KEY[6 * i + 1]; E_KEY[6 * i + 7] = t;    \
    t ^= E_KEY[6 * i + 2]; E_KEY[6 * i + 8] = t;    \
    t ^= E_KEY[6 * i + 3]; E_KEY[6 * i + 9] = t;    \
    t ^= E_KEY[6 * i + 4]; E_KEY[6 * i + 10] = t;   \
    t ^= E_KEY[6 * i + 5]; E_KEY[6 * i + 11] = t;   \
}

#define loop8(i)                                    \
{   t = ror32(t,  8); ; t = ls_box(t) ^ rco_tab[i];  \
    t ^= E_KEY[8 * i];     E_KEY[8 * i + 8] = t;    \
    t ^= E_KEY[8 * i + 1]; E_KEY[8 * i + 9] = t;    \
    t ^= E_KEY[8 * i + 2]; E_KEY[8 * i + 10] = t;   \
    t ^= E_KEY[8 * i + 3]; E_KEY[8 * i + 11] = t;   \
    t  = E_KEY[8 * i + 4] ^ ls_box(t); E_KEY[8 * i + 12] = t;                \
    t ^= E_KEY[8 * i + 5]; E_KEY[8 * i + 13] = t;   \
    t ^= E_KEY[8 * i + 6]; E_KEY[8 * i + 14] = t;   \
    t ^= E_KEY[8 * i + 7]; E_KEY[8 * i + 15] = t;   \
}

int tabs_inited = 0;
int aes_set_key(struct aes_ctx *ctx_arg, const unsigned char *in_key, unsigned int key_len)
{
    struct aes_ctx *ctx = ctx_arg;
    const __le32 *key = (const __le32 *)in_key;
    u32 i, t, u, v, w;

    if (key_len != 16 && key_len != 24 && key_len != 32) {
        return -1;
    }

    if(!tabs_inited)
    {
        gen_tabs();
        tabs_inited = 1;
    }
    
    ctx->key_length = key_len;

    E_KEY[0] = le32_to_cpu(key[0]);
    E_KEY[1] = le32_to_cpu(key[1]);
    E_KEY[2] = le32_to_cpu(key[2]);
    E_KEY[3] = le32_to_cpu(key[3]);

    switch (key_len) {
    case 16:
        t = E_KEY[3];
        for (i = 0; i < 10; ++i)
            loop4 (i);
        break;

    case 24:
        E_KEY[4] = le32_to_cpu(key[4]);
        t = E_KEY[5] = le32_to_cpu(key[5]);
        for (i = 0; i < 8; ++i)
            loop6 (i);
        break;

    case 32:
        E_KEY[4] = le32_to_cpu(key[4]);
        E_KEY[5] = le32_to_cpu(key[5]);
        E_KEY[6] = le32_to_cpu(key[6]);
        t = E_KEY[7] = le32_to_cpu(key[7]);
        for (i = 0; i < 7; ++i)
            loop8 (i);
        break;
    }

    D_KEY[0] = E_KEY[0];
    D_KEY[1] = E_KEY[1];
    D_KEY[2] = E_KEY[2];
    D_KEY[3] = E_KEY[3];

    for (i = 4; i < key_len + 24; ++i) {
        imix_col (D_KEY[i], E_KEY[i]);
    }

    return 0;
}

/* encrypt a block of text */

#define f_nround(bo, bi, k) \
    f_rn(bo, bi, 0, k);     \
    f_rn(bo, bi, 1, k);     \
    f_rn(bo, bi, 2, k);     \
    f_rn(bo, bi, 3, k);     \
    k += 4

#define f_lround(bo, bi, k) \
    f_rl(bo, bi, 0, k);     \
    f_rl(bo, bi, 1, k);     \
    f_rl(bo, bi, 2, k);     \
    f_rl(bo, bi, 3, k)

void aes_encrypt(const struct aes_ctx *ctx_arg, unsigned char *out, const unsigned char *in)
{
    const struct aes_ctx *ctx = ctx_arg;
    const __le32 *src = (const __le32 *)in;
    __le32 *dst = (__le32 *)out;
    u32 b0[4], b1[4];
    const u32 *kp = (const u32 *)E_KEY + 4;

    b0[0] = le32_to_cpu(src[0]) ^ E_KEY[0];
    b0[1] = le32_to_cpu(src[1]) ^ E_KEY[1];
    b0[2] = le32_to_cpu(src[2]) ^ E_KEY[2];
    b0[3] = le32_to_cpu(src[3]) ^ E_KEY[3];

    if (ctx->key_length > 24) {
        f_nround (b1, b0, kp);
        f_nround (b0, b1, kp);
    }

    if (ctx->key_length > 16) {
        f_nround (b1, b0, kp);
        f_nround (b0, b1, kp);
    }

    f_nround (b1, b0, kp);
    f_nround (b0, b1, kp);
    f_nround (b1, b0, kp);
    f_nround (b0, b1, kp);
    f_nround (b1, b0, kp);
    f_nround (b0, b1, kp);
    f_nround (b1, b0, kp);
    f_nround (b0, b1, kp);
    f_nround (b1, b0, kp);
    f_lround (b0, b1, kp);

    dst[0] = cpu_to_le32(b0[0]);
    dst[1] = cpu_to_le32(b0[1]);
    dst[2] = cpu_to_le32(b0[2]);
    dst[3] = cpu_to_le32(b0[3]);
}

/* decrypt a block of text */

#define i_nround(bo, bi, k) \
    i_rn(bo, bi, 0, k);     \
    i_rn(bo, bi, 1, k);     \
    i_rn(bo, bi, 2, k);     \
    i_rn(bo, bi, 3, k);     \
    k -= 4

#define i_lround(bo, bi, k) \
    i_rl(bo, bi, 0, k);     \
    i_rl(bo, bi, 1, k);     \
    i_rl(bo, bi, 2, k);     \
    i_rl(bo, bi, 3, k)

void aes_decrypt(const struct aes_ctx *ctx_arg, unsigned char *out, const unsigned char *in)
{
    const struct aes_ctx *ctx = ctx_arg;
    const __le32 *src = (const __le32 *)in;
    __le32 *dst = (__le32 *)out;
    u32 b0[4], b1[4];
    const int key_len = ctx->key_length;
    const u32 *kp = (const u32 *)D_KEY + key_len + 20;

    b0[0] = le32_to_cpu(src[0]) ^ E_KEY[key_len + 24];
    b0[1] = le32_to_cpu(src[1]) ^ E_KEY[key_len + 25];
    b0[2] = le32_to_cpu(src[2]) ^ E_KEY[key_len + 26];
    b0[3] = le32_to_cpu(src[3]) ^ E_KEY[key_len + 27];

    if (key_len > 24) {
        i_nround (b1, b0, kp);
        i_nround (b0, b1, kp);
    }

    if (key_len > 16) {
        i_nround (b1, b0, kp);
        i_nround (b0, b1, kp);
    }

    i_nround (b1, b0, kp);
    i_nround (b0, b1, kp);
    i_nround (b1, b0, kp);
    i_nround (b0, b1, kp);
    i_nround (b1, b0, kp);
    i_nround (b0, b1, kp);
    i_nround (b1, b0, kp);
    i_nround (b0, b1, kp);
    i_nround (b1, b0, kp);
    i_lround (b0, b1, kp);

    dst[0] = cpu_to_le32(b0[0]);
    dst[1] = cpu_to_le32(b0[1]);
    dst[2] = cpu_to_le32(b0[2]);
    dst[3] = cpu_to_le32(b0[3]);
}

/********************
* type:   0: encrypt; 1: decrypt
* out buf len >= (in_len + 15) / 16 * 16
*********************/
int aes_aes(const struct aes_ctx *ctx, const unsigned char *in, int in_len, unsigned char *out, int type)
{
    int len;
    unsigned char buf[AES_BLOCK_SIZE];
    int out_len =0;
    
    while(in_len > 0)
    {
        memset(buf, 0, sizeof(buf));
        len = (in_len > AES_BLOCK_SIZE) ? AES_BLOCK_SIZE : in_len;
        memcpy(buf, in, len);

        if(type)
            aes_decrypt(ctx, out, buf);
        else
            aes_encrypt(ctx, out, buf);
        
        in_len -= len;
        out += AES_BLOCK_SIZE;
        in += len;
        out_len += AES_BLOCK_SIZE;
    }

    return out_len;
}





BYTE ConverHexCharToByte(BYTE chHexChar)
{
    const static BYTE nValueTable[123] =
    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    ,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,0,0
    ,0,0,0,0,0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0
    ,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,11,12
    ,13,14,15 };

    if (chHexChar >= 48 && chHexChar <= 123)
    {
        return nValueTable[chHexChar];
    }

    return 0;
}

int ConvertHexTextToBin(const std::string& strSource, char* pOut, const int nLen)
{
    const int nStrLen = custom_min(int(strSource.size()), nLen * 2);  //确保不能越界

    for (int i = 0; i < nStrLen;)
    {
        *pOut++ = ConverHexCharToByte((BYTE)strSource.at(i)) * 16
            + ConverHexCharToByte((BYTE)strSource.at(i + 1));

        i += 2;
    }

    return (nStrLen / 2);
}

char* Ax3_stristr_ansi(const char *str1, const char *str2)
{
    char *cp = (char *)str1;
    char *s1, *s2;

    if (!*str2)
        return((char *)str1);

    while (*cp)
    {
        s1 = cp;
        s2 = (char *)str2;

        while (*s1 && *s2)
        {
            if (*s1 == *s2 || abs(*s1 - *s2) == 32)
            {
                s1++, s2++;
            }
            else
            {
                break;
            }
        }

        if (!*s2)
            return(cp);

        cp++;
    }

    return(nullptr);
}


//--------------------------------------------------------------------------------
/// <summary>
/// 查找字符串串，成功返回开始位置
/// </summary>
/// <param name="pSource">源字符串</param>
/// <param name="sPat">被查找的字符串</param>
/// <returns>成功返回开始位置，否则-1</returns>
/// <created>Andy,2020/8/2</created>
/// <changed>Andy,2020/8/2</changed>
//--------------------------------------------------------------------------------
int strupos(const char *pSource, const char *sPat)
{
    int nResult = -1;
    const char* pStart = strstr(pSource, sPat);

    if (nullptr != pStart)
    {
        nResult = (int)(pStart - pSource);
    }

    return nResult;
}

int striupos(const char * pSource, const char * sPat)
{
    int nResult = -1;
    const char* pStart = Ax3_stristr_ansi(pSource, sPat);

    if (nullptr != pStart)
    {
        nResult = (int)(pStart - pSource);
    }

    return nResult;
}




EccEncrypt::EccEncrypt() {
    
}

//void EccEncrypt::InitECCKey() {
//    std::string strKey = ("FDEAB7BEB6119B28AF2C8069690DFD9E31826941CE38AB49");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPublicKeyX, ECC_KEY_LEN);
//
//    strKey = ("310B70A07716300EFC2D6761B3975E2AEC4E4C6097A7D12C");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPublicKeyY, ECC_KEY_LEN);
//
//    strKey = ("8E0A669C5A185F21119BC3D5CFEB11A293EAB39325AC9D48");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szLittleKey, ECC_KEY_LEN);
//
//    strKey = ("EBDCA421B0A437B7A059D3EFC94B353B771F84DC09B6FFD6");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szRKeyX, ECC_KEY_LEN);
//
//    strKey = ("A2564A78C9C6E3CF7983FDF89F0F66BC9927B2CAC98973E4");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szRKeyY, ECC_KEY_LEN);
//
//    strKey = ("D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF88920");
////    assert(strKey.size() == ECC_KEY_LEN * 2);
//    ConvertHexTextToBin(strKey, m_szPrivateKey, ECC_KEY_LEN);
//
//#ifdef _DEBUG
//    UCHAR szInputPlaintext[47] = "D77A7BEAE1724E0CBAB6B3412A3A4DCFC9C3BC256EF889";
//    UCHAR szOutputCiphertext[49] = { 0 };
//    UCHAR szOutputPlaintext[49] = { 0 };
//    ecc_enc((unsigned char *)m_szPublicKeyX, (unsigned char *)m_szPublicKeyY, (unsigned char *)m_szLittleKey,
//        szInputPlaintext, 46, szOutputCiphertext, 49);
//
//    ecc_dec((unsigned char *)m_szRKeyX, (unsigned char *)m_szRKeyY, (unsigned char *)m_szPrivateKey,
//        szOutputCiphertext, 48, szOutputPlaintext, 49);
//
//#endif
//}

unsigned int CalcAlign(unsigned int n, unsigned align)
{
    return ((n + align - 1) & (~(align - 1)));
}
