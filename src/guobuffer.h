#ifndef __GUOBUFFER_H__
#define __GUOBUFFER_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


    typedef struct guobuffer
    {
        unsigned char* mem;
        unsigned char* ptr;
        size_t size;
        size_t used;
        unsigned char bitpos;
        int  bitmode;
        char bitmode_str[16];
    } guobuffer;

    guobuffer* guobuffer_Create(const size_t n);
    guobuffer* guobuffer_CreateFromFile(const char* filename);
    void guobuffer_Destroy(guobuffer* buf);

    int  guobuffer_ReadFile     (guobuffer* buf, const char* filename);

    void guobuffer_WriteUint8   (guobuffer* buf, const unsigned char num);
    void guobuffer_WriteSint8   (guobuffer* buf, const signed char num);
    void guobuffer_WriteUint16  (guobuffer* buf, const unsigned short num);
    void guobuffer_WriteSint16  (guobuffer* buf, const signed short num);
    void guobuffer_WriteUint32  (guobuffer* buf, const unsigned int num);
    void guobuffer_WriteSint32  (guobuffer* buf, const signed int num);
    void guobuffer_WriteChars   (guobuffer* buf, const char* num, const size_t n);
    int  guobuffer_WriteFile    (guobuffer* buf, const char* filename);
    void guobuffer_SetBits      (guobuffer* buf, const unsigned char bitnum);
    void guobuffer_SetBitStream (guobuffer* buf, const unsigned char* bits, const unsigned char n);

    void guobuffer_ReadChars(guobuffer* buf, char* num, const size_t n);

    
    size_t guo_GetSizeFromFileName  (const char* FileName);
    size_t guo_GetSizeFromFilePoint (FILE* fp);
    int    guo_BigSmall();
    void   guo_ReverseBytes(char* mem, int n);
    unsigned short guo_BigUint16(unsigned short num);
    unsigned char  guo_GetBit(unsigned char num, unsigned char n);
    unsigned char  guo_SetBit(unsigned char* num, unsigned char n, unsigned char bitnum);

#ifdef __cplusplus
}
#endif


#endif


