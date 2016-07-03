/*########################################################
 * guojpeg.c: a simple JPEG encoder and decoder
 * author: Guo Xiaoyong
 *########################################################*/


/*oooooooooooooooooooooooooooooo
 * matrix is stored column-wise
 * (i, j)-entry of a picture is the element lies in
 * i-th row and j-th column
 *oooooooooooooooooooooooooooooo*/



#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <memory.h>

#include "guojpeg.h"


#define SOI  0xD8
#define EOI  0xD9
#define APP0 0xE0
#define DQT  0xDB
#define DHT  0xC4
#define SOF0 0xC0
#define SOS  0xDA
#define COM  0xFE
#define DRI  0xDD

#define RST0 0xD0
#define RST1 0xD1
#define RST2 0xD2
#define RST3 0xD3
#define RST4 0xD4
#define RST5 0xD5
#define RST6 0xD6
#define RST7 0xD7



    /* The ITU-T standard provides some sample quantization
       tables (for luminance and chrominance) that are often
       the basis for many different quantization tables through
       a scaling function. see itu t81 page 147*/
    const float stdlqt[64] =
    {   16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68,109,103, 77,
        24, 35, 55, 64, 81,104,113, 92,
        49, 64, 78, 87,103,121,120,101,
        72, 92, 95, 98,112,100,103, 99
    };


    const float stdcqt[64] =
    {   17, 18, 24, 47, 99, 99, 99, 99,
        18, 21, 26, 66, 99, 99, 99, 99,
        24, 26, 56, 99, 99, 99, 99, 99,
        47, 66, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99
    };

    const float fullqt[64] =
    {   1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };


    /*oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
    void Index_2dto4d(int i, int j, int* x, int* y, int* ii, int* jj)
    {
        *x  = i / 8;
        *ii = i % 8;
        *y  = j / 8;
        *jj = j % 8;
    }


    void Index_4dto2d(int x, int y, int ii, int jj, int* i, int* j)
    {
        *i = x*8 + ii;
        *j = y*8 + jj;
    }

    void to8bits(unsigned char num, unsigned char* bits)
    {
        unsigned char mask = 0x0001;
        unsigned char i;

        for (i = 0; i < 8; ++i)
        {
            bits[7-i] = (unsigned char) ((num >> i) & mask);
        }
    }

    char* bits2str(unsigned char* bits, unsigned int numbits)
    {
        static char str[64];
        unsigned int i;

        for (i = 0; i < numbits; ++i)
        {
            str[i] = (bits[i] == 0) ? '0':'1';
        }
        str[numbits] = 0;

        return str;
    }


    int JPEG_FRLE(int zz[64], JFIF_RLE* rle)
    {
        int i, j, nz, end;

        for (i = 63; i >= 1; i--)
        {
            //fprintf(stderr, "zz[%d]=%d\n", i, zz[i]);
            if ( zz[i] != 0 ) break;
        }
        end = i;

        j = 0; // point to rle entries
        nz = 0;
        for (i = 1; i <= end; ++i)
        {
            if (zz[i] == 0 && nz < 15)
            {
                nz++;
            }
            else
            {
                rle[j].nz = nz;
                rle[j].num = zz[i];
                nz = 0;
                j++;
            }
        }

        if (end != 63)
        {
            rle[j].nz  = 0;
            rle[j].num = 0;
            return j;
        }
        else
        {
            return j-1;
        }
    }


    int JPEG_IRLE(JFIF_RLE* rle, int zz[64])
    {
        int i, j, pos;

        i = 0;
        pos = 1;
        while (pos < 63)
        {
            if (rle[i].nz == 0 && rle[i].num == 0)
            {
                for (j = pos; j < 64; ++j)
                {
                    zz[j] = 0;
                }

                pos = 63;
            }
            else
            {
                for (j = pos; j < pos+rle[i].nz; j++)
                {
                    assert( zz[j] == 0 );
                    zz[j] = 0;
                }
                assert( zz[j] == rle[i].num );
                zz[j] = rle[i].num;
                pos += (rle[i].nz+1);
                assert(pos < 64);
            }
            i++;
        }

        assert(pos == 63);
        return (i-1);
    }




    /* for debug */
    int JPEG_PRINTRLE(JFIF_RLE* rle)
    {
        int i = 0;
        while ( !(rle[i].nz==0 && rle[i].num == 0) && (i < 63) )
        {
            printf("rle[%d] = (%d, %d)\n", i, rle[i].nz, rle[i].num);
            i++;
        }

        printf("rle[%d] = (%d, %d)\nend\n", i, rle[i].nz, rle[i].num);
        return i;
    }


    unsigned char CombineRS(unsigned int a, unsigned int b)
    {
        return (unsigned char)( (a<<4)+b );
    }


    unsigned int* DivideRS(unsigned char a)
    {
        static unsigned int rs[2];
        rs[0] = (a >> 4);
        rs[1] = (a - (rs[0]<<4));

        return rs;
    }

    /* a sign, b additional bits */
    unsigned int CombineAdd(unsigned int a, unsigned int b, unsigned int catcode)
    {
        if (catcode == 0)
        {
            return 0;
        }
        else if (catcode == 1)
        {
            return a;
        }
        else
        {
            assert(  b < (unsigned int)(1<<(catcode-1)) );
            return  (a<<(catcode-1))+b;
        }
    }

    /*
    int CheckBits(unsigned int a, unsigned int catcode)
    {
         unsigned int mask = 1;
         int num = 0;
         int i;

         i = 0;
         while (i < 32)
         {
                 (a>>(31-i)) & mask
         }
    }*/

    unsigned int* DivideAdd(unsigned int a, unsigned int catcode)
    {
        static unsigned int add[2];
        if (catcode == 0)
        {
            add[0] = 0;
            add[1] = 0;
        }
        else if (catcode == 1)
        {
            assert(a == 0 || a == 1);
            add[0] = a;
            add[1] = 0;
        }
        else
        {
            add[0] = (a >> (catcode-1));
            add[1] = a - (add[0] << (catcode-1));
        }

        if (!(add[0] == 0 || add[0] == 1)) fprintf(stderr, "a = %u, catcode = %u\n", a, catcode);
        assert(add[0] == 0 || add[0] == 1);
        return add;
    }


    unsigned int* CatAddCode(int num)
    {
        /*  n[0] cat code,  n[1] sign, n[2] additional bits */
        static unsigned int n[3];

        if (num == 0)
        {
            n[0] = 0;
            n[1] = 0;
            n[2] = 0;

            return n;
        }
        else if (num == 1 || num == -1)
        {
            n[0] = 1;
            n[1] = (num == 1);
            n[2] = 0;

            return n;
        }


        n[1] = (unsigned int)(num > 0);
        num = abs(num);

        n[0] = 0u;
        while ( num >= (1<<n[0]) ) n[0]++;

        if (n[1] == 0)
        {
            n[2] = (1<<n[0]) - num - 1;
        }
        else if (n[1] == 1)
        {
            n[2] = num - ( 1<<(n[0]-1) );
        }

        if (!( n[2] < (1u<<(n[0]-1)))) fprintf(stderr, "%u, %u, %u, %d\n", n[0],n[1],n[2], num);;
        assert( n[2] < (1u<<(n[0]-1)));

        return n;
    }


    /* a catcode, b sign, c value */
    int CombineNum(unsigned int a, unsigned int b, unsigned int c)
    {
        if (!(b == 0 || b == 1)) fprintf(stderr, "b=%d\n", b);
        assert (b == 0 || b == 1);
        if (a == 0) return 0;
        if (a == 1) return (b==0)?-1:1;

        if (b == 0)
        {
            return -((int) (((1<<a)-1)-c) );
        }
        else
        {
            return (int)( c + (1<<(a-1)) );
        }
    }



    void JPEG_RLE2RSA(JFIF_RLE* rle, JFIF_RSA* rsa)
    {
        unsigned int* code;

        code = CatAddCode(rle[0].num);
        rsa[0].rs  = CombineRS(rle[0].nz, code[0]);
        rsa[0].add = CombineAdd(code[1], code[2], code[0]);

        assert(rsa[0].add < (1<<16));
    }


    void JPEG_RSA2RLE(JFIF_RSA* rsa, JFIF_RLE* rle)
    {
        unsigned int* rs;
        unsigned int* add;

        assert(rsa->add < (1<<16));

        rs  = DivideRS(rsa[0].rs);
        add = DivideAdd(rsa[0].add, rs[1]);
        rle[0].nz  = (int)rs[0];
        rle[0].num = CombineNum(rs[1], add[0], add[1]);

        assert(rle[0].nz < 16);
        assert(rle[0].num < 255*8);
    }



    void JPEG_RLE2RSAlen(JFIF_RLE* rle, JFIF_RSA* rsa, int len)
    {
        unsigned int* code;
        int i;

        for (i = 0; i < len; ++i)
        {
            code = CatAddCode(rle[i].num);
            rsa[i].rs  = CombineRS(rle[i].nz, code[0]);
            rsa[i].add = CombineAdd(code[1], code[2], code[0]);
        }
    }


    void JPEG_RSA2RLElen(JFIF_RSA* rsa, JFIF_RLE* rle, int len)
    {
        unsigned int* rs;
        unsigned int* add;
        int i;

        for (i = 0; i < len; ++i)
        {
            rs  = DivideRS(rsa[i].rs);
            add = DivideAdd(rsa[i].add, rs[1]);
            rle[i].nz  = rs[0];
            rle[i].num = CombineNum(rs[1], add[0], add[1]);
        }
    }


    int JPEG_IRSA(JFIF_RSA* rsa, JFIF_RLE* rle, int zz[64])
    {
        int i, j, pos;

        i = 0;
        pos = 1;
        while (pos < 63)
        {
            assert(rsa[i].add < (1<<16));
            JPEG_RSA2RLE(rsa+i, rle+i);

            if (rle[i].nz == 0 && rle[i].num == 0)
            {
                for (j = pos; j < 64; ++j)
                {
                    zz[j] = 0;
                }

                pos = 63;
            }
            else
            {
                for (j = pos; j < pos+rle[i].nz; j++)
                {
                    assert( zz[j] == 0 );
                    zz[j] = 0;
                }

                if (!( zz[j] == rle[i].num )) fprintf(stderr, "zz[%d] = %d, rle[%d] = (%d, %d), rsa[%d] = (%u, %u)\n", j, zz[j], i, rle[i].nz, rle[i].num, i, rsa[i].rs, rsa[i].add);
                assert( zz[j] == rle[i].num );
                zz[j] = rle[i].num;
                pos += (rle[i].nz+1);
                assert(pos < 64);
            }
            i++;
        }

        assert(pos == 63);
        return (i-1);
    }




    /*ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
    /* RGB2YCbCr and YCbCr2RGB: implemented according to
     * JPEG File Interchange Format Version 1.02, page 4 */


    /* tested */
    void RGB2YCbCr(JFIF_RGBcolor* rgbcolor, JFIF_YCCcolor* ycccolor)
    {
        float R, G, B, Y, Cr, Cb;

        R = rgbcolor->r;
        G = rgbcolor->g;
        B = rgbcolor->b;

        assert(R >= 0.0f && R <= 255.0f);
        assert(G >= 0.0f && G <= 255.0f);
        assert(B >= 0.0f && B <= 255.0f);

        Y  = +0.2990f*R + 0.5870f*G + 0.1140f*B + 000.0f;
        Cb = -0.1687f*R - 0.3313f*G + 0.5000f*B + 128.0f;
        Cr = +0.5000f*R - 0.4187f*G - 0.0813f*B + 128.0f;

        Y  = round(Y);
        Cb = round(Cb);
        Cr = round(Cr);

        if (Y  < 0.0f) Y  = 0.0f;
        if (Cb < 0.0f) Cb = 0.0f;
        if (Cr < 0.0f) Cr = 0.0f;

        if (Y  > 255.0f) Y  = 255.0f;
        if (Cb > 255.0f) Cb = 255.0f;
        if (Cr > 255.0f) Cr = 255.0f;

        if (Y  < 0.0f || Y  > 255.0f) printf("Y  = %f\n", Y );
        if (Cb < 0.0f || Cb > 255.0f) printf("Cb = %f\n", Cb);
        if (Cr < 0.0f || Cr > 255.0f) printf("Cr = %f\n", Cr);

        ycccolor->y  = Y;
        ycccolor->cr = Cr;
        ycccolor->cb = Cb;
    }


    /* tested */
    void YCbCr2RGB(JFIF_YCCcolor* ycccolor, JFIF_RGBcolor* rgbcolor)
    {
        float R, G, B, Y, Cr, Cb;

        Y  = ycccolor->y;
        Cb = ycccolor->cb;
        Cr = ycccolor->cr;

#if 0
        assert(Y  >= 0.0f && Y  <= 255.0f);
        assert(Cr >= 0.0f && Cr <= 255.0f);
        assert(Cb >= 0.0f && Cb <= 255.0f);
#endif

        R = Y + 1.40200f * (Cr-128.0f);
        G = Y - 0.34414f * (Cb-128.0f) - 0.71414f*(Cr-128.0f);
        B = Y + 1.77200f * (Cb-128.0f);

        R = round(R);
        G = round(G);
        B = round(B);

        if (R < 0.0f) R = 0.0f;
        if (G < 0.0f) G = 0.0f;
        if (B < 0.0f) B = 0.0f;

        if (R > 255.0f) R = 255.0f;
        if (G > 255.0f) G = 255.0f;
        if (B > 255.0f) B = 255.0f;

        if (R < 0.0f || R > 255.0f) printf("R = %f\n", R);
        if (G < 0.0f || G > 255.0f) printf("G = %f\n", G);
        if (B < 0.0f || B > 255.0f) printf("B = %f\n", B);

        rgbcolor->r = R;
        rgbcolor->g = G;
        rgbcolor->b = B;
    }



    /* fdct8 and idct8, lazy implementation of itu.t81 jpeg, page 31 */
    /* tested */
    void fdct8(float data[64], float out[64])
    {
        static const float co = M_PI/16.0f;
        int x, y, u, v;
        float sum;

        for (u = 0; u < 8; u++)
        {
            for (v = 0; v < 8; v++)
            {
                sum = 0.0f;
                for (x = 0; x < 8; ++x)
                {
                    for (y = 0; y < 8; ++y)
                    {
                        sum += ( data[x+y*8] * cos(co*(2*x+1)*u) * cos(co*(2*y+1)*v) );
                    }
                }

                if (u == 0) sum *= sqrt(1.0f/2.0f);
                if (v == 0) sum *= sqrt(1.0f/2.0f);

                out[u+v*8] = sum / 4.0f;
            }
        }
    }


    /* tested */
    void idct8(float data[64], float out[64])
    {
        static const float co = M_PI/16.0f;
        int x, y, u, v;
        float sum, tmp;

        for (x = 0; x < 8; x++)
        {
            for (y = 0; y < 8; y++)
            {
                sum = 0.0f;
                for (u = 0; u < 8; ++u)
                {
                    for (v = 0; v < 8; ++v)
                    {
                        tmp = data[u+v*8] * cos(co*(2*x+1)*u) * cos(co*(2*y+1)*v);
                        if (u == 0) tmp *= sqrt(1.0f/2.0f);
                        if (v == 0) tmp *= sqrt(1.0f/2.0f);
                        sum += tmp;
                    }
                }

                out[x+y*8] = sum / 4.0f;
            }
        }
    }


    void fQuant(float data[64], float qtable[64])
    {
        int i, j;
        for (i = 0; i < 8; ++i)
        {
            for (j = 0; j < 8; ++j)
            {
                data[i+j*8] = round( data[i+j*8] / qtable[i+j*8] );
            }
        }
    }


    void iQuant(float data[64], float qtable[64])
    {
        int i, j;
        for (i = 0; i < 8; ++i)
        {
            for (j = 0; j < 8; ++j)
            {
                data[i+j*8] *= qtable[i+j*8];
            }
        }
    }



    void fZigZag(float data[64], int* out)
    {
        /* ZigZag reordering matrix */
        static const int ZigZag[64] =
        {
            0, 1, 5, 6,14,15,27,28,
            2, 4, 7,13,16,26,29,42,
            3, 8,12,17,25,30,41,43,
            9,11,18,24,31,40,44,53,
            10,19,23,32,39,45,52,54,
            20,22,33,38,46,51,55,60,
            21,34,37,47,50,56,59,61,
            35,36,48,49,57,58,62,63
        };

        int i, j, idx;
        for (i = 0; i < 8; ++i)
        {
            for (j = 0; j < 8; ++j)
            {
                idx = ZigZag[i+j*8];
                out[idx] = (int)data[i+j*8];
            }
        }
    }


    void iZigZag(int data[64], float* out)
    {
        /* ZigZag reordering matrix */
        static const int ZigZag[64] =
        {
            0, 1, 5, 6,14,15,27,28,
            2, 4, 7,13,16,26,29,42,
            3, 8,12,17,25,30,41,43,
            9,11,18,24,31,40,44,53,
            10,19,23,32,39,45,52,54,
            20,22,33,38,46,51,55,60,
            21,34,37,47,50,56,59,61,
            35,36,48,49,57,58,62,63
        };

        int i, j, idx;
        for (i = 0; i < 8; ++i)
        {
            for (j = 0; j < 8; ++j)
            {
                idx = ZigZag[i+j*8];
                out[i+j*8] = (float)data[idx];
            }
        }
    }


    /*ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
    /* JFIF_RGBImage class */
    /* tested */
    JFIF_RGBImage* JFIF_RGBImage_Create(int h, int w, JFIF_CLR mode)
    {
        JFIF_RGBImage* ei;

        assert(h > 0);
        assert(w > 0);

        ei = (JFIF_RGBImage*)malloc( sizeof(JFIF_RGBImage) );
        ei->h = h;
        ei->w = w;
        ei->mode = mode;

        if (mode == GRAY)
        {
            ei->r = (float*)malloc( sizeof(float)*h*w );
            ei->g = NULL;
            ei->b = NULL;
            assert(ei->r);
        }
        else
        {
            ei->r = (float*)malloc( sizeof(float)*h*w );
            ei->g = (float*)malloc( sizeof(float)*h*w );
            ei->b = (float*)malloc( sizeof(float)*h*w );
            assert(ei->r);
            assert(ei->g);
            assert(ei->b);
        }

        return ei;
    }


    /* tested */
    JFIF_RGBImage* JFIF_RGBImage_CreateFromYCCImage(JFIF_YCCImage* yi)
    {
        JFIF_RGBImage* ei;
        ei = JFIF_RGBImage_Create(yi->h, yi->w, yi->mode);
        JFIF_YCC2RGB(yi, ei);

        return ei;
    }


    /* tested */
    void JFIF_RGBImage_Destroy(JFIF_RGBImage* ei)
    {
        free(ei->r);
        free(ei->g);
        free(ei->b);
        free(ei);
    }


    /* tested */
    void JFIF_RGBImage_Set(JFIF_RGBImage* ei, int i, int j, JFIF_RGBcolor* rgbcolor)
    {
        assert(i >= 0 && i < ei->h);
        assert(j >= 0 && j < ei->w);

        if ( ei->mode == GRAY )
        {
            ei->r[j*ei->h + i] = rgbcolor->r;
        }
        else
        {
            ei->r[j*ei->h + i] = rgbcolor->r;
            ei->g[j*ei->h + i] = rgbcolor->g;
            ei->b[j*ei->h + i] = rgbcolor->b;
        }
    }


    /* tested */
    void JFIF_RGBImage_Get(JFIF_RGBImage* ei, int i, int j, JFIF_RGBcolor* rgbcolor)
    {
        assert(i >= 0 && i < ei->h);
        assert(j >= 0 && j < ei->w);

        if ( ei->mode == GRAY )
        {
            rgbcolor->r = ei->r[j*ei->h + i];
            rgbcolor->g = 0.0f;
            rgbcolor->b = 0.0f;
        }
        else
        {
            rgbcolor->r = ei->r[j*ei->h + i];
            rgbcolor->g = ei->g[j*ei->h + i];
            rgbcolor->b = ei->b[j*ei->h + i];
        }
    }



    /*ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo*/
    /* JFIF_YCCImage class */
    /* tested */
    JFIF_YCCImage* JFIF_YCCImage_Create(int h, int w, JFIF_CLR mode)
    {
        JFIF_YCCImage* yccimage;

        assert(h > 0);
        assert(w > 0);

        yccimage = (JFIF_YCCImage*)malloc( sizeof(JFIF_YCCImage) );
        yccimage->h = h;
        yccimage->w = w;
        yccimage->mode = mode;

        if (mode == GRAY)
        {
            yccimage->y  = (float*)malloc( sizeof(float)*h*w );
            yccimage->cr = NULL;
            yccimage->cb = NULL;
            assert(yccimage->y);
        }
        else
        {
            yccimage->y  = (float*)malloc( sizeof(float)*h*w );
            yccimage->cr = (float*)malloc( sizeof(float)*h*w );
            yccimage->cb = (float*)malloc( sizeof(float)*h*w );
            assert(yccimage->y);
            assert(yccimage->cr);
            assert(yccimage->cb);
        }

        return yccimage;
    }


    /* tested */
    JFIF_YCCImage* JFIF_YCCImage_CreateFromRGBImage(JFIF_RGBImage* ei)
    {
        JFIF_YCCImage* yi;
        yi = JFIF_YCCImage_Create(ei->h, ei->w, ei->mode);
        JFIF_RGB2YCC(ei, yi);

        return yi;
    }


    JFIF_YCCImage* JFIF_YCCImage_CreateFromYSS(JFIF_YCCSubSampling* yss)
    {
        JFIF_YCCImage* yi;
        yi = JFIF_YCCImage_Create(yss->yh, yss->yw, yss->mode);
        JFIF_YSS2YCC(yss, yi);

        return yi;
    }


    /* tested */
    void JFIF_YCCImage_Destroy(JFIF_YCCImage* yimg)
    {
        free(yimg->y);
        free(yimg->cr);
        free(yimg->cb);
        free(yimg);
    }


    /* tested */
    void JFIF_YCCImage_Set(JFIF_YCCImage* yi, int i, int j, JFIF_YCCcolor* ycccolor)
    {
        assert(i >= 0 && i < yi-> h);
        assert(j >= 0 && j < yi-> w);

        if (yi->mode == GRAY)
        {
            yi->y [j*yi->h + i] = ycccolor->y;
        }
        else
        {
            yi->y [j*yi->h + i] = ycccolor->y;
            yi->cr[j*yi->h + i] = ycccolor->cr;
            yi->cb[j*yi->h + i] = ycccolor->cb;
        }
    }


    /* tested */
    void JFIF_YCCImage_Get(JFIF_YCCImage* yi, int i, int j, JFIF_YCCcolor* ycccolor)
    {
        if (yi->mode == GRAY)
        {
            ycccolor->y  = yi->y [j*yi->h + i];
            ycccolor->cr = 0.0f;
            ycccolor->cb = 0.0f;
        }
        else
        {
            ycccolor->y  = yi->y [j*yi->h + i];
            ycccolor->cr = yi->cr[j*yi->h + i];
            ycccolor->cb = yi->cb[j*yi->h + i];
        }
    }


    void JFIF_YCCImage_Set_Y(JFIF_YCCImage* yi, int i, int j, float value)
    {
        assert(yi->mode != GRAY);
        yi->y[j*yi->h+i] = value;
    }

    void JFIF_YCCImage_Set_Cr(JFIF_YCCImage* yi, int i, int j, float value)
    {
        assert(yi->mode != GRAY);
        yi->cr[j*yi->h+i] = value;
    }

    void JFIF_YCCImage_Set_Cb(JFIF_YCCImage* yi, int i, int j, float value)
    {
        assert(yi->mode != GRAY);
        yi->cb[j*yi->h+i] = value;
    }


    /* tested */
    float JFIF_YCCSubSampling_Get_Y(JFIF_YCCSubSampling* yss, int i, int j)
    {
        assert(i >= 0 && i < yss->yh);
        assert(j >= 0 && j < yss->yw);

        return yss->y[j*yss->yh+i];
    }

    /* tested */
    float JFIF_YCCSubSampling_Get_Cr(JFIF_YCCSubSampling* yss, int i, int j)
    {
        assert(i >= 0 && i < yss->ch);
        assert(j >= 0 && j < yss->cw);

        return yss->cr[j*yss->ch+i];
    }

    /* tested */
    float JFIF_YCCSubSampling_Get_Cb(JFIF_YCCSubSampling* yss, int i, int j)
    {
        assert(i >= 0 && i < yss->ch);
        assert(j >= 0 && j < yss->cw);

        return yss->cb[j*yss->ch+i];
    }

    /* tested */
    void JFIF_YCCSubSampling_Set_Y(JFIF_YCCSubSampling* yss, int i, int j, float value)
    {
        assert(yss->mode != GRAY);
        assert(i >=0 && i < yss->yh);
        assert(j >=0 && j < yss->yw);

        yss->y[j*yss->yh+i] = value;
    }

    /* tested */
    void JFIF_YCCSubSampling_Set_Cr(JFIF_YCCSubSampling* yss, int i, int j, float value)
    {
        assert(yss->mode != GRAY);
        assert(i >= 0 && i < yss->ch);
        assert(j >= 0 && j < yss->cw);

        yss->cr[j*yss->ch+i] = value;
    }


    /* tested */
    void JFIF_YCCSubSampling_Set_Cb(JFIF_YCCSubSampling* yss, int i, int j, float value)
    {
        assert(yss->mode != GRAY);
        assert(i >= 0 && i < yss->ch);
        assert(j >= 0 && j < yss->cw);

        yss->cb[j*yss->ch+i] = value;
    }


    /* tested */
    void JFIF_RGB2YCC(JFIF_RGBImage* ei, JFIF_YCCImage* yi)
    {
        JFIF_RGBcolor rgbcolor;
        JFIF_YCCcolor ycccolor;
        int i, j, h, w;

        assert(ei->mode == yi->mode);

        h = ei->h;
        w = ei->w;

        if (ei->mode == GRAY)
        {
            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    JFIF_RGBImage_Get(ei, i, j, &rgbcolor);
                    ycccolor.y = rgbcolor.r;
                    JFIF_YCCImage_Set(yi, i, j, &ycccolor);
                }
            }
        }
        else
        {
            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    JFIF_RGBImage_Get(ei, i, j, &rgbcolor);
                    RGB2YCbCr(&rgbcolor, &ycccolor);
                    JFIF_YCCImage_Set(yi, i, j, &ycccolor);
                }
            }
        }
    }


    /* tested */
    void JFIF_YCC2RGB(JFIF_YCCImage* yi, JFIF_RGBImage* ei)
    {
        JFIF_RGBcolor rgbcolor;
        JFIF_YCCcolor ycccolor;
        int i, j, h, w;

        assert(ei->mode == yi->mode);

        h = yi->h;
        w = yi->w;

        if (yi->mode == GRAY)
        {
            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    JFIF_YCCImage_Get(yi, i, j, &ycccolor);
                    rgbcolor.r = ycccolor.y;
                    JFIF_RGBImage_Set(ei, i, j, &rgbcolor);
                }
            }
        }
        else
        {
            for (i = 0; i < h; ++i)
            {
                for (j = 0; j < w; ++j)
                {
                    JFIF_YCCImage_Get(yi, i, j, &ycccolor);
                    YCbCr2RGB(&ycccolor, &rgbcolor);
                    JFIF_RGBImage_Set(ei, i, j, &rgbcolor);
                }
            }
        }
    }


    /* tested */
    JFIF_YCCSubSampling* JFIF_YCCSubSampling_Create(int h, int w, JFIF_CLR mode, JFIF_SSM ssm)
    {
        JFIF_YCCSubSampling* yss;
        assert(h > 0);
        assert(w > 0);

        yss = (JFIF_YCCSubSampling*)malloc( sizeof(JFIF_YCCSubSampling) );
        assert(yss);
        yss->ssm  = ssm;
        yss->mode = mode;

        if (GRAY == mode)
        {
            yss->yh  = h;
            yss->yw  = w;
            yss->ch  = 0;
            yss->cw  = 0;
            yss->y   = (float*)malloc( sizeof(float)*h*w );
            yss->cr  = NULL;
            yss->cb  = NULL;

            assert(yss->y);
            assert(ssm == YONLY);
        }
        else
        {
            if (H1V1 == ssm)
            {
                yss->yh = h;
                yss->yw = w;
                yss->ch = h;
                yss->cw = w;

                yss->y  = (float*)malloc( sizeof(float)*h*w );
                yss->cr = (float*)malloc( sizeof(float)*h*w );
                yss->cb = (float*)malloc( sizeof(float)*h*w );

                assert(yss->y);
                assert(yss->cr);
                assert(yss->cb);
            }
            else if (H2V1 == ssm)
            {
                yss->yh  = h;
                yss->yw  = w;
                yss->ch  = h;
                yss->cw  = w/2 + !!(w%2);

                yss->y  = (float*)malloc( sizeof(float)*yss->yh*yss->yw);
                yss->cr = (float*)malloc( sizeof(float)*yss->ch*yss->cw);
                yss->cb = (float*)malloc( sizeof(float)*yss->ch*yss->cw);

                assert(yss->y);
                assert(yss->cr);
                assert(yss->cb);
            }
            else if (H2V2 == ssm)
            {
                yss->yh = h;
                yss->yw = w;
                yss->ch = h/2 + !!(h%2);
                yss->cw = w/2 + !!(w%2);

                yss->y  = (float*)malloc( sizeof(float)*yss->yh*yss->yw);
                yss->cr = (float*)malloc( sizeof(float)*yss->ch*yss->cw);
                yss->cb = (float*)malloc( sizeof(float)*yss->ch*yss->cw);

                assert(yss->y);
                assert(yss->cr);
                assert(yss->cb);
            }
            else
            {
                fprintf(stderr, "JFIF_YCCSubSampling_Create: sampling mode node defined!\nOnly support H1V1, H2V1, H2V2!\n");
                free(yss);
                yss = NULL;
            }
        }

        return yss;
    }


    /* tested */
    JFIF_YCCSubSampling* JFIF_YCCSubSampling_CreateFromYCCImage(JFIF_YCCImage* yi, JFIF_SSM ssm)
    {
        JFIF_YCCSubSampling* yss;
        yss = JFIF_YCCSubSampling_Create(yi->h, yi->w, yi->mode, ssm);
        JFIF_YCC2YSS(yi, yss);
        return yss;
    }


    JFIF_YCCSubSampling* JFIF_YCCSubSampling_CreateFromYBI(JFIF_YSSBlockImage* ybi)
    {
        JFIF_YCCSubSampling* yss;
        yss = JFIF_YCCSubSampling_Create(ybi->oyh, ybi->oyw, ybi->mode, ybi->ssm);
        JFIF_YBI2YSS(ybi, yss);
        return yss;
    }


    /* tested */
    void JFIF_YCC2YSS(JFIF_YCCImage* yi, JFIF_YCCSubSampling* yss)
    {
        int i, j;
        JFIF_YCCcolor c1, c2, c3, c4;
        float value;

        if (GRAY == yi->mode)
        {
            memcpy(yss->y, yi->y, yi->h*yi->w);
        }
        else
        {
            if ( H1V1 == yss->ssm )
            {
                memcpy(yss->y,  yi->y,  sizeof(float)*yi->h*yi->w);
                memcpy(yss->cr, yi->cr, sizeof(float)*yi->h*yi->w);
                memcpy(yss->cb, yi->cb, sizeof(float)*yi->h*yi->w);
            }
            else if ( H2V1 == yss->ssm )
            {
                memcpy(yss->y,  yi->y,  sizeof(float)*yi->h*yi->w);
                for (i = 0; i < yss->ch; ++i)
                {
                    for (j = 0; j < yss->cw; ++j)
                    {
                        JFIF_YCCImage_Get(yi, i, 2*j+0, &c1);

                        if (2*j+1 == yi->w)
                        {
                            c2 = c1;
                        }
                        else
                        {
                            JFIF_YCCImage_Get(yi, i, 2*j+1, &c2);
                        }

                        value = (c1.cr + c2.cr)/2.0f;
                        JFIF_YCCSubSampling_Set_Cr(yss, i, j, value);


                        value = (c1.cb + c2.cb)/2.0f;
                        JFIF_YCCSubSampling_Set_Cb(yss, i, j, value);
                    }
                }
            }
            else if ( H2V2 == yss->ssm )
            {
                memcpy(yss->y,  yi->y,  sizeof(float)*yi->h*yi->w);
                for (i = 0; i < yss->ch; ++i)
                {
                    for (j = 0; j < yss->cw; ++j)
                    {
                        JFIF_YCCImage_Get(yi, 2*i+0, 2*j+0, &c1);

                        if (2*i+1 == yi->h)
                        {
                            c2 = c1;
                        }
                        else
                        {
                            JFIF_YCCImage_Get(yi, 2*i+1, 2*j+0, &c2);
                        }

                        if (2*j+1 == yi->w)
                        {
                            c3 = c1;
                        }
                        else
                        {
                            JFIF_YCCImage_Get(yi, 2*i+0, 2*j+1, &c3);
                        }

                        if (2*i+1 == yi->h || 2*j+1 == yi->w)
                        {
                            c4 = c1;
                        }
                        else
                        {
                            JFIF_YCCImage_Get(yi, 2*i+1, 2*j+1, &c4);
                        }

                        value = (c1.cr + c2.cr + c3.cr + c4.cr)/4.0f;
                        JFIF_YCCSubSampling_Set_Cr(yss, i, j, value);

                        value = (c1.cb + c2.cb + c3.cb + c4.cb)/4.0f;
                        JFIF_YCCSubSampling_Set_Cb(yss, i, j, value);
                    }
                }
            }
            else
            {
                fprintf(stderr, "JFIF_YCC2YSS: mode not defined!\n");
            }
        }
    }


    /* tested */
    void JFIF_YCCSubSampling_Destroy(JFIF_YCCSubSampling* yss)
    {
        free(yss->y);
        free(yss->cr);
        free(yss->cb);
        free(yss);
    }

    /* tested */
    void JFIF_YSS2YCC(JFIF_YCCSubSampling* yss, JFIF_YCCImage* yi)
    {
        int i, j;
        float value;

        if (GRAY == yss->mode)
        {
            memcpy(yi->y, yss->y, yss->yh*yss->yw);
        }
        else
        {
            if (H1V1 == yss->ssm)
            {
                memcpy(yi->y,  yss->y,  sizeof(float)*yi->h*yi->w);
                memcpy(yi->cr, yss->cr, sizeof(float)*yi->h*yi->w);
                memcpy(yi->cb, yss->cb, sizeof(float)*yi->h*yi->w);
            }
            else if (H2V1 == yss->ssm)
            {
                memcpy(yi->y,  yss->y,  sizeof(float)*yi->h*yi->w);
                for (i = 0; i < yi->h; ++i)
                {
                    for (j = 0; j < yi->w; ++j)
                    {
                        value = JFIF_YCCSubSampling_Get_Cr(yss, i, j/2);
                        JFIF_YCCImage_Set_Cr(yi, i, j, value);

                        value = JFIF_YCCSubSampling_Get_Cb(yss, i, j/2);
                        JFIF_YCCImage_Set_Cb(yi, i, j, value);
                    }
                }
            }
            else if (H2V2 == yss->ssm)
            {
                memcpy(yi->y,  yss->y,  sizeof(float)*yi->h*yi->w);
                for (i = 0; i < yi->h; ++i)
                {
                    for (j = 0; j < yi->w; ++j)
                    {
                        value = JFIF_YCCSubSampling_Get_Cr(yss, i/2, j/2);
                        JFIF_YCCImage_Set_Cr(yi, i, j, value);

                        value = JFIF_YCCSubSampling_Get_Cb(yss, i/2, j/2);
                        JFIF_YCCImage_Set_Cb(yi, i, j, value);
                    }
                }
            }
            else
            {
                fprintf(stderr, "JFIF_YSS2YCC: subsampling mode not defined!\n");
            }
        }
    }


    /* JFIF_YSSBlockImage class */
    JFIF_YSSBlockImage* JFIF_YSSBlockImage_Create(int oyh, int oyw, int och, int ocw, JFIF_CLR mode, JFIF_SSM ssm)
    {
        JFIF_YSSBlockImage* ybi;
        int yh, yw, ch, cw;

        ybi = (JFIF_YSSBlockImage*)malloc( sizeof(JFIF_YSSBlockImage) );

        ybi->oyh  = oyh;
        ybi->oyw  = oyw;
        ybi->och  = och;
        ybi->ocw  = ocw;
        ybi->yh   = oyh/8 + !!(oyh%8);
        ybi->yw   = oyw/8 + !!(oyw%8);
        ybi->ch   = och/8 + !!(och%8);
        ybi->cw   = ocw/8 + !!(ocw%8);
        ybi->mode = mode;
        ybi->ssm  = ssm;

        yh = ybi->yh;
        yw = ybi->yw;
        ch = ybi->ch;
        cw = ybi->cw;

        if (ybi->mode == GRAY)
        {
            ybi->y  = (JFIF_b8x8*)malloc( sizeof(JFIF_b8x8)*yh*yw );
            ybi->cr = NULL;
            ybi->cb = NULL;
        }
        else
        {
            ybi->y  = (JFIF_b8x8*)malloc( sizeof(JFIF_b8x8)*yh*yw );
            ybi->cr = (JFIF_b8x8*)malloc( sizeof(JFIF_b8x8)*ch*cw );
            ybi->cb = (JFIF_b8x8*)malloc( sizeof(JFIF_b8x8)*ch*cw );
        }

        //JFIF_YSSBlockImage_SetQT(ybi, stdlqt, stdcqt);
        JFIF_YSSBlockImage_SetQT(ybi, fullqt, fullqt);
        ybi->buffer = guobuffer_Create(yh*yw*16+1024);

        return ybi;
    }


    JFIF_YSSBlockImage* JFIF_YSSBlockImage_CreateFromYSS(JFIF_YCCSubSampling* yss)
    {
        JFIF_YSSBlockImage* ybi;
        ybi = JFIF_YSSBlockImage_Create(yss->yh, yss->yw, yss->ch, yss->cw, yss->mode, yss->ssm);
        JFIF_YSS2YBI(yss, ybi);
        return ybi;
    }


    void JFIF_YSSBlockImage_Destroy(JFIF_YSSBlockImage* ybi)
    {
        free(ybi->y);
        free(ybi->cr);
        free(ybi->cb);
        guobuffer_Destroy(ybi->buffer);
        free(ybi);
    }


    void JFIF_YSSBlockImage_Set_Y(JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj, float value)
    {
        int yh;
        yh = ybi->yh;
        ybi->y[y*yh+x].data[ii+jj*8] = value;
    }

    void JFIF_YSSBlockImage_Set_Cr(JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj, float value)
    {
        int ch;
        ch = ybi->ch;
        ybi->cr[y*ch+x].data[ii+jj*8] = value;
    }

    void JFIF_YSSBlockImage_Set_Cb(JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj, float value)
    {
        int ch;
        ch = ybi->ch;
        ybi->cb[y*ch+x].data[ii+jj*8] = value;
    }


    float JFIF_YSSBlockImage_Get_Y(JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj)
    {
        int yh;
        yh = ybi->yh;
        return ybi->y[y*yh+x].data[ii+jj*8];
    }


    float JFIF_YSSBlockImage_Get_Cr(JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj)
    {
        int ch;
        ch = ybi->ch;
        return ybi->cr[y*ch+x].data[ii+jj*8];
    }


    float JFIF_YSSBlockImage_Get_Cb(JFIF_YSSBlockImage* ybi, int x, int y, int ii, int jj)
    {
        int ch;
        ch = ybi->ch;

        assert(ii >= 0 && ii < 8);
        assert(jj >= 0 && jj < 8);
        assert(x >= 0 && x < ybi->ch);
        assert(y >= 0 && y < ybi->cw);

        return ybi->cb[y*ch+x].data[ii+jj*8];
    }


    void JFIF_YSSBlockImage_FillBlock_Y(JFIF_YSSBlockImage* ybi)
    {
        int k, i, j, rw, rh;
        float value;

        rw = ybi->oyw % 8;
        rh = ybi->oyh % 8;

        if (rw != 0 && rh != 0)
        {
            for (k = 0; k < (ybi->yh-1); ++k)
            {
                for (i = 0; i < 8; ++i)
                {
                    for (j = rw; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Y(ybi, k, ybi->yw-1, i, j-1);
                        JFIF_YSSBlockImage_Set_Y(ybi, k, ybi->yw-1, i, j, value);
                    }
                }
            }

            for (k = 0; k < (ybi->yw-1); ++k)
            {
                for (i = rh; i < 8; ++i)
                {
                    for (j = 0; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Y(ybi, ybi->yh-1, k, i-1, j);
                        JFIF_YSSBlockImage_Set_Y(ybi, ybi->yh-1, k, i, j, value);
                    }
                }
            }

            for (i = rh; i < 8; ++i)
            {
                for (j = 0; j < 8; ++j)
                {
                    value = JFIF_YSSBlockImage_Get_Y(ybi, ybi->yh-1, ybi->yw-1, i-1, j);
                    JFIF_YSSBlockImage_Set_Y(ybi, ybi->yh-1, ybi->yw-1, i, j, value);
                }
            }

            for (i = 0; i < 8; ++i)
            {
                for (j = rw; i < 8; ++i)
                {
                    value = JFIF_YSSBlockImage_Get_Y(ybi, ybi->yh-1, ybi->yw-1, i, j-1);
                    JFIF_YSSBlockImage_Set_Y(ybi, ybi->yh-1, ybi->yw-1, i, j, value);
                }
            }
        }
        else if (rw != 0 && rh == 0)
        {
            for (k = 0; k < ybi->yh; ++k)
            {
                for (i = 0; i < 8; ++i)
                {
                    for (j = rw; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Y(ybi, k, ybi->yw-1, i, j-1);
                        JFIF_YSSBlockImage_Set_Y(ybi, k, ybi->yw-1, i, j, value);
                    }
                }
            }
        }
        else if (rw == 0 && rh != 0 )
        {
            for (k = 0; k < ybi->yw; ++k)
            {
                for (i = rh; i < 8; ++i)
                {
                    for (j = 0; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Y(ybi, ybi->yh-1, k, i-1, j);
                        JFIF_YSSBlockImage_Set_Y(ybi, ybi->yh-1, k, i, j, value);
                    }
                }
            }
        }
    }


    void JFIF_YSSBlockImage_FillBlock_C(JFIF_YSSBlockImage* ybi)
    {
        int k, i, j, rw, rh;
        float value;

        rw = ybi->ocw % 8;
        rh = ybi->och % 8;

        if (rw != 0 && rh != 0)
        {
            for (k = 0; k < (ybi->ch-1); ++k)
            {
                for (i = 0; i < 8; ++i)
                {
                    for (j = rw; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Cb(ybi, k, ybi->yw-1, i, j-1);
                        JFIF_YSSBlockImage_Set_Cb(ybi, k, ybi->yw-1, i, j, value);

                        value = JFIF_YSSBlockImage_Get_Cr(ybi, k, ybi->yw-1, i, j-1);
                        JFIF_YSSBlockImage_Set_Cr(ybi, k, ybi->yw-1, i, j, value);
                    }
                }
            }

            for (k = 0; k < (ybi->cw-1); ++k)
            {
                for (i = rh; i < 8; ++i)
                {
                    for (j = 0; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Cr(ybi, ybi->yh-1, k, i-1, j);
                        JFIF_YSSBlockImage_Set_Cr(ybi, ybi->yh-1, k, i, j, value);

                        value = JFIF_YSSBlockImage_Get_Cb(ybi, ybi->yh-1, k, i-1, j);
                        JFIF_YSSBlockImage_Set_Cb(ybi, ybi->yh-1, k, i, j, value);
                    }
                }
            }

            for (i = rh; i < 8; ++i)
            {
                for (j = 0; j < 8; ++j)
                {
                    value = JFIF_YSSBlockImage_Get_Cb(ybi, ybi->ch-1, ybi->cw-1, i-1, j);
                    JFIF_YSSBlockImage_Set_Cb(ybi, ybi->ch-1, ybi->cw-1, i, j, value);

                    value = JFIF_YSSBlockImage_Get_Cr(ybi, ybi->ch-1, ybi->cw-1, i-1, j);
                    JFIF_YSSBlockImage_Set_Cr(ybi, ybi->ch-1, ybi->cw-1, i, j, value);
                }
            }

            for (i = 0; i < 8; ++i)
            {
                for (j = rw; i < 8; ++i)
                {
                    value = JFIF_YSSBlockImage_Get_Cr(ybi, ybi->ch-1, ybi->cw-1, i, j-1);
                    JFIF_YSSBlockImage_Set_Cr(ybi, ybi->ch-1, ybi->cw-1, i, j, value);

                    value = JFIF_YSSBlockImage_Get_Cb(ybi, ybi->ch-1, ybi->cw-1, i, j-1);
                    JFIF_YSSBlockImage_Set_Cb(ybi, ybi->ch-1, ybi->cw-1, i, j, value);
                }
            }
        }
        else if (rw != 0 && rh == 0)
        {
            for (k = 0; k < ybi->ch; ++k)
            {
                for (i = 0; i < 8; ++i)
                {
                    for (j = rw; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Cr(ybi, k, ybi->cw-1, i, j-1);
                        JFIF_YSSBlockImage_Set_Cr(ybi, k, ybi->cw-1, i, j, value);

                        value = JFIF_YSSBlockImage_Get_Cb(ybi, k, ybi->cw-1, i, j-1);
                        JFIF_YSSBlockImage_Set_Cb(ybi, k, ybi->cw-1, i, j, value);
                    }
                }
            }
        }
        else if (rw == 0 && rh != 0 )
        {
            for (k = 0; k < ybi->cw; ++k)
            {
                for (i = rh; i < 8; ++i)
                {
                    for (j = 0; j < 8; ++j)
                    {
                        value = JFIF_YSSBlockImage_Get_Cr(ybi, ybi->ch-1, k, i-1, j);
                        JFIF_YSSBlockImage_Set_Cr(ybi, ybi->ch-1, k, i, j, value);

                        value = JFIF_YSSBlockImage_Get_Cb(ybi, ybi->ch-1, k, i-1, j);
                        JFIF_YSSBlockImage_Set_Cb(ybi, ybi->ch-1, k, i, j, value);
                    }
                }
            }
        }
    }

    void JFIF_YSSBlockImage_FillBlock(JFIF_YSSBlockImage* ybi)
    {
        JFIF_YSSBlockImage_FillBlock_Y(ybi);
        JFIF_YSSBlockImage_FillBlock_C(ybi);
    }


    void JFIF_YSS2YBI(JFIF_YCCSubSampling* yss, JFIF_YSSBlockImage* ybi)
    {
        int i, j, x, y, ii, jj;
        float value;

        assert(yss->mode == ybi->mode);

        if (GRAY == yss->mode)
        {
            for (i = 0; i < yss->yh; ++i)
            {
                for (j = 0; j < yss->yw; ++j)
                {
                    Index_2dto4d(i, j, &x, &y, &ii, &jj);
                    value = JFIF_YCCSubSampling_Get_Y(yss, i, j);
                    JFIF_YSSBlockImage_Set_Y(ybi, x, y, ii, jj, value);
                }
            }
        }
        else
        {
            for (i = 0; i < yss->yh; ++i)
            {
                for (j = 0; j < yss->yw; ++j)
                {
                    Index_2dto4d(i, j, &x, &y, &ii, &jj);
                    value = JFIF_YCCSubSampling_Get_Y(yss, i, j);
                    JFIF_YSSBlockImage_Set_Y(ybi, x, y, ii, jj, value);
                }
            }

            for (i = 0; i < yss->ch; ++i)
            {
                for (j = 0; j < yss->cw; ++j)
                {
                    Index_2dto4d(i, j, &x, &y, &ii, &jj);

                    value = JFIF_YCCSubSampling_Get_Cr(yss, i, j);
                    JFIF_YSSBlockImage_Set_Cr(ybi, x, y, ii, jj, value);

                    value = JFIF_YCCSubSampling_Get_Cb(yss, i, j);
                    JFIF_YSSBlockImage_Set_Cb(ybi, x, y, ii, jj, value);
                }
            }
        }

        JFIF_YSSBlockImage_FillBlock(ybi);
    }


    void JFIF_YBI2YSS(JFIF_YSSBlockImage* ybi, JFIF_YCCSubSampling* yss)
    {
        int i, j, x, y, ii, jj;
        float value;

        assert(ybi->mode == yss->mode);

        if (GRAY == ybi->mode)
        {
            for (i = 0; i < ybi->oyh; ++i)
            {
                for (j = 0; j < ybi->oyw; ++j)
                {
                    Index_2dto4d(i, j, &x, &y, &ii, &jj);
                    value = JFIF_YSSBlockImage_Get_Y(ybi, x, y, ii, jj);
                    JFIF_YCCSubSampling_Set_Y(yss, i, j, value);
                }
            }
        }
        else
        {
            for (i = 0; i < ybi->oyh; ++i)
            {
                for (j = 0; j < ybi->oyw; ++j)
                {
                    Index_2dto4d(i, j, &x, &y, &ii, &jj);
                    value = JFIF_YSSBlockImage_Get_Y(ybi, x, y, ii, jj);
                    JFIF_YCCSubSampling_Set_Y(yss, i, j, value);
                }
            }

            for (i = 0; i < ybi->och; ++i)
            {
                for (j = 0; j < ybi->ocw; ++j)
                {
                    Index_2dto4d(i, j, &x, &y, &ii, &jj);
                    value = JFIF_YSSBlockImage_Get_Cr(ybi, x, y, ii, jj);
                    JFIF_YCCSubSampling_Set_Cr(yss, i, j, value);

                    value = JFIF_YSSBlockImage_Get_Cb(ybi, x, y, ii, jj);
                    JFIF_YCCSubSampling_Set_Cb(yss, i, j, value);
                }
            }
        }
    }




    float* JFIF_YSSBlockImage_GetBlock_Y(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->y[j*ybi->yh+i].data;
    }

    float* JFIF_YSSBlockImage_GetBlock_Cr(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cr[j*ybi->ch+i].data;
    }

    float* JFIF_YSSBlockImage_GetBlock_Cb(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cb[j*ybi->ch+i].data;
    }


    int* JFIF_YSSBlockImage_GetBlock_Yzz(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->y[j*ybi->yh+i].zz;
    }

    int* JFIF_YSSBlockImage_GetBlock_Crzz(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cr[j*ybi->ch+i].zz;
    }

    int* JFIF_YSSBlockImage_GetBlock_Cbzz(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cb[j*ybi->ch+i].zz;
    }


    JFIF_RLE* JFIF_YSSBlockImage_GetBlock_Yrle(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->y[j*ybi->yh+i].rle;
    }


    JFIF_RLE* JFIF_YSSBlockImage_GetBlock_Cbrle(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cb[j*ybi->ch+i].rle;
    }


    JFIF_RLE* JFIF_YSSBlockImage_GetBlock_Crrle(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cr[j*ybi->ch+i].rle;
    }


    JFIF_RSA* JFIF_YSSBlockImage_GetBlock_Yrsa(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->y[j*ybi->yh+i].rsa;
    }


    JFIF_RSA* JFIF_YSSBlockImage_GetBlock_Cbrsa(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cb[j*ybi->ch+i].rsa;
    }


    JFIF_RSA* JFIF_YSSBlockImage_GetBlock_Crrsa(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cr[j*ybi->ch+i].rsa;
    }

    void JFIF_YSSBlockImage_SetYrlelen(JFIF_YSSBlockImage* ybi, int i, int j, int value)
    {
        ybi->y[j*ybi->yh+i].rlelen = value;
    }

    void JFIF_YSSBlockImage_SetCrrlelen(JFIF_YSSBlockImage* ybi, int i, int j, int value)
    {
        ybi->cr[j*ybi->ch+i].rlelen = value;
    }

    void JFIF_YSSBlockImage_SetCbrlelen(JFIF_YSSBlockImage* ybi, int i, int j, int value)
    {
        ybi->cb[j*ybi->ch+i].rlelen = value;
    }

    int JFIF_YSSBlockImage_GetYrlelen(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->y[j*ybi->yh+i].rlelen;
    }

    int JFIF_YSSBlockImage_GetCrrlelen(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cr[j*ybi->ch+i].rlelen;
    }

    int JFIF_YSSBlockImage_GetCbrlelen(JFIF_YSSBlockImage* ybi, int i, int j)
    {
        return ybi->cb[j*ybi->ch+i].rlelen;
    }


    void JFIF_YSSBlockImage_SetBlock_YDiffDC(JFIF_YSSBlockImage* ybi, int i, int j, int DiffDC)
    {
        ybi->y[j*ybi->yh+i].DiffDC = DiffDC;
    }

    void JFIF_YSSBlockImage_SetBlock_CbDiffDC(JFIF_YSSBlockImage* ybi, int i, int j, int DiffDC)
    {
        ybi->cb[j*ybi->ch+i].DiffDC = DiffDC;
    }

    void JFIF_YSSBlockImage_SetBlock_CrDiffDC(JFIF_YSSBlockImage* ybi, int i, int j, int DiffDC)
    {
        ybi->cr[j*ybi->ch+i].DiffDC = DiffDC;
    }


    void JFIF_YSSBlockImage_FDCT(JFIF_YSSBlockImage* ybi)
    {
        int i, j, k;
        float* blk;
        float tmp[64];

        for (i = 0; i < ybi->yh; i++)
        {
            for (j = 0; j < ybi->yw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Y(ybi, i, j);
                for (k = 0; k < 64; ++k) blk[k] -= 128.0f;
                fdct8(blk, tmp);
                memcpy(blk, tmp, sizeof(tmp));
            }
        }

        for (i = 0; i < ybi->ch; i++)
        {
            for (j = 0; j < ybi->cw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Cr(ybi, i, j);
                for (k = 0; k < 64; ++k) blk[k] -= 128.0f;
                fdct8(blk, tmp);
                memcpy(blk, tmp, sizeof(tmp));

                blk = JFIF_YSSBlockImage_GetBlock_Cb(ybi, i, j);
                for (k = 0; k < 64; ++k) blk[k] -= 128.0f;
                fdct8(blk, tmp);
                memcpy(blk, tmp, sizeof(tmp));
            }
        }
    }


    void JFIF_YSSBlockImage_IDCT(JFIF_YSSBlockImage* ybi)
    {
        int i, j, k;
        float* blk;
        float tmp[64];

        for (i = 0; i < ybi->yh; i++)
        {
            for (j = 0; j < ybi->yw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Y(ybi, i, j);
                idct8(blk, tmp);
                for (k = 0; k < 64; ++k) tmp[k] += 128.0f;
                memcpy(blk, tmp, sizeof(tmp));
            }
        }

        for (i = 0; i < ybi->ch; i++)
        {
            for (j = 0; j < ybi->cw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Cr(ybi, i, j);
                idct8(blk, tmp);
                for (k = 0; k < 64; ++k) tmp[k] += 128.0f;
                memcpy(blk, tmp, sizeof(tmp));

                blk = JFIF_YSSBlockImage_GetBlock_Cb(ybi, i, j);
                idct8(blk, tmp);
                for (k = 0; k < 64; ++k) tmp[k] += 128.0f;
                memcpy(blk, tmp, sizeof(tmp));
            }
        }
    }

    void JFIF_YSSBlockImage_SetQT(JFIF_YSSBlockImage* ybi, const float lqt[64], const float cqt[64])
    {
        memcpy(ybi->lqt, lqt, sizeof(float)*64u);
        memcpy(ybi->cqt, cqt, sizeof(float)*64u);
    }

    void JFIF_YSSBlockImage_FQuantize(JFIF_YSSBlockImage* ybi)
    {
        int i, j;
        float* blk;
        int* zz;

        for (i = 0; i < ybi->yh; i++)
        {
            for (j = 0; j < ybi->yw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Y(ybi, i, j);
                fQuant(blk, ybi->lqt);

                zz = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i, j);
                fZigZag(blk, zz);

            }
        }

        for (i = 0; i < ybi->ch; i++)
        {
            for (j = 0; j < ybi->cw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Cr(ybi, i, j);
                fQuant(blk, ybi->cqt);
                zz = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i, j);
                fZigZag(blk, zz);

                blk = JFIF_YSSBlockImage_GetBlock_Cb(ybi, i, j);
                fQuant(blk, ybi->cqt);
                zz = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j);
                fZigZag(blk, zz);
            }
        }
    }


    void JFIF_YSSBlockImage_IQuantize(JFIF_YSSBlockImage* ybi)
    {
        int i, j;
        float* blk;
        int* zz;

        for (i = 0; i < ybi->yh; i++)
        {
            for (j = 0; j < ybi->yw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Y(ybi, i, j);
                zz = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i, j);
                iZigZag(zz, blk);
                iQuant(blk, ybi->lqt);
            }
        }

        for (i = 0; i < ybi->ch; i++)
        {
            for (j = 0; j < ybi->cw; j++)
            {
                blk = JFIF_YSSBlockImage_GetBlock_Cr(ybi, i, j);
                zz = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i, j);
                iZigZag(zz, blk);
                iQuant(blk, ybi->cqt);

                blk = JFIF_YSSBlockImage_GetBlock_Cb(ybi, i, j);
                zz = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j);
                iZigZag(zz, blk);
                iQuant(blk, ybi->cqt);
            }
        }
    }


    /* EOF is represented by nz = -1*/
    void JFIF_YSSBlockImage_FACRLE(JFIF_YSSBlockImage* ybi)
    {
        int i, j, len;
        int* zz;
        JFIF_RLE* rle;
        JFIF_RSA* rsa;

        for (i = 0; i < ybi->yh; i++)
        {
            for (j = 0; j < ybi->yw; j++)
            {
                zz  = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i, j);
                rle = JFIF_YSSBlockImage_GetBlock_Yrle(ybi, i, j);
                rsa = JFIF_YSSBlockImage_GetBlock_Yrsa(ybi, i, j);
                len = JPEG_FRLE(zz, rle);
                JFIF_YSSBlockImage_SetYrlelen(ybi, i, j, len);
                JPEG_RLE2RSAlen(rle, rsa, len);
            }
        }

        for (i = 0; i < ybi->ch; i++)
        {
            for (j = 0; j < ybi->cw; j++)
            {
                zz  = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i, j);
                rle = JFIF_YSSBlockImage_GetBlock_Crrle(ybi, i, j);
                rsa = JFIF_YSSBlockImage_GetBlock_Crrsa(ybi, i, j);
                len = JPEG_FRLE(zz, rle);
                JFIF_YSSBlockImage_SetCrrlelen(ybi, i, j, len);
                JPEG_RLE2RSAlen(rle, rsa, len);

                /* Cb */
                zz  = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j);
                rle = JFIF_YSSBlockImage_GetBlock_Cbrle(ybi, i, j);
                rsa = JFIF_YSSBlockImage_GetBlock_Cbrsa(ybi, i, j);
                len = JPEG_FRLE(zz, rle);
                JFIF_YSSBlockImage_SetCbrlelen(ybi, i, j, len);
                JPEG_RLE2RSAlen(rle, rsa, len);
            }
        }
    }


    /* EOF is represented by nz = -1*/
    void JFIF_YSSBlockImage_IACRLE(JFIF_YSSBlockImage* ybi)
    {
        int i, j;
        int* zz;
        JFIF_RLE* rle;
        JFIF_RSA* rsa;

        for (i = 0; i < ybi->yh; i++)
        {
            for (j = 0; j < ybi->yw; j++)
            {
                zz  = JFIF_YSSBlockImage_GetBlock_Yzz (ybi, i, j);
                rle = JFIF_YSSBlockImage_GetBlock_Yrle(ybi, i, j);
                rsa = JFIF_YSSBlockImage_GetBlock_Yrsa(ybi, i, j);
                //JPEG_IRLE(rle, zz);
                JPEG_IRSA(rsa, rle, zz);
            }
        }

        for (i = 0; i < ybi->ch; i++)
        {
            for (j = 0; j < ybi->cw; j++)
            {
                zz  = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j);
                rle = JFIF_YSSBlockImage_GetBlock_Cbrle(ybi, i, j);
                rsa = JFIF_YSSBlockImage_GetBlock_Cbrsa(ybi, i, j);
                //JPEG_IRLE(rle, zz);
                JPEG_IRSA(rsa, rle, zz);

                zz  = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i, j);
                rle = JFIF_YSSBlockImage_GetBlock_Crrle(ybi, i, j);
                rsa = JFIF_YSSBlockImage_GetBlock_Crrsa(ybi, i, j);
                //JPEG_IRLE(rle, zz);
                JPEG_IRSA(rsa, rle, zz);
            }
        }
    }


    void JFIF_YSSBlockImage_DiffDC(JFIF_YSSBlockImage* ybi)
    {
        int* zz_prev;
        int* zz_curr;
        int i, j;

        for (i = 0; i < ybi->yh; ++i)
        {
            for (j = 0; j < ybi->yw; ++j)
            {
                if (i == 0 && j == 0)
                {
                    zz_curr = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i, j);
                    JFIF_YSSBlockImage_SetBlock_YDiffDC(ybi, i, j, zz_curr[0]);
                }
                else
                {
                    zz_curr = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i, j);
                    if (j > 0)
                    {
                        zz_prev = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i, j-1);
                    }
                    else
                    {
                        zz_prev = JFIF_YSSBlockImage_GetBlock_Yzz(ybi, i-1, ybi->yw-1);
                    }

                    JFIF_YSSBlockImage_SetBlock_YDiffDC(ybi, i, j, zz_curr[0]-zz_prev[0]);
                }
            }
        }


        for (i = 0; i < ybi->ch; ++i)
        {
            for (j = 0; j < ybi->cw; ++j)
            {
                if (i == 0 && j == 0)
                {
                    zz_curr = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j);
                    JFIF_YSSBlockImage_SetBlock_CbDiffDC(ybi, i, j, zz_curr[0]);
                }
                else
                {
                    zz_curr = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j);
                    if (j > 0)
                    {
                        zz_prev = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i, j-1);
                    }
                    else
                    {
                        zz_prev = JFIF_YSSBlockImage_GetBlock_Cbzz(ybi, i-1, ybi->yw-1);
                    }
                    JFIF_YSSBlockImage_SetBlock_CbDiffDC(ybi, i, j, zz_curr[0]-zz_prev[0]);


                    zz_curr = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i, j);
                    if (j > 0)
                    {
                        zz_prev = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i, j-1);
                    }
                    else
                    {
                        zz_prev = JFIF_YSSBlockImage_GetBlock_Crzz(ybi, i-1, ybi->yw-1);
                    }
                    JFIF_YSSBlockImage_SetBlock_CrDiffDC(ybi, i, j, zz_curr[0]-zz_prev[0]);
                }
            }
        }
    }




    void JFIF_YSSBlockImage_WriteAPP0(JFIF_YSSBlockImage* ybi)
    {
        static const char version[2]   = {1, 2};
        static const char identifier[]   = "JFIF";
        static const char APP0_mark[2] = {0xFF, APP0};
        static unsigned short Length = 16u;
        guobuffer* buffer = ybi->buffer;

        guobuffer_WriteChars   (buffer, APP0_mark, 2);   // APP0 mark
        guobuffer_WriteUint16  (buffer, guo_BigUint16(Length));         // APP0 segment length
        guobuffer_WriteChars   (buffer, identifier, 5);  // JFIF identifier
        guobuffer_WriteChars   (buffer, version, 2);     // version
        guobuffer_WriteUint8   (buffer, 0);                  // units
        guobuffer_WriteUint16  (buffer, guo_BigUint16(1));   // Xdensity
        guobuffer_WriteUint16  (buffer, guo_BigUint16(1));   // Ydensity
        guobuffer_WriteUint8   (buffer, 0);                  // Xthumbnail
        guobuffer_WriteUint8   (buffer, 0);                  // Ythumbnail
    }


    void JFIF_YSSBlockImage_ReadAPP0(JFIF_YSSBlockImage* ybi)
    {
        static const char EOI_mark[2] = {0xFF, EOI};
        char tmpmark[2];

        guobuffer* buffer = ybi->buffer;
        guobuffer_ReadChars(tmpmark, buffer, 2);

        if (memcmp(EOI_mark, tmpmark, 2) != 0)
        {
            fprintf(stderr, "No EOI mark!\n");
            exit(2);
        }
    }





    void JFIF_YSSBlockImage_WriteDQT(JFIF_YSSBlockImage* ybi)
    {
        static const char DQT_mark[2] = {0xFF, DQT};
        static const unsigned char QTPrecision = 0; // 8bit precision

        unsigned short Length;
        int i;
        int qt[64];


        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars  (buffer, DQT_mark, 2);     // DQT mark

        if (ybi->mode == GRAY)
        {
            Length = 67u;
        }
        else
        {
            Length = 132u;
        }
        guobuffer_WriteUint16 (buffer, guo_BigUint16(Length));

        guobuffer_WriteUint8(buffer, (unsigned char)(QTPrecision<<4)+0);
        fZigZag(ybi->lqt, qt);
        for (i = 0; i < 64; ++i)
        {
            guobuffer_WriteUint8(buffer, (unsigned char)qt[i]);
        }

        if (ybi->mode != GRAY)
        {
            guobuffer_WriteUint8(buffer, (unsigned char)(QTPrecision<<4)+1);
            fZigZag(ybi->cqt, qt);
            for (i = 0; i < 64; ++i)
            {
                guobuffer_WriteUint8(buffer, (unsigned char)qt[i]);
            }
        }
    }


    void JFIF_YSSBlockImage_WriteSOF0(JFIF_YSSBlockImage* ybi)
    {
        static const char SOF0_mark[2] = {0xFF, SOF0};
        static const unsigned char Precision = 8u;

        unsigned short Length;
        unsigned char  Sampling1 = (1<<4)+1;
        unsigned char  Sampling2 = (1<<4)+1;
        unsigned char  Sampling3 = (1<<4)+1;

        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars  (buffer, SOF0_mark, 2);                // DQT mark

        if (ybi->mode == GRAY)
        {
            Length = 11u;
            guobuffer_WriteUint16(buffer, guo_BigUint16(Length));                      // 2
        }
        else
        {
            Length = 17u;
            guobuffer_WriteUint16(buffer, guo_BigUint16(Length));
        }

        guobuffer_WriteUint8   (buffer, Precision);                  // 1
        guobuffer_WriteUint16  (buffer, guo_BigUint16((unsigned short)ybi->oyh));   // 2
        guobuffer_WriteUint16  (buffer, guo_BigUint16((unsigned short)ybi->oyw));   // 2

        if (ybi->mode == GRAY)
        {
            guobuffer_WriteUint8(buffer, 1u);    // number of components 1

            guobuffer_WriteUint8(buffer, 1u);                         // 1
            guobuffer_WriteUint8(buffer, (unsigned char)((1<<4)+1) ); // 1
            guobuffer_WriteUint8(buffer, 0u);                         // 1
        }
        else
        {
            guobuffer_WriteUint8(buffer, 3u); // number of components

            if (ybi->ssm == H1V1)
            {
                Sampling1 = (unsigned char)( (1<<4)+1 );
                Sampling2 = (unsigned char)( (1<<4)+1 );
                Sampling3 = (unsigned char)( (1<<4)+1 );
            }
            else if (ybi->ssm == H2V1)
            {
                Sampling1 = (unsigned char)( (2<<4)+1 );
                Sampling2 = (unsigned char)( (1<<4)+1 );
                Sampling3 = (unsigned char)( (1<<4)+1 );
            }
            else if (ybi->ssm == H2V2)
            {
                Sampling1 = (unsigned char)( (2<<4)+2 );
                Sampling2 = (unsigned char)( (1<<4)+1 );
                Sampling3 = (unsigned char)( (1<<4)+1 );
            }
            else
            {
                fprintf(stderr, "Sampling mode not supported!\n");
            }

            guobuffer_WriteUint8(buffer, 1u);
            guobuffer_WriteUint8(buffer, Sampling1);
            guobuffer_WriteUint8(buffer, 0u);

            guobuffer_WriteUint8(buffer, 2u);
            guobuffer_WriteUint8(buffer, Sampling2);
            guobuffer_WriteUint8(buffer, 1u);

            guobuffer_WriteUint8(buffer, 3u);
            guobuffer_WriteUint8(buffer, Sampling3);
            guobuffer_WriteUint8(buffer, 1u);
        }
    }


    void JFIF_YSSBlockImage_WriteDEBUGDHT(JFIF_YSSBlockImage* ybi)
    {
        static const char DHT_mark[2] = {0xFF, DHT};
        static const char Lum = 0;
        static const char Chr = 1;
        static const char DC  = 0;
        static const char AC  = 1;

        unsigned short Length;
        int i;

        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars  (buffer, DHT_mark, 2);              // DQT mark

        if (ybi->mode == GRAY)
        {
            Length = 2+(1+16+255)*2;
        }
        else
        {
            Length = 2+(1+16+255)*4;
        }
        guobuffer_WriteUint16 (buffer, guo_BigUint16(Length));

        guobuffer_WriteUint8(buffer, (unsigned char)(DC<<4)+Lum);
        for (i = 0; i < 16; ++i)
        {
            if (i == 7)
            {
                guobuffer_WriteUint8(buffer, 255u);
            }
            else
            {
                guobuffer_WriteUint8(buffer, 0u);
            }
        }

        for (i = 0; i < 255; ++i)
        {
            guobuffer_WriteUint8(buffer, (unsigned char)i);
        }

        guobuffer_WriteUint8(buffer, (unsigned char)(AC<<4)+Lum);
        for (i = 0; i < 16; ++i)
        {
            if (i == 7)
            {
                guobuffer_WriteUint8(buffer, 255u);
            }
            else
            {
                guobuffer_WriteUint8(buffer, 0u);
            }
        }

        for (i = 0; i < 255; ++i)
        {
            guobuffer_WriteUint8(buffer, (unsigned char)i);
        }


        if (ybi->mode != GRAY)
        {
            guobuffer_WriteUint8(buffer, (unsigned char)(DC<<4)+Chr);
            for (i = 0; i < 16; ++i)
            {
                if (i == 7)
                {
                    guobuffer_WriteUint8(buffer, 255u);
                }
                else
                {
                    guobuffer_WriteUint8(buffer, 0u);
                }
            }

            for (i = 0; i < 255; ++i)
            {
                guobuffer_WriteUint8(buffer, (unsigned char)i);
            }

            guobuffer_WriteUint8  (buffer, (unsigned char)(AC<<4)+Chr);
            for (i = 0; i < 16; ++i)
            {
                if (i == 7)
                {
                    guobuffer_WriteUint8(buffer, 255u);
                }
                else
                {
                    guobuffer_WriteUint8(buffer, 0u);
                }
            }

            for (i = 0; i < 255; ++i)
            {
                guobuffer_WriteUint8(buffer, (unsigned char)i);
            }
        }
    }


    void JFIF_YSSBlockImage_WriteSOS(JFIF_YSSBlockImage* ybi)
    {
        static const char SOS_mark[2] = {0xFF, SOS};

        unsigned short Length;
        unsigned char  numComp;
        int i, j, k, len;
        unsigned char bitlen;
        JFIF_RSA* rsa;

        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars  (buffer, SOS_mark, 2);              // DQT mark

        if (ybi->mode == GRAY)
        {
            Length  = 8u;
            numComp = 1u;
        }
        else
        {
            Length = 12u;
            numComp = 3u;
        }

        guobuffer_WriteUint16 (buffer, guo_BigUint16(Length));
        guobuffer_WriteUint8  (buffer, numComp);

        guobuffer_WriteUint8  (buffer, 1u);
        guobuffer_WriteUint8  (buffer, 0u);
        if (ybi->mode != GRAY)
        {
            guobuffer_WriteUint8(buffer, 2u);
            guobuffer_WriteUint8(buffer, (1<<4)+1);
            guobuffer_WriteUint8(buffer, 3u);
            guobuffer_WriteUint8(buffer, (1<<4)+1);
        }

        guobuffer_WriteUint8(buffer, 0u);
        guobuffer_WriteUint8(buffer, 63u);
        guobuffer_WriteUint8(buffer, 0u);


        for (i = 0; i < ybi->yh; ++i)
        {
            for (j = 0; j < ybi->yw; ++j)
            {

                if (i == 10 && j == 10)
                {
                    guobuffer_WriteUint8(buffer, 0x08);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);

                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                }
                else
                {
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                    guobuffer_WriteUint8(buffer, 0x00);
                }

                /*
                          rsa = JFIF_YSSBlockImage_GetBlock_Yrsa(ybi, i, j);
                          len = JFIF_YSSBlockImage_GetYrlelen(ybi, i, j);
                          for (k = 0; k < len; ++k)
                          {
                               guobuffer_WriteBitsFromInt(buffer, (unsigned int)(rsa[k].rs),  8u);
                               bitlen = (unsigned char)(0x0011 & rsa[k].rs);
                               guobuffer_WriteBitsFromInt(buffer, (unsigned int)(rsa[k].add), bitlen);
                          }
                          */
            }
        }

