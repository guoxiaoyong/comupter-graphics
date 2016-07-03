#ifndef __GUOJPEG_H__
#define __GUOJPEG_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "guobuffer.h"


    typedef enum JFIF_SSM {
        YONLY = 0, H1V1 = 1, H2V1 = 2, H2V2 = 3
                                          }
                                          JFIF_SSM;

                                          typedef enum JFIF_CLR { GRAY = 0, COLOR = 1} JFIF_CLR;


    typedef struct JFIF_HTnode
    {
        struct JFIF_HTnode* l;
        struct JFIF_HTnode* r;
        unsigned char symbol; // leafnode has this value
        int isleaf;
    } JFIF_HTnode;


    typedef struct JFIF_EncEntry
    {
         unsigned char bits[16];
         unsigned char numbits;
    } JFIF_EncEntry;


    typedef struct JFIF_EncTable
    {
         JFIF_EncEntry entries[256];
    } JFIF_EncTable;


    typedef struct JFIF_RGBcolor
    {
        float r;
        float g;
        float b;
    } JFIF_RGBcolor;


    typedef struct JFIF_YCCcolor
    {
        float y;
        float cb;
        float cr;
    } JFIF_YCCcolor;


    typedef struct JFIF_RGBImage
    {
        int h;
        int w;
        float* r;
        float* g;
        float* b;
        JFIF_CLR mode;
    } JFIF_RGBImage;


    typedef struct JFIF_YCCImage
    {
        int h;
        int w;
        float* y;
        float* cr;
        float* cb;
        JFIF_CLR mode;
    } JFIF_YCCImage;


    typedef struct JFIF_YCCSubSampling
    {
        int yh;
        int yw;
        int ch;
        int cw;
        JFIF_SSM ssm;
        JFIF_CLR mode;
        float* y;
        float* cr;
        float* cb;
    } JFIF_YCCSubSampling;

    typedef struct JFIF_RLE
    {
        int nz;
        int num;
    } JFIF_RLE;


    typedef struct JFIF_RSA
    {
        unsigned char rs;
        unsigned int  add;
    } JFIF_RSA;


    typedef struct JFIF_b8x8
    {
        float data[64];
        int   zz[64];
        JFIF_RLE rle[64];
        JFIF_RSA rsa[64];
        int rlelen;
        int DiffDC;
    } JFIF_b8x8;


    typedef struct JFIF_SegPtrs
    {
            unsigned char* soi;  
            unsigned char* eoi;  
            unsigned char* app0;
            unsigned char* dqt;
            unsigned char* dht;
            unsigned char* sof0;
            unsigned char* sos;
            unsigned char* com;
            unsigned char* dri;

            unsigned char* rst0;
            unsigned char* rst1;
            unsigned char* rst2;
            unsigned char* rst3;
            unsigned char* rst4;
            unsigned char* rst5;
            unsigned char* rst6;
            unsigned char* rst7;
    } JFIF_SegPtrs;


    typedef struct JFIF_YSSBlockImage
    {
        int oyh;
        int oyw;

        int och;
        int ocw;

        int yh;
        int yw;
        int ch;
        int cw;

        JFIF_SSM ssm;
        JFIF_CLR mode;

        JFIF_b8x8* y;
        JFIF_b8x8* cr;
        JFIF_b8x8* cb;

        float lqt[64];
        float cqt[64];

        guobuffer* buffer;
        JFIF_SegPtrs segptrs;

    } JFIF_YSSBlockImage;


    JFIF_RGBImage* JFIF_RGBImage_Create(int h, int w, JFIF_CLR mode);
    JFIF_RGBImage* JFIF_RGBImage_CreateFromYCCImage(JFIF_YCCImage* yi);
    void JFIF_RGBImage_Destroy  (JFIF_RGBImage* ei);
    void JFIF_RGBImage_Set      (JFIF_RGBImage* ei, int i, int j, JFIF_RGBcolor* rgbcolor);
    void JFIF_RGBImage_Get      (JFIF_RGBImage* ei, int i, int j, JFIF_RGBcolor* rgbcolor);


    JFIF_YCCImage* JFIF_YCCImage_Create(int h, int w, JFIF_CLR mode);
    JFIF_YCCImage* JFIF_YCCImage_CreateFromRGBImage(JFIF_RGBImage* ei);
    JFIF_YCCImage* JFIF_YCCImage_CreateFromYSS(JFIF_YCCSubSampling* yss);
    void JFIF_YCCImage_Destroy  (JFIF_YCCImage* yi);
    void JFIF_YCCImage_Set      (JFIF_YCCImage* yi, int i, int j, JFIF_YCCcolor* ycccolor);
    void JFIF_YCCImage_Get      (JFIF_YCCImage* yi, int i, int j, JFIF_YCCcolor* ycccolor);
    void JFIF_YCCImage_Set_Cr   (JFIF_YCCImage* yi, int i, int j, float value);
    void JFIF_YCCImage_Set_Cb   (JFIF_YCCImage* yi, int i, int j, float value);


    void JFIF_RGB2YCC(JFIF_RGBImage* ei, JFIF_YCCImage* yi);
    void JFIF_YCC2RGB(JFIF_YCCImage* yi, JFIF_RGBImage* ei);


    JFIF_YCCSubSampling* JFIF_YCCSubSampling_Create(int h, int w, JFIF_CLR mode, JFIF_SSM ssm);
    JFIF_YCCSubSampling* JFIF_YCCSubSampling_CreateFromYCCImage(JFIF_YCCImage* yi, JFIF_SSM ssm);
    JFIF_YCCSubSampling* JFIF_YCCSubSampling_CreateFromYBI(JFIF_YSSBlockImage* ybi);
    void  JFIF_YCCSubSampling_Destroy   (JFIF_YCCSubSampling* yss);
    void  JFIF_YCCSubSampling_Set_Cr    (JFIF_YCCSubSampling* yss, int i, int j, float value);
    void  JFIF_YCCSubSampling_Set_Cb    (JFIF_YCCSubSampling* yss, int i, int j, float value);
    float JFIF_YCCSubSampling_Get_Cr    (JFIF_YCCSubSampling* yss, int i, int j);
    float JFIF_YCCSubSampling_Get_Cb    (JFIF_YCCSubSampling* yss, int i, int j);
    float JFIF_YCCSubSampling_Get_Y     (JFIF_YCCSubSampling* yss, int i, int j);


    void JFIF_YCC2YSS(JFIF_YCCImage* yi, JFIF_YCCSubSampling* yss);
    void JFIF_YSS2YCC(JFIF_YCCSubSampling* yss, JFIF_YCCImage* yi);
    void JFIF_YSS2YBI(JFIF_YCCSubSampling* yss, JFIF_YSSBlockImage* ybi);
    void JFIF_YBI2YSS(JFIF_YSSBlockImage* ybi, JFIF_YCCSubSampling* yss);

    JFIF_YSSBlockImage* JFIF_YSSBlockImage_Create(int oyh, int oyw, int och, int ocw, JFIF_CLR mode, JFIF_SSM ssm);
    JFIF_YSSBlockImage* JFIF_YSSBlockImage_CreateFromYSS(JFIF_YCCSubSampling* yss);
    void JFIF_YSSBlockImage_Destroy    (JFIF_YSSBlockImage* ybi);
    int  JFIF_YSSBlockImage_SetElement (JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj, JFIF_YCCcolor* ycccolor);
    int  JFIF_YSSBlockImage_GetElement (JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj, JFIF_YCCcolor* ycccolor);
    void JFIF_YSSBlockImage_FDCT(JFIF_YSSBlockImage* ybi);
    void JFIF_YSSBlockImage_IDCT(JFIF_YSSBlockImage* ybi);
    void JFIF_YSSBlockImage_FQuantize(JFIF_YSSBlockImage* ybi);
    void JFIF_YSSBlockImage_IQuantize(JFIF_YSSBlockImage* ybi);
    void JFIF_YSSBlockImage_SetQT(JFIF_YSSBlockImage* ybi, const float lqt[64], const float cqt[64]);
    void JFIF_YSSBlockImage_FACRLE(JFIF_YSSBlockImage* ybi);
    void JFIF_YSSBlockImage_IACRLE(JFIF_YSSBlockImage* ybi);
    void JFIF_YSSBlockImage_DiffDC(JFIF_YSSBlockImage* ybi);

    float* JFIF_YSSBlockImage_GetBlock_Y(JFIF_YSSBlockImage* ybi, int i, int j);
    float* JFIF_YSSBlockImage_GetBlock_Cr(JFIF_YSSBlockImage* ybi, int i, int j);
    float* JFIF_YSSBlockImage_GetBlock_Cb(JFIF_YSSBlockImage* ybi, int i, int j);
    int* JFIF_YSSBlockImage_GetBlock_Yzz(JFIF_YSSBlockImage* ybi, int i, int j);
    int* JFIF_YSSBlockImage_GetBlock_Crzz(JFIF_YSSBlockImage* ybi, int i, int j);
    int* JFIF_YSSBlockImage_GetBlock_Cbzz(JFIF_YSSBlockImage* ybi, int i, int j);

    JFIF_RLE* JFIF_YSSBlockImage_GetBlock_Yrle(JFIF_YSSBlockImage* ybi, int i, int j);
    JFIF_RLE* JFIF_YSSBlockImage_GetBlock_Crrle(JFIF_YSSBlockImage* ybi, int i, int j);
    JFIF_RLE* JFIF_YSSBlockImage_GetBlock_Cbrle(JFIF_YSSBlockImage* ybi, int i, int j);

    void JFIF_YSSBlockImage_WriteFile(JFIF_YSSBlockImage* ybi, const char* filename);

    void JFIF_YSSBlockImage_ReadEOI(JFIF_YSSBlockImage* ybi);


    int  JPEG_FRLE(int zz[64], JFIF_RLE* rle);
    int  JPEG_IRLE(JFIF_RLE* rle, int zz[64]);
    int  JPEG_PRINTRLE(JFIF_RLE* rle);
    unsigned int* CatAddCode(int num);




   
    JFIF_HTnode* JFIF_HTnode_Create();
    JFIF_HTnode* JFIF_HTnode_CreateTree(unsigned char* bits, unsigned char* num);
    void JFIF_HTnode_DestroyTree(JFIF_HTnode* htn);
    void JFIF_HTnode_Set(JFIF_HTnode* node,
                         unsigned char  currentLevel,
                         unsigned char  targetLevel,
                         unsigned char* set,
                         unsigned char  num,
                         unsigned char* hValues,
                         int offset);


    void JFIF_HTnode_Transverse(JFIF_HTnode* node, JFIF_EncTable* enctbl, int level, unsigned char bits[16]);

    JFIF_EncTable* JFIF_EncTable_Create(unsigned char* numbits, unsigned char* symbols);
    void JFIF_EncTable_Destroy(JFIF_EncTable* enctbl);
    void JFIF_EncTable_Print(JFIF_EncTable* enctbl);




#ifdef __cplusplus
}
#endif


#endif


