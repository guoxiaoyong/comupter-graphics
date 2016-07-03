#include "tcimg.h"
#include "guojpeg.h"


int main(void)
{

        unsigned char numbits[16] = {0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
        unsigned char symbols[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

        JFIF_EncTable* enctbl = JFIF_EncTable_Create(numbits, symbols);
        JFIF_EncTable_Print(enctbl);
        JFIF_EncTable_Destroy(enctbl);


#if 0
     unsigned char a = 0xff;

     printf("a = %hhu\n", a);

     guo_SetBit(&a, 0, 0);
     guo_SetBit(&a, 1, 0);
     guo_SetBit(&a, 2, 0);
     guo_SetBit(&a, 3, 0);
     guo_SetBit(&a, 6, 0);
     guo_SetBit(&a, 7, 1);
     printf("a = %hhu\n", a);


     guo_SetBit(&a, 2, 1);
     printf("a = %hhu\n", guo_GetBit(a, 0));
     printf("a = %hhu\n", guo_GetBit(a, 1));
     printf("a = %hhu\n", guo_GetBit(a, 2));
     printf("a = %hhu\n", guo_GetBit(a, 3));
#endif
     


#if 0
    TrueColorImage bmp;
    TCIMG_ReadFromFile(&bmp, "test.bmp");
    UINT8 R, G, B;
    JFIF_RGBcolor rgbcolor;
    int i, j;

    JFIF_RGBImage* ei = JFIF_RGBImage_Create(bmp.ImgHeight, bmp.ImgWidth, COLOR);

    for (i = 0; i < ei->h; ++i)
    {
        for (j = 0; j < ei->w; ++j)
        {
            TCIMG_GetPixelClr(&bmp, j, i, &R, &G, &B);
            rgbcolor.r = R;
            rgbcolor.g = G;
            rgbcolor.b = B;
            JFIF_RGBImage_Set(ei, i, j, &rgbcolor);
        }
    }

    TCIMG_Clear(&bmp);

    JFIF_YCCImage* yi  = JFIF_YCCImage_CreateFromRGBImage(ei);
    JFIF_YCCSubSampling* yss = JFIF_YCCSubSampling_CreateFromYCCImage(yi, H1V1);
    JFIF_YSSBlockImage* ybi = JFIF_YSSBlockImage_CreateFromYSS(yss);

    JFIF_YSSBlockImage_FDCT(ybi);
    JFIF_YSSBlockImage_FQuantize(ybi);
    JFIF_YSSBlockImage_FACRLE(ybi);
    JFIF_YSSBlockImage_IACRLE(ybi);
    JFIF_YSSBlockImage_IQuantize(ybi);
    JFIF_YSSBlockImage_IDCT(ybi);

    JFIF_YSSBlockImage_WriteFile(ybi, "ha.jpg");

    JFIF_YCCSubSampling* yssi = JFIF_YCCSubSampling_CreateFromYBI(ybi);
    JFIF_YCCImage* yii = JFIF_YCCImage_CreateFromYSS(yssi);
    JFIF_RGBImage* eii = JFIF_RGBImage_CreateFromYCCImage(yii);

    for (i = 0; i < eii->h; ++i)
    {
        for (j = 0; j < eii->w; ++j)
        {
            JFIF_RGBImage_Get(eii, i, j, &rgbcolor);
            R = (UINT8)round(rgbcolor.r);
            G = (UINT8)round(rgbcolor.g);
            B = (UINT8)round(rgbcolor.b);
            TCIMG_SetPixelClr(&bmp, j, i, R, G, B);
        }
    }

    TCIMG_SaveToFile(&bmp, "new.bmp");


    JFIF_RGBImage_Destroy(ei);
    JFIF_RGBImage_Destroy(eii);
    JFIF_YCCImage_Destroy(yi);
    JFIF_YCCImage_Destroy(yii);
    JFIF_YCCSubSampling_Destroy(yss);
#endif

    return 0;
}