#if 0
        for (i = 0; i < ybi->ch; ++i)
        {
            for (j = 0; j < ybi->cw; ++j)
            {
                guobuffer_WriteUint8(buffer, 0x08);
                guobuffer_WriteUint8(buffer, 0x00);
                guobuffer_WriteUint8(buffer, 0x08);
                guobuffer_WriteUint8(buffer, 0x00);
                guobuffer_WriteUint8(buffer, 0x00);

                guobuffer_WriteUint8(buffer, 0x08);
                guobuffer_WriteUint8(buffer, 0x00);
                guobuffer_WriteUint8(buffer, 0x08);
                guobuffer_WriteUint8(buffer, 0x00);
                guobuffer_WriteUint8(buffer, 0x00);

                /*
                      rsa = JFIF_YSSBlockImage_GetBlock_Cbrsa(ybi, i, j);
                      len = JFIF_YSSBlockImage_GetCbrlelen(ybi, i, j);
                      for (k = 0; k < len; ++k)
                      {
                           guobuffer_WriteBitsFromInt(buffer, (unsigned int)(rsa[k].rs),  8u);
                           bitlen = (unsigned char)(0x0011 & rsa[k].rs);
                           guobuffer_WriteBitsFromInt(buffer, (unsigned int)(rsa[k].add), bitlen);
                      }

                      rsa = JFIF_YSSBlockImage_GetBlock_Crrsa(ybi, i, j);
                      len = JFIF_YSSBlockImage_GetCrrlelen(ybi, i, j);
                      for (k = 0; k < len; ++k)
                      {
                           guobuffer_WriteBitsFromInt(buffer, (unsigned int)(rsa[k].rs),  8u);
                           bitlen = (unsigned char)(0x0011 & rsa[k].rs);
                           guobuffer_WriteBitsFromInt(buffer, (unsigned int)(rsa[k].add), bitlen);
                      }
                        */
            }
        }
#endif
    }


    void JFIF_YSSBlockImage_WriteCOM(JFIF_YSSBlockImage* ybi)
    {
        static const char COM_mark[2] = {0xFF, COM};
        static const char comments[] = "created by guojpeg, guoxiaoyong@guoxiaoyong.net";

        unsigned short Length;

        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars  (buffer, COM_mark, 2);
        Length = strlen(comments)+2;
        guobuffer_WriteUint16 (buffer, guo_BigUint16(Length));
        guobuffer_WriteChars  (buffer, comments, strlen(comments));
    }


    void JFIF_YSSBlockImage_WriteSOI(JFIF_YSSBlockImage* ybi)
    {
        static const char SOI_mark[2] = {0xFF, SOI};
        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars  (buffer, SOI_mark, 2);
    }




    void JFIF_YSSBlockImage_WriteEOI(JFIF_YSSBlockImage* ybi)
    {
        static const char EOI_mark[2] = {0xFF, EOI};
        guobuffer* buffer = ybi->buffer;
        guobuffer_WriteChars (buffer, EOI_mark, 2);
    }



    void JFIF_YSSBlockImage_WriteFile(JFIF_YSSBlockImage* ybi, const char* filename)
    {
        guobuffer* buffer = ybi->buffer;

        JFIF_YSSBlockImage_WriteSOI(ybi);
        JFIF_YSSBlockImage_WriteAPP0(ybi);
        JFIF_YSSBlockImage_WriteDQT(ybi);
        JFIF_YSSBlockImage_WriteCOM(ybi);
        JFIF_YSSBlockImage_WriteSOF0(ybi);
        JFIF_YSSBlockImage_WriteDEBUGDHT(ybi);
        JFIF_YSSBlockImage_WriteSOS(ybi);
        JFIF_YSSBlockImage_WriteEOI(ybi);

        guobuffer_WriteFile(buffer, filename);
    }


    void JFIF_YSSBlockImage_FindSegs(JFIF_YSSBlockImage* ybi)
    {
        size_t i;
        guobuffer* buffer = ybi->buffer; 
        memset(&ybi->segptrs, 0, sizeof(ybi->segptrs));

        for (i = 0; i < buffer->size-1; ++i)
        {
            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == SOI)
            {
                ybi->segptrs.soi = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == EOI)
            {
                ybi->segptrs.eoi = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == SOF0)
            {
                ybi->segptrs.sof0 = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == APP0)
            {
                ybi->segptrs.app0 = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == DQT)
            {
                ybi->segptrs.dqt = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == DHT)
            {
                ybi->segptrs.dht = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == SOS)
            {
                ybi->segptrs.dht = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == COM)
            {
                ybi->segptrs.dht = (buffer->mem+i);
            }

            if ( buffer->mem[i] == 0xFF && buffer->mem[i+1] == DRI)
            {
                ybi->segptrs.dht = (buffer->mem+i);
            }
        }

        assert(ybi->segptrs.soi);
        assert(ybi->segptrs.eoi);
        assert(ybi->segptrs.sof0);
        assert(ybi->segptrs.dqt);
        assert(ybi->segptrs.dht);
        assert(ybi->segptrs.app0);

        assert(ybi->segptrs.dri == NULL);
    }




    void JFIF_YSSBlockImage_ReadFile(JFIF_YSSBlockImage* ybi, const char* filename)
    {
        guobuffer* buffer = ybi->buffer; 
        guobuffer_ReadFile(buffer, filename);
        JFIF_YSSBlockImage_FindSegs(ybi);
    }




    /*#####################################################################
     *
     * define huffman tree node class
     *
     * ##################################################################*/




    JFIF_HTnode* JFIF_HTnode_Create()
    {
        JFIF_HTnode* htn;

        htn = (JFIF_HTnode*)malloc( sizeof(JFIF_HTnode) );
        htn->l      = NULL;
        htn->r      = NULL;
        htn->isleaf = 0;
        htn->symbol = 0;

        return htn;
    }


    void JFIF_HTnode_DestroyTree(JFIF_HTnode* htn)
    {
        if(htn == NULL) return;

        JFIF_HTnode_DestroyTree(htn->l);
        JFIF_HTnode_DestroyTree(htn->r);
        free(htn);
    }


    JFIF_HTnode* JFIF_HTnode_CreateTree(unsigned char* numbits, unsigned char* symbols)
    {
        JFIF_HTnode* htn;
        int  i, len;
        unsigned int  offset;
        unsigned char LeafCount;

        htn = JFIF_HTnode_Create();

        for (i = 15; i >= 0; i--)
        {
            if (numbits[i] != 0) break;
        }

        len = i;

        offset = 0;
        for(i = 0; i <= len; i++)
        {
            LeafCount = 0;
            JFIF_HTnode_Set(htn, 0, i, &LeafCount, numbits[i], symbols, offset);
            offset += numbits[i];
        }

        return htn;
    }


    /*A recursive function to fill in the values at a particular level of the Huffman Tree*/
    void JFIF_HTnode_Set(JFIF_HTnode    *node,
                         unsigned char  CurrLev,
                         unsigned char  TargLev,
                         unsigned char  *LeafCount,
                         unsigned char  TotalLeaf,
                         unsigned char  *Symbols,
                         int offset)
    {
        if (CurrLev == TargLev)
        {
            if (*LeafCount < TotalLeaf)
            {
                node->l = JFIF_HTnode_Create();
                node->l->isleaf = 1;
                node->l->symbol = Symbols[offset + *LeafCount];
                (*LeafCount)++;
            }
            else
            {
                if (CurrLev < 15) node->l = JFIF_HTnode_Create();
            }

            if(*LeafCount < TotalLeaf)
            {
                node->r = JFIF_HTnode_Create();
                node->r->isleaf = 1;
                node->r->symbol = Symbols[offset + *LeafCount];
                (*LeafCount)++;
            }
            else
            {
                if (CurrLev < 15) node->r = JFIF_HTnode_Create();
            }
        }
        else
        {
            if (!node->l->isleaf)
            {
                JFIF_HTnode_Set(node->l, CurrLev+1, TargLev, LeafCount, TotalLeaf, Symbols, offset);
            }

            if (!node->r->isleaf)
            {
                JFIF_HTnode_Set(node->r, CurrLev+1, TargLev, LeafCount, TotalLeaf, Symbols, offset);
            }
        }
    }



    void JFIF_HTnode_Transverse(JFIF_HTnode* node, JFIF_EncTable* enctbl, int level, unsigned char bits[16])
    {
        assert(node != NULL);

        if (node->l != NULL)
        {
            assert(node->r != NULL);
            if (node->l->isleaf || node->r->isleaf)
            {
                if (node->l->isleaf)
                {
                    bits[level] = 0;
                    memcpy(enctbl->entries[ node->l->symbol ].bits, bits, sizeof(unsigned char)*16);
                    enctbl->entries[ node->l->symbol ].numbits = level+1;
                }
                else
                {
                    bits[level] = 0;
                    JFIF_HTnode_Transverse(node->l, enctbl, level+1, bits);
                }

                if (node->r->isleaf)
                {
                    bits[level] = 1;
                    memcpy(enctbl->entries[ node->r->symbol ].bits, bits, sizeof(unsigned char)*16);
                    enctbl->entries[ node->r->symbol ].numbits = level+1;
                }
                else
                {
                    bits[level] = 1;
                    JFIF_HTnode_Transverse(node->r, enctbl, level+1, bits);

                }
            }
            else
            {
                bits[level] = 0;
                JFIF_HTnode_Transverse(node->l, enctbl, level+1, bits);

                bits[level] = 1;
                JFIF_HTnode_Transverse(node->r, enctbl, level+1, bits);
            }
        }
    }


    JFIF_EncTable* JFIF_EncTable_Create(unsigned char* numbits, unsigned char* symbols)
    {
        JFIF_HTnode* htn;
        JFIF_EncTable* enctbl;
        unsigned char bits[16];
        int i;

        enctbl = (JFIF_EncTable*)malloc( sizeof(JFIF_EncTable) );

        if (numbits == NULL || symbols == NULL)
        {
            for (i = 0; i < 256; ++i)
            {
                to8bits(i, enctbl->entries[i].bits);
                enctbl->entries[i].numbits = 8;
            }
        }
        else
        {
            htn = JFIF_HTnode_CreateTree(numbits, symbols);
            JFIF_HTnode_Transverse(htn, enctbl, 0, bits);
        }

        return enctbl;
    }


    void JFIF_EncTable_Destroy(JFIF_EncTable* enctbl)
    {
        free(enctbl);
    }

    void JFIF_EncTable_Print(JFIF_EncTable* enctbl)
    {
        int i;
        char str0[256] = "symbol = %02hhX, bits = % ";
        char str1[256] = "16";
        char str2[256] = "s, numbits = %hhu\n";
        char *bits;
        unsigned char nb_max;

        nb_max = enctbl->entries[0].numbits;
        for (i = 1; i < 256; ++i)
        {
            if ( nb_max < enctbl->entries[i].numbits )
            {
                nb_max = enctbl->entries[i].numbits;
            }
        }

        sprintf(str1, "%hhu", nb_max);
        strcat(str0, str1);
        strcat(str0, str2);

        for (i = 0; i < 256; ++i)
        {
            if ( enctbl->entries[i].numbits > 0 )
            {
                bits = bits2str(enctbl->entries[i].bits, enctbl->entries[i].numbits);
                printf(str0, i, bits, enctbl->entries[i].numbits);
            }
        }
    }




#if 0
    void HTree_decode(JFIF_HTnode* DC, JFIF_HTnode* AC, BYTE* qTable, Coefficients* coefficients, int index, FILE* file, BYTE* b, BYTE* bitPos)
    {
        WORD* uncompressed = coefficients[index].values;
        HTree_T node = dcTable;
        BYTE bit, value;
        BOOL decodingCategory = FALSE;
        WORD temp;
        int i, j;


        for(i = 0; i < 64; i++)
        {
            uncompressed[i] = 0;
        }

        i = 0;
        while(i < 64 && !feof(file))
        {
            if(*bitPos > 7)
            {
                *bitPos = 0;
                *b = getByteF(file);
            }

            if(decodingCategory)
            {
                if(value == EOB)
                {
                    if(i > 0)
                    {
                        break;
                    }
                    else
                    {
                        uncompressed[0] = 0;
                    }
                }
                else if(i == 0)
                {   /*Decoding DC coefficient*/
                    temp = getFromCategory(value, b, bitPos, file);
                    uncompressed[0] = temp;
                }
                else
                {   /*Decoding AC coefficient*/
                    /*Fill in previous zeroes (this is the run-length portion)*/
                    for(j = 0; j < getHighNibble(value); j++)
                    {
                        if(i > 63)
                        {
                            fprintf(stderr, "Error: Greater than 64 DCT coefficients encountered\n");
                            break;
                        }
                        uncompressed[i] = 0;
                        i++;
                    }
                    if(i < 64)
                    {
                        temp = getFromCategory(getLowNibble(value), b, bitPos, file);
                        uncompressed[i] = temp;
                    }
                }

                i++;
                decodingCategory = FALSE;
                node = acTable;
            }
            else
            {
                if(node->leafNode)
                {
                    value = node->value;
                    decodingCategory = TRUE;
                }
                else
                {
                    bit = getBit(*b, *bitPos);
                    *bitPos = *bitPos + 1;
                    if(bit == 0)
                        node = node->left;
                    else
                        node = node->right;
                }
            }


            if(feof(file))
            {
                fprintf(stderr, "Warning: end of file reached before finished decoding data section\n");
            }
        }

        /*Now dequantize the values*/
        for(i=0; i<64; i++)
        {
            uncompressed[i] = uncompressed[i] * qTable[i];
            /*printf("%i ", (int)((short int)uncompressed[i]));*/
        }
    }

#endif





#ifdef __cplusplus
}
#endif

