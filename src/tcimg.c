/*###################################################################
 *
 * tcimg.h tcimg.c - tcimg stands for True Color Image. This library
 *                   provides a set of routines to manipulating 24bit
 *                   Bitmap files (widely used Microsoft V3.0). This
 *                   library should work on both small endian machine
 *                   and big endian machine.
 *
 * Copyright (c) 2002-2009, Xiaoyong Guo
 *
 *
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *
 *
 * mailto: guo.xiaoyong@gmail.com
 *         http://www.wordeazy.com
 *
 *
 *##################################################################*/


/*#################################################################
 *
 * Created : Aug, 12, 2002
 *
 * Modified: Mar, 01, 2006
 * Modified: Apr, 15, 2009
 *
 * Modified: Jan, 12, 2009
 *        1. add support for big endianness
 *        2. fix a bug of setting proper
 *           bytewidth (also called pitch)
 *
 *
 * TODO: add more basic drawing functions
 *
 *#################################################################*/


/* standard C library headers */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <string.h>
#include <setjmp.h>
#include <limits.h>
#include <float.h>


#include <tcimg.h>

static jmp_buf FileWriteExceptionEnv;
static jmp_buf FileReadExceptionEnv;

#define TCIMG_LineTo_v3         TCIMG_LineTo
#define TCIMG_TriangleFill_v2   TCIMG_TriangleFill

#ifdef __cplusplus
extern "C" {
#endif


    /*##############################################################
     *
     * IsLittleEndian: test whether the ENDIANNESS of the
     *                 machine currently working on.
     *
     * ############################################################*/

    int IsLittleEndian()
    {
        unsigned int   number = 0x12345678;
        unsigned char *bytes  = (unsigned char*)&number;

        if ( bytes[0] == (unsigned char)0x78 )
        {
            return 1;
        }
        else if (bytes[0] == (unsigned char)0x12)
        {
            return 0;
        }
        else
        {
            fprintf(stderr, "IsLittleEndian: error occurred!!\n");
            return -1;
        }
    }




    /*##############################################################
     *
     * SEWriteINT16ToFile and SEWriteINT32ToFile:
     *             Write 2-byte integer and 4-byte integer to
     *             the disk file according to their endianness.
     *             Both types of integers are written in small
     *             endianness (that where the prefix SE came from)
     *
     * ############################################################*/

    void SEWriteINT16ToFile( FILE* const fp,
                             const char endianness,
                             const void* const data)
    {
        const char* bytes = (const char*)data;
        const int ErrorID = (int)('w');
        const INT32 SIZ  = (INT32)sizeof(UINT16);
        register INT32 i;

        if ( endianness == 's' || endianness == 'S' )
        {
            for (i = 0; i < SIZ; ++i)
            {
                fputc(bytes[i], fp);
                if ( ferror(fp) )
                {
                    longjmp(FileWriteExceptionEnv, ErrorID);
                }
            }
        }
        else if ( endianness == 'b' || endianness == 'B' )
        {
            for (i = SIZ-1; i >= 0; --i)
            {
                fputc(bytes[i], fp);
                if ( ferror(fp) )
                {
                    longjmp(FileWriteExceptionEnv, ErrorID);
                }
            }
        }
        else
        {
            /* should never happend */
            fprintf(stderr, "SEWriteINT16ToFile Error!!\n");
        }
    }


    void SEWriteINT32ToFile( FILE* const fp,
                             const char endianness,
                             const void* const data)
    {
        const char* bytes = (const char*)data;
        const int ErrorID = (int)('w');
        const INT32 SIZ  = (INT32)sizeof(INT32);
        register int i;

        if ( endianness == 's' || endianness == 'S' )
        {
            for (i = 0; i < SIZ; ++i)
            {
                fputc(bytes[i], fp);
                if ( ferror(fp) )
                {
                    longjmp(FileWriteExceptionEnv, ErrorID);
                }
            }
        }
        else if ( endianness == 'b' || endianness == 'B' )
        {
            for (i = SIZ-1; i >= 0; --i)
            {
                fputc(bytes[i], fp);
                if ( ferror(fp) )
                {
                    longjmp(FileWriteExceptionEnv, ErrorID);
                }
            }
        }
        else
        {
            /* should never happend */
            fprintf(stderr, "SEWriteINT32ToFile Error!!\n");
        }
    }




    /*##############################################################
     *
     * SEReadINT16FromFile and SEReadINT32FromFile:
     *             Read 2-byte integer and 4-byte integer stored
     *             in little endian order.
     *
     * ############################################################*/


    void SEReadINT16FromFile( FILE* const fp,
                              const char endianness,
                              void* const data)
    {
        char* const bytes = (char* const)data;
        const int ErrorID = (int)('r');
        const INT32 SIZ  = (INT32)sizeof(UINT16);
        register int i;

        if ( endianness == 's' || endianness == 'S' )
        {
            for (i = 0; i < SIZ; ++i)
            {
                bytes[i] = (char)fgetc(fp);
                if ( ferror(fp) )
                {
                    longjmp(FileReadExceptionEnv, ErrorID);
                }
            }
        }
        else if ( endianness == 'b' || endianness == 'B' )
        {
            for (i = SIZ-1; i >= 0; --i)
            {
                bytes[i] = (char)fgetc(fp);
                if ( ferror(fp) )
                {
                    longjmp(FileReadExceptionEnv, ErrorID);
                }
            }
        }
        else
        {
            /* should never happend */
            fprintf(stderr, "SEWriteINT16ToFile Error!!\n");
        }
    }


    void SEReadINT32FromFile( FILE* const fp,
                              const char endianness,
                              void* const data)
    {
        char* const bytes = (char* const)data;
        const int ErrorID = (int)('r');
        const INT32 SIZ  = (INT32)sizeof(INT32);
        register int i;

        if ( endianness == 's' || endianness == 'S' )
        {
            for (i = 0; i < SIZ; ++i)
            {
                bytes[i] = (char)fgetc(fp);
                if ( ferror(fp) )
                {
                    longjmp(FileReadExceptionEnv, ErrorID);
                }
            }
        }
        else if ( endianness == 'b' || endianness == 'B' )
        {
            for (i = SIZ-1; i >= 0; --i)
            {
                bytes[i] = (char)fgetc(fp);
                if ( ferror(fp) )
                {
                    longjmp(FileReadExceptionEnv, ErrorID);
                }
            }
        }
        else
        {
            /* should never happend */
            fprintf(stderr, "SEWriteINT16ToFile Error!!\n");
        }
    }



    /*#############################################################
     *
     * TCIMG_Init
     *
     * return value:  0: successful
     *                1: file open error
     *                2: file writting error with fputc
     *                3: file writting error with fwrite
     *
     * ############################################################*/

    int TCIMG_Init(TrueColorImage* img, INT32 Width, INT32 Height)
    {
        /* Bytes Per Pixel */
        const INT32 BPP = 3;

        assert(Width  >= 0);
        assert(Height >= 0);

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        img->ImgWidth  = Width;
        img->ImgHeight = Height;
        img->ImgByteWidth = img->ImgWidth * BPP;
        img->DepthFlag = 0;

        /* follow the rule which states the
         * Each new .BMP line must start on an even 32 bit boundary!
        * i.e., img->ImgByteWidth*8 % 32 == 0 */

        if ( img->ImgByteWidth % 4 != 0 )
        {
            /* img->ImgByteWidth = (img->ImgByteWidth / 4 + 1) * 4 ;*/
            img->ImgByteWidth = ( (img->ImgByteWidth >> 2) + 1) << 2;
        }


        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        img->ImgTotalPixels = img->ImgWidth * img->ImgHeight;
        /* in case there's an overflow */
        assert (img->ImgTotalPixels > img->ImgWidth );

        img->ImgTotalBytes = img->ImgHeight * img->ImgByteWidth;
        /* in case there's an overflow */
        assert (img->ImgTotalBytes > img->ImgHeight);

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


        img->ImgData = (UINT8*)malloc((size_t)img->ImgTotalBytes);
        if (img->ImgData == NULL)
        {
            return 1;
        }

        img->PL0 = PList_Create(img->ImgTotalBytes);
        img->PL1 = PList_Create(img->ImgTotalBytes);
        img->PL2 = PList_Create(img->ImgTotalBytes);
        img->PL3 = PList_Create(img->ImgTotalBytes);

        img->SET = EdgeTable_Create(64);
        img->AET = EdgeTable_Create(64);

        assert(img->PL0 != NULL);
        assert(img->PL1 != NULL);
        assert(img->PL2 != NULL);
        assert(img->PL3 != NULL);
        assert(img->SET != NULL);
        assert(img->AET != NULL);

        img->DepthBuffer = (float*)malloc( sizeof(float)*img->ImgHeight*img->ImgWidth);

        /* successful */
        return 0;
    }

    /*FIXME: use ReadFromFile then Release will result in crash */
    void TCIMG_Release(TrueColorImage* img)
    {
        free(img->ImgData);
        free(img->DepthBuffer);

        PList_Destroy(img->PL0);
        PList_Destroy(img->PL1);
        PList_Destroy(img->PL2);
        PList_Destroy(img->PL3);
        EdgeTable_Destroy(img->SET);
        EdgeTable_Destroy(img->AET);

        img->ImgData = NULL;

        /* dimension */
        img->ImgWidth  = 0;
        img->ImgHeight = 0;

        /* these data can be computed from all the above
         * values, stored here to be ready to use */
        img->ImgByteWidth   = 0;
        img->ImgTotalPixels = 0;
        img->ImgTotalBytes  = 0;
    }


    TrueColorImage* TCIMG_Create(INT32 Width, INT32 Height)
    {
        TrueColorImage* img;
        img = (TrueColorImage*)malloc( sizeof(TrueColorImage) );
        TCIMG_Init(img, Width, Height);

        return img;
    }

    void TCIMG_Destroy(TrueColorImage* img)
    {
        TCIMG_Release(img);
        free(img);
        img = NULL;
    }


    /*#############################################################
     *
     * TCIMG_IsInit: check whether a TrueColorImage structure has
     *               has been initialized.
     *
     * return value: 0, not initialized
     *               1, the structure has been initialized
     *
     * Note: this function check whether all the fields
     *       in the structure is consistent and reasonable.
     *       If the return value is 0, then this data structure
     *       is definited not initialized. If the return value
     *       is 1, then this structure is almost surely
     *       initialized. Although theoretically it is
     *       possible that all the fields in the structure
     *       are not been initialized but appears consistent
     *       and reasonable, but the chance for this to happen
     *       is very very small.
     *
     * ##########################################################*/


    int TCIMG_IsInit(TrueColorImage* const img)
    {
        const INT32 BPP = 3;

        if (img == NULL)
        {
            return 0;
        }

        if (img->ImgData == NULL)
        {
            return 0;
        }

        if (img->ImgWidth <=0)
        {
            return 0;
        }

        if (img->ImgHeight <= 0)
        {
            return 0;
        }

        /* follow the rule which states the
         * Each new .BMP line must start on an even 32 bit boundary! */
        if ( img->ImgByteWidth % 4 == 0 )
        {
            if (img->ImgByteWidth != img->ImgWidth * BPP)
            {
                return 0;
            }
        }
        else
        {
            /* img->ImgByteWidth =(ImgWidth*3 / 4 + 1) * 4 ;*/
            if ( img->ImgByteWidth
                    !=
                    ( (((img->ImgWidth * BPP) >> 2) + 1) << 2 ) )
            {
                return 0;
            }
        }


        if ( img->ImgTotalPixels
                != img->ImgWidth * img->ImgHeight)
        {
            return 0;
        }

        if ( img->ImgTotalBytes
                != img->ImgHeight * img->ImgByteWidth)
        {
            return 0;
        }

        return 1;
    }



    /*#############################################################
     *
     * TCIMG_SaveToFile
     *
     * return value:  0: successful
     *                1: file open error
     *                2: file writting error with fputc
     *                3: file writting error with fwrite
     *
     * ############################################################*/

    /* save and read from disk files */
    int TCIMG_SaveToFile(TrueColorImage* img, char filename[])
    {
        FILE* fp;
        int   info;
        MSDIB_FileHeader    bmFH;
        V3MSDIB_InfoHeader  bmIH;

        const UINT32 FHsiz = 14;
        const UINT32 IHsiz = 40;
        const UINT32 HeaderSiz = FHsiz+IHsiz;

        int ExceptionID;

        char endianness;

        if ( IsLittleEndian() )
        {
            endianness = 'S';
        }
        else
        {
            endianness = 'B';
        }


        /* open a file to write the image data */
        fp = fopen(filename, "wb");

        if ( fp == NULL )
        {
            /* openfile error */
            return 1;
        }

        ExceptionID = setjmp(FileWriteExceptionEnv);

        if (ExceptionID != 0)
        {
            info = fclose (fp);
            assert(info == 0);
            /* file writting error */
            return 2;
        }




        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /*++ BEGINNING OF SETTING THE FILE HEADER PARAMETERS */

        /* fill the field of the FileHeader and InfoHeader
         * with right numbers */
        bmFH.bfType = (UINT16)(('M' << 8) + 'B');

        bmFH.bfSize
        = HeaderSiz +
          (UINT32)(img->ImgHeight) * (UINT32)(img->ImgByteWidth);

        bmFH.bfReserved = 0x00000000;

        /* if there's palette, what is the OffBits ? */
        bmFH.bfOffBits  = HeaderSiz;

        /*++ END of SETTING THE FILE HEADER PARAMETERS */





        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /*++ BEGINNING OF SETTING THE INFO HEADER PARAMETERS */

        bmIH.biSize = IHsiz;

        bmIH.biWidth  = img->ImgWidth;
        bmIH.biHeight = img->ImgHeight;

        /* always 1 */
        bmIH.biPlanes = 1;

        /* should be 24 for true color image */
        bmIH.biBitCount = (UINT16)24;

        /* no compression */
        bmIH.biCompression = 0;

        bmIH.biSizeImage
        = (UINT32)(img->ImgHeight) * (UINT32)(img->ImgByteWidth);

        /* not useful for true color image */
        bmIH.biXPelsPerMeter  = 0;
        bmIH.biYPelsPerMeter  = 0;
        bmIH.biClrUsed        = 0;
        bmIH.biClrImportant   = 0;

        /*++ END OF SETTING THE INFO HEADER PARAMETERS */





        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* Write File Header */

        SEWriteINT16ToFile(fp, endianness, &bmFH.bfType    );
        SEWriteINT32ToFile(fp, endianness, &bmFH.bfSize    );
        SEWriteINT32ToFile(fp, endianness, &bmFH.bfReserved);
        SEWriteINT32ToFile(fp, endianness, &bmFH.bfOffBits );

        /*+++++++++++++++++++++++++++++++++++++++++++++++++*/
        /* Write Info Header */
        SEWriteINT32ToFile(fp, endianness, &bmIH.biSize  );
        SEWriteINT32ToFile(fp, endianness, &bmIH.biWidth );
        SEWriteINT32ToFile(fp, endianness, &bmIH.biHeight);

        SEWriteINT16ToFile(fp, endianness, &bmIH.biPlanes  );
        SEWriteINT16ToFile(fp, endianness, &bmIH.biBitCount);

        SEWriteINT32ToFile(fp, endianness, &bmIH.biCompression  );
        SEWriteINT32ToFile(fp, endianness, &bmIH.biSizeImage    );
        SEWriteINT32ToFile(fp, endianness, &bmIH.biXPelsPerMeter);
        SEWriteINT32ToFile(fp, endianness, &bmIH.biYPelsPerMeter);
        SEWriteINT32ToFile(fp, endianness, &bmIH.biClrUsed      );
        SEWriteINT32ToFile(fp, endianness, &bmIH.biClrImportant );



        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* Write bitmap data, endianness is not a concern here */

        if ( fwrite(img->ImgData, bmIH.biSizeImage, 1, fp)
                != (size_t)bmIH.biSizeImage )
        {
            info = fclose(fp);
            assert(info == 0);
            return 3;
        }

        info = fclose(fp);
        assert(info == 0);
        return 0;
    }



    int TCIMG_ReadFromFile(TrueColorImage* img, char filename[])
    {
        FILE*  fp = NULL;
        char   head = 0;
        int    info = 1;

        const UINT32 FHsiz = 14;
        const UINT32 IHsiz = 40;
        const UINT32 HeaderSiz = FHsiz+IHsiz;
        const UINT32 BPP   = 3; /* Bytes Per Pixel */

        /* important field and their position in the file */
        UINT32 bfOffBits = 0;
        INT32  bfOffBits_offset = 10;

        INT32 biWidth = 0;
        INT32 biWidth_offset = 18;

        INT32 biHeight = 0;
        INT32 biHeight_offset = 22;

        UINT16 biBitCount = 0;
        INT32  biBitCount_offset = 28;

        UINT32 biCompression = 0;
        INT32  biCompression_offset = 30;

        int ExceptionID = 9;

        char endianness;

        if ( IsLittleEndian() )
        {
            endianness = 'S';
        }
        else
        {
            endianness = 'B';
        }



        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* open a BMP file */
        fp = fopen(filename, "rb");
        if (fp == NULL)
        {
            info = fclose(fp);
            assert(info == 0);
            /* file open error */
            return 1;
        }



        /* Exception handling */
        ExceptionID = setjmp(FileWriteExceptionEnv);

        if (ExceptionID != 0)
        {
            info = fclose (fp);
            assert(info == 0);
            /* file writting error */
            return 2;
        }

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* check the magic number */
        /* read the first byte */
        head = (char)fgetc(fp);
        if (ferror(fp))
        {
            info = fclose(fp);
            assert(info == 0);
            /* file reading error */
            return 3;
        }

        if ( head != 'B' )
        {
            info = fclose(fp);
            assert(info == 0);
            /* not-a-BMP file error */
            return 4;
        }

        /* read the second byte */
        head = (char)fgetc(fp);
        if (ferror(fp))
        {
            info = fclose(fp);
            assert(info == 0);
            /* file reading error */
            return 5;
        }
        if (head != 'M')
        {
            info = fclose(fp);
            assert(info == 0);
            /* not-a-BMP file error */
            return 6;
        }


        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* only several field are important */

        info = fseek(fp, (long int)bfOffBits_offset, SEEK_SET);
        if (info == 0)
        {
            SEReadINT32FromFile(fp, endianness, &bfOffBits);
        }
        else
        {
            info = fclose(fp);
            assert(info == 0);
            return 7;
        }

        info = fseek(fp, (long int)biBitCount_offset, SEEK_SET);
        if (info == 0)
        {
            SEReadINT16FromFile(fp, endianness, &biBitCount);
        }
        else
        {
            info = fclose(fp);
            assert(info == 0);
            return 8;
        }

        info = fseek(fp, (long int)biCompression_offset, SEEK_SET);
        if (info == 0)
        {
            SEReadINT32FromFile(fp, endianness, &biCompression);
        }
        else
        {
            info = fclose(fp);
            assert(info == 0);
            return 9;
        }



        if (bfOffBits != HeaderSiz)
        {
            info = fclose(fp);
            assert(info == 0);
            return 10;
        }

        if ((UINT32)biBitCount != BPP*8)
        {
            info = fclose(fp);
            assert(info == 0);
            return 11;
        }

        if (biCompression != 0)
        {
            info = fclose(fp);
            assert(info == 0);
            return 12;
        }


        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* get the width and height of the image */
        info = fseek(fp, (long int)biWidth_offset, SEEK_SET);
        if (info == 0)
        {
            SEReadINT32FromFile(fp, endianness, &biWidth);
        }
        else
        {
            info = fclose(fp);
            assert(info == 0);
            return 13;
        }

        info = fseek(fp, (long int)biHeight_offset, SEEK_SET);
        if (info == 0)
        {
            SEReadINT32FromFile(fp, endianness, &biHeight);
        }
        else
        {
            info = fclose(fp);
            assert(info == 0);
            return 14;
        }

        assert(biWidth  >= 0);
        assert(biHeight >= 0);


        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        img->ImgWidth  = biWidth;
        img->ImgHeight = biHeight;

        /* follow the rule which states the
         * Each new .BMP line must start on an even 32 bit boundary! */
        if ( biWidth % 4 == 0 )
        {
            img->ImgByteWidth = BPP * biWidth;
        }
        else
        {
            /* img->ImgRealWidth = (biWidth * 3 / 4 + 1) * 4 ; */
            img->ImgByteWidth = (((biWidth * 3) >> 2) + 1) << 2;
        }

        img->ImgTotalPixels = img->ImgWidth * img->ImgHeight;
        /* in case there's an overflow */
        assert (img->ImgTotalPixels > img->ImgWidth );

        img->ImgTotalBytes = img->ImgHeight * img->ImgByteWidth;
        /* in case there's an overflow */
        assert (img->ImgTotalBytes > img->ImgHeight);

        img->ImgData = (UINT8*)malloc((size_t)img->ImgTotalBytes);

        if (img->ImgData == NULL)
        {
            info = fclose(fp);
            assert(info == 0);
            return 15;
        }


        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        /* goto the image data block */
        info = fseek(fp, (long int)HeaderSiz, SEEK_SET);
        if (info == 0)
        {
            int siz = fread(img->ImgData, 1, (size_t)img->ImgTotalBytes, fp);
            assert(siz == img->ImgTotalBytes);

            if (ferror(fp))
            {
                info = fclose(fp);
                assert(info == 0);
                return 16;
            }
        }
        else
        {
            info = fclose(fp);
            assert(info == 0);
            return 17;
        }


        info = fclose(fp);
        assert(info == 0);
        /* successful */
        return 0;
    }


    /*###############################################################
     *
     * TCIMG_SetPixelClr: set the point (x, y) to be the
     *                    RGB color (R, G, B)
     *
    ###############################################################*/

    int TCIMG_SetPixelClr(TrueColorImage* img, INT32 x, INT32 y, UINT8 R, UINT8 G, UINT8 B)
    {
        /* Byte Per Pixel */
        const INT32 BPP = 3;
        UINT8* ptr = NULL;
        assert(img != NULL);

        /* check range */
        if ( x < 0 || x >= img->ImgWidth )
        {
            return 1;
        }

        if ( y < 0 || y >= img->ImgHeight)
        {
            return 2;
        }

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

        ptr = img->ImgData + (y * img->ImgByteWidth) + (x * BPP);

        assert(ptr != NULL);

        if (img->DepthFlag == 1)
        {
            if (B >= ptr[0] && G >= ptr[1] && R >= ptr[2])
            {
                ptr[0] = B;
                ptr[1] = G;
                ptr[2] = R;
            }
        }
        else
        {
            ptr[0] = B;
            ptr[1] = G;
            ptr[2] = R;
        }

        /* successful */
        return 0;
    }


    int TCIMG_SetPixelColor(TrueColorImage* img, Point2D* p, RGBColor* color)
    {
        /* Byte Per Pixel */
        const INT32 BPP = 3;
        UINT8* ptr = NULL;
        assert(img != NULL);

        /* check range */
        if ( p->x < 0 || p->x >= img->ImgWidth )
        {
            return 1;
        }

        if ( p->y < 0 || p->y >= img->ImgHeight)
        {
            return 2;
        }

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

        ptr = img->ImgData + (p->y * img->ImgByteWidth) + (p->x * BPP);

        assert(ptr != NULL);

        if (img->DepthFlag == 1)
        {
            if (RGBColor_GetB(color) >= ptr[0] && RGBColor_GetG(color) >= ptr[1] && RGBColor_GetR(color) >= ptr[2])
            {
                ptr[0] = RGBColor_GetB(color);
                ptr[1] = RGBColor_GetG(color);
                ptr[2] = RGBColor_GetR(color);
            }
        }
        else
        {
            ptr[0] = RGBColor_GetB(color);
            ptr[1] = RGBColor_GetG(color);
            ptr[2] = RGBColor_GetR(color);
        }

        return 0;
    }


    /**
     * TCIMG_Depth_Functions
     * */
    void TCIMG_Depth_Clear(TrueColorImage* img)
    {
        int i;
        for (i = 0; i < img->ImgTotalPixels; ++i)
        {
            img->DepthBuffer[i] = FLT_MAX;
        }
    }

    float* TCIMG_Depth_GetPixelDepth(TrueColorImage* img, Point2D* p)
    {
        if (!( (p->x >= 0) && (p->y >= 0) && (p->x < img->ImgWidth) && (p->y < img->ImgHeight) ))
        {
            return NULL;
        }

        return &( img->DepthBuffer[p->y*img->ImgWidth+p->x] );
    }


    int TCIMG_Depth_SetPixelColor(TrueColorImage* img, Point2D* p, RGBColor* color, float depth)
    {
        /* Byte Per Pixel */
        static const INT32 BPP = 3;
        UINT8* ptr = NULL;
        assert(img != NULL);

        float* old_depth = TCIMG_Depth_GetPixelDepth(img, p);
        if (old_depth == NULL)
        {
            return 4;
        }

        if (*old_depth <= depth)
        {
            return 3;
        }

        /* check range */
        if ( p->x < 0 || p->x >= img->ImgWidth )
        {
            return 1;
        }

        if ( p->y < 0 || p->y >= img->ImgHeight)
        {
            return 2;
        }

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

        ptr = img->ImgData + (p->y * img->ImgByteWidth) + (p->x * BPP);
        assert(ptr != NULL);
        ptr[0] = RGBColor_GetB(color);
        ptr[1] = RGBColor_GetG(color);
        ptr[2] = RGBColor_GetR(color);
        *old_depth = depth;

        return 0;
    }


#if 0
    int TCIMG_Depth_TriangleAETLineColor(TrueColorImage* img, RGBColor* color)
    {
        int j;
        int left  = (int)round( (img->AET->recorde[0]).x );
        int right = (int)round( (img->AET->recorde[1]).x );
        Point2D p;

        //fprintf(stderr, "img->AET->len = %d\n", img->AET->len);
        assert(img->AET->len == 2);

        if (left > right)
        {
            j = left;
            left  = right;
            right = j;
        }

        p.y = img->AET->y;
        for (j = left; j <= right; ++j)
        {
            p.x = j;
            TCIMG_Depth_SetPixelColor(img, p, color, depth);
        }

        return 0;
    }
#endif



    int TCIMG_SetPixel( TrueColorImage* const img, INT32 x, INT32 y )
    {
        TCIMG_SetPixelClr(img, x, y, img->RPenClr, img->GPenClr, img->BPenClr);

        return 0;
    }


    /*###############################################################
     *
     * TCIMG_SetPixelClr: set the point (x, y) to be the
     *                    RGB color (R, G, B)
     *
    ###############################################################*/

    int TCIMG_GetPixelClr(TrueColorImage* img, INT32 x, INT32 y, UINT8 *R, UINT8 *G, UINT8 *B )
    {
        /* Byte Per Pixel */
        const INT32 BPP = 3;

        UINT8* ptr = NULL;

        assert(img != NULL);

        /* check range */
        if ( x < 0 || x >= img->ImgWidth )
        {
            return 1;
        }

        if ( y < 0 || y >= img->ImgHeight)
        {
            return 2;
        }

        /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

        ptr = img->ImgData + (y * img->ImgByteWidth) + (x * BPP);

        assert(ptr != NULL);

        *B = ptr[0];
        *G = ptr[1];
        *R = ptr[2];

        /* successful */
        return 0;
    }


    int TCIMG_FillClr(TrueColorImage* img, UINT8 R, UINT8 G, UINT8 B )
    {
        register INT32 i, j;

        if ( !TCIMG_IsInit(img) )
        {
            return 1;
        }


        for (i = 0; i < img->ImgWidth; ++i)
        {
            for (j = 0; j < img->ImgHeight; ++j)
            {
                TCIMG_SetPixelClr(img, i, j, R, G, B);
            }
        }

        return 0;
    }


    int TCIMG_FillColor(TrueColorImage* img, RGBColor* c)
    {
        return TCIMG_FillClr(img, RGBColor_GetR(c), RGBColor_GetG(c), RGBColor_GetB(c));
    }


    int TCIMG_Clear(TrueColorImage* img)
    {
        if ( !TCIMG_IsInit(img) )
        {
            return 1;
        }

        memset(img->ImgData, 0, img->ImgTotalBytes);

        return 0;
    }

    int TCIMG_MoveTo(TrueColorImage* img, INT32 x, INT32 y)
    {
        img->XcurrPos = x;
        img->YcurrPos = y;

        return 0;
    }

    int TCIMG_SetPenClr(TrueColorImage* img, UINT8 R, UINT8 G, UINT8 B)
    {
        img->RPenClr = R;
        img->GPenClr = G;
        img->BPenClr = B;

        return 0;
    }


    /*#######################################################################*/
    /* line drawing procedures */
    /*#######################################################################*/
    void TCIMG_VerticalLine(TrueColorImage* img, INT32 x, INT32 y1, INT32 y2)
    {
        int i, Ymin, Ymax;

        if (y1 < y2)
        {
            Ymin = y1;
            Ymax = y2;
        }
        else
        {
            Ymin = y2;
            Ymax = y1;
        }

        for (i = Ymin; i <= Ymax; i++)
        {
            TCIMG_SetPixel(img, x, i);
        }
    }


    void TCIMG_HorizontalLine(TrueColorImage* img, INT32 x1, INT32 x2, INT32 y)
    {
        int i, Xmin, Xmax;

        if (x1 < x2)
        {
            Xmin = x1;
            Xmax = x2;
        }
        else
        {
            Xmin = x2;
            Xmax = x1;
        }

        for (i = Xmin; i <= Xmax; i++)
        {
            TCIMG_SetPixel(img, i, y);
        }
    }



    void TCIMG_DiagonalLine(TrueColorImage* img, INT32 x1, INT32 y1, INT32 x2, INT32 y2)
    {
        int i, Xmin, Xmax, ys, ye, sign;
        int len = abs(x1-x2)+1;
        assert( abs(x1-x2) == abs(y1-y2) );

        if (x1 < x2)
        {
            Xmin = x1;
            Xmax = x2;
            ys = y1;
            ye = y2;
        }
        else
        {
            Xmin = x2;
            Xmax = x1;
            ys = y2;
            ye = y1;
        }

        sign = (ys < ye)? 1: -1;

        for (i = 0; i < len; i++)
        {
            TCIMG_SetPixel(img, Xmin+i, ys+i*sign);
        }
    }



    void TCIMG_BresenhamLineXP(TrueColorImage* img, INT32 x0, INT32 y0, INT32 xEnd, INT32 yEnd)
    {
        int dx = abs(x0 - xEnd);
        int dy = abs(y0 - yEnd);
        int p = 2*dy - dx;
        int twoDy = 2*dy;
        int twoDyMinusDx= 2*(dy-dx);
        int x, y;

        assert( (float)(y0-yEnd) / (float)(x0-xEnd) < 1.0 );
        assert( (float)(y0-yEnd) / (float)(x0-xEnd) > 0.0 );

        if (x0 > xEnd)
        {
            x = xEnd;
            y = yEnd;
            xEnd = x0;
        }
        else
        {
            x = x0;
            y = y0;
        }

        TCIMG_SetPixel(img, x, y);

        while (x < xEnd)
        {
            x++;
            if (p < 0)
            {
                p += twoDy;
            }
            else
            {
                y++;
                p += twoDyMinusDx;
            }
            TCIMG_SetPixel(img, x, y);
        }
    }



    void TCIMG_BresenhamLineXM(TrueColorImage* img, INT32 x0, INT32 y0, INT32 xEnd, INT32 yEnd)
    {
        int dx =  abs(x0 - xEnd);
        int dy = -abs(y0 - yEnd);
        int p  = -(2*dy+dx);
        int twoDy = 2*dy;
        int twoDxMinusDy= 2*(-dx-dy);
        int x, y;

#if 0
        if ( (float)(y0-yEnd) * (float)(x0-xEnd) >  0.0 )
        {
            fprintf(stderr, "y0 = %d, yEnd = %d, x0 = %d, xEnd = %d\n", y0, yEnd, x0, xEnd);
        }
#endif

        assert( (float)(y0-yEnd) / (float)(x0-xEnd) <  0.0 );
        assert( (float)(y0-yEnd) / (float)(x0-xEnd) > -1.0 );

        if (x0 > xEnd)
        {
            x = xEnd;
            y = yEnd;
            xEnd = x0;
        }
        else
        {
            x = x0;
            y = y0;
        }

        TCIMG_SetPixel(img, x, y);

        while (x < xEnd)
        {
            x++;
            if (p < 0)
            {
                p -= twoDy;
            }
            else
            {
                y--;
                p += twoDxMinusDy;
            }

            TCIMG_SetPixel(img, x, y);
        }

    }


    void TCIMG_BresenhamLineYP(TrueColorImage* img, INT32 x0, INT32 y0, INT32 xEnd, INT32 yEnd)
    {
        int dx = abs(x0 - xEnd);
        int dy = abs(y0 - yEnd);
        int p = 2*dx - dy;
        int twoDx = 2*dx;
        int twoDxMinusDy= 2*(dx-dy);
        int x, y;

        if (y0 > yEnd)
        {
            y = yEnd;
            x = xEnd;
            yEnd = y0;
        }
        else
        {
            y = y0;
            x = x0;
        }

        TCIMG_SetPixel(img, x, y);

        while (y < yEnd)
        {
            y++;
            if (p < 0)
            {
                p += twoDx;
            }
            else
            {
                x++;
                p += twoDxMinusDy;
            }
            TCIMG_SetPixel(img, x, y);
        }
    }


    void TCIMG_BresenhamLineYM(TrueColorImage* img, INT32 x0, INT32 y0, INT32 xEnd, INT32 yEnd)
    {
        int dx = -abs(x0 - xEnd);
        int dy =  abs(y0 - yEnd);
        int p  = -(2*dx+dy);
        int twoDx = 2*dx;
        int twoDyMinusDx= 2*(-dy-dx);
        int x, y;

        if (y0 > yEnd)
        {
            y = yEnd;
            x = xEnd;
            yEnd = y0;
        }
        else
        {
            y = y0;
            x = x0;
        }

        TCIMG_SetPixel(img, x, y);

        while (y < yEnd)
        {
            y++;
            if (p < 0)
            {
                p -= twoDx;
            }
            else
            {
                x--;
                p += twoDyMinusDx;
            }
            TCIMG_SetPixel(img, x, y);
        }
    }


    int TCIMG_LineTo_v1(TrueColorImage* img, INT32 x, INT32 y)
    {
        /* draw one point */
        if (x == img->XcurrPos && y == img->YcurrPos)
        {
            TCIMG_SetPixel(img, x, y);
            return 0;
        }

        /* draw verticle line */
        if(x == img->XcurrPos )
        {
            TCIMG_VerticalLine(img, x, img->YcurrPos, y);
            TCIMG_MoveTo(img, x, y);
            return 0;
        }


        /* draw horizontal line */
        if(y == img->YcurrPos)
        {
            TCIMG_HorizontalLine(img, img->XcurrPos, x, y);
            TCIMG_MoveTo(img, x, y);
            return 0;
        }


        if ( abs(x-img->XcurrPos) == abs(y-img->YcurrPos) )
        {
            TCIMG_DiagonalLine(img, img->XcurrPos, img->YcurrPos, x, y);
            TCIMG_MoveTo(img, x, y);
            return 0;
        }

        int DeltaX = (x - img->XcurrPos);
        int DeltaY = (y - img->YcurrPos);

        if ( abs(DeltaX) > abs(DeltaY) )
        {
            if (DeltaX*DeltaY < 0)
            {
                TCIMG_BresenhamLineXM(img, img->XcurrPos, img->YcurrPos, x, y);
                //fprintf(stderr, "x=%d, y=%d, xp=%d, yp=%d\n", x, y, img->XcurrPos, img->YcurrPos);
            }
            else
            {
                TCIMG_BresenhamLineXP(img, img->XcurrPos, img->YcurrPos, x, y);
            }

            TCIMG_MoveTo(img, x, y);
            return 0;
        }


        if ( abs(DeltaX) < abs(DeltaY) )
        {
            if (DeltaX*DeltaY < 0)
            {
                TCIMG_BresenhamLineYM(img, img->XcurrPos, img->YcurrPos, x, y);
            }
            else
            {
                TCIMG_BresenhamLineYP(img, img->XcurrPos, img->YcurrPos, x, y);
            }

            TCIMG_MoveTo(img, x, y);
            return 0;
        }


        return 1;
    }


    int TCIMG_LineTo_v2(TrueColorImage* img, INT32 x, INT32 y)
    {
        int i;
        int len;
        int step;
        double slope;

        /* draw one point */
        if (x == img->XcurrPos && y == img->YcurrPos)
        {
            TCIMG_SetPixel(img, x, y);
            return 0;
        }

        /* draw verticle line */
        if(x == img->XcurrPos )
        {
            len  = y - img->YcurrPos;
            step = len > 0 ? 1: -1;
            len  = abs(len);

            for (i = 0; i <= len; ++i)
            {
                TCIMG_SetPixel(img, x, img->YcurrPos + i*step);
            }

            TCIMG_MoveTo(img, x, y);
            return 0;
        }


        /* draw horizontal line */
        if(y == img->YcurrPos )
        {
            len  = x - img->XcurrPos;
            step = len > 0 ? 1: -1;
            len  = abs(len);

            for (i = 0; i <= len; ++i)
            {
                TCIMG_SetPixel(img, img->XcurrPos + i*step, y);
            }

            TCIMG_MoveTo(img, x, y);

            return 0;
        }


#define ROUND(x) ((int)(x+0.5))

        slope = (double)(x - img->XcurrPos) / (double)(y - img->YcurrPos);

        if ( fabs(slope) > 1.0 )
        {
            slope = 1.0 / slope;

            if( x > img->XcurrPos )
            {
                for(i = img->XcurrPos; i <= x; i++)
                    TCIMG_SetPixel(img, i, ROUND((double)img->YcurrPos + slope*(i - img->XcurrPos)) );
            }
            else
            {
                for(i = x; i <= img->XcurrPos; i++)
                    TCIMG_SetPixel(img, i, ROUND((double)y + slope*(i-x)) );
            }
        }
        else
        {
            if( y > img->YcurrPos )
            {
                for(i = img->YcurrPos; i <= y; i++)
                    TCIMG_SetPixel(img, ROUND((double)img->XcurrPos + slope*(i-img->YcurrPos)), i);
            }
            else
            {
                for(i = y; i <= img->YcurrPos; i++)
                    TCIMG_SetPixel(img, ROUND((double)x + slope*(i-y)), i);
            }
        }
#undef ROUND

        TCIMG_MoveTo(img, x, y);

        return 0;
    }


    int TCIMG_LineTo_v3(TrueColorImage* img, INT32 x, INT32 y)
    {
        int i;
        Point2D p1, p2;

        p1.x = img->XcurrPos;
        p1.y = img->YcurrPos;

        p2.x = x;
        p2.y = y;

        PList_Line(img->PL0, &p1, &p2);

        for (i = 0; i < img->PL0->len; ++i)
        {
            TCIMG_SetPixel(img, (img->PL0->p[i]).x, (img->PL0->p[i]).y);
        }


        TCIMG_MoveTo(img, x, y);
        return 0;
    }





    int TCIMG_Line(TrueColorImage* img, INT32 x[], INT32 y[], int len)
    {
        int i;
        assert(len >= 0);
        if (len == 0)
        {
            return 0;
        }
        else if(len == 1)
        {
            TCIMG_MoveTo(img, x[0], y[0]);
            TCIMG_SetPixelClr(img, x[0], y[0], img->RPenClr, img->GPenClr, img->BPenClr);
            return 0;
        }

        TCIMG_MoveTo(img, x[0], y[0]);
        for (i = 1; i < len; ++i)
        {
            TCIMG_LineTo(img, x[i], y[i]);
        }

        return 0;
    }


    int TCIMG_Polygon(TrueColorImage* img, INT32 x[], INT32 y[], int len)
    {
        TCIMG_Line(img, x, y, len);
        TCIMG_LineTo(img, x[0], y[0]);

        return 0;
    }


    int TCIMG_SetDepthFlag (TrueColorImage* img, int flag)
    {
        return img->DepthFlag = flag;
    }


    int TCIMG_LineToGradient(TrueColorImage* img, Point2D* p1, Point2D* p2, RGBColor* c1, RGBColor* c2)
    {
        int i;
        RGBColor c;
        double f;

        PList_Line(img->PL0, p1, p2);

        f = 1.0/(double)img->PL0->len;

        for (i = 0; i < img->PL0->len; ++i)
        {
            assert(fabs(i*f) <= 1.00001);
            RGBColor_LinearGradient(c1, c2, 1.0-i*f, &c);
            TCIMG_SetPixelColor(img, &(img->PL0->p[i]), &c);
        }

        return 0;
    }


    int TCIMG_SetPenColor(TrueColorImage* img, RGBColor* color)
    {
        return TCIMG_SetPenClr(img, RGBColor_GetR(color), RGBColor_GetG(color), RGBColor_GetB(color));
    }




    int TCIMG_TriangleFill_v1(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3)
    {
        int xmin, xmax, ymin, ymax;
        int i, j;

        p1->z = 1;
        p2->z = 1;
        p3->z = 1;

        xmin = p1->x < p2->x ? p1->x:p2->x;
        xmin = xmin < p3->x ? xmin:p3->x;

        xmax = p1->x > p2->x ? p1->x:p2->x;
        xmax = xmax > p3->x ? xmax:p3->x;

        ymin = p1->y < p2->y ? p1->y:p2->y;
        ymin = ymin < p3->y ? ymin:p3->y;

        ymax = p1->y > p2->y ? p1->y:p2->y;
        ymax = ymax > p3->y ? ymax:p3->y;

        Point2D p;
        p.z = 1;

        for (j = ymin; j <= ymax; ++j)
        {
            for (i = xmin; i <= xmax; ++i)
            {
                /* if (i, j) in the triangle */
                p.x = i;
                p.y = j;
                if ( Point2D_InTriangle(&p, p1, p2, p3) )
                {
                    TCIMG_SetPixel(img, i, j);
                }
            }
        }

        TCIMG_Triangle(img, p1, p2, p3);

        return 0;
    }




    int TCIMG_TriangleFill_v2(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3)
    {
        Point2D p[3];
        int info;
        p[0] = *p1;
        p[1] = *p2;
        p[2] = *p3;

        /* construct sorted edge table */
        Point2D_SortTriangleY(p, p+1, p+2);

        EdgeTable_Add(img->SET, p+1, p+2);

        EdgeTable_Add(img->AET, p+0, p+1);
        EdgeTable_Add(img->AET, p+0, p+2);

        img->AET->y = p[0].y;
        while (img->AET->len > 1)
        {
            TCIMG_TriangleAETLine(img);
            info = TCIMG_TriangleUpdateAET(img);
            if (info == 2) break;
        }

        img->SET->len = 0;
        img->AET->len = 0;
        return 0;
    }


    int TCIMG_TriangleAETLine(TrueColorImage* img)
    {
        int j;
        int left  = (int)round( (img->AET->recorde[0]).x );
        int right = (int)round( (img->AET->recorde[1]).x );

        //fprintf(stderr, "img->AET->len = %d\n", img->AET->len);
        assert(img->AET->len == 2);

        if (left > right)
        {
            j = left;
            left  = right;
            right = j;
        }

        for (j = left; j <= right; ++j)
        {
            TCIMG_SetPixel(img, j, img->AET->y);
        }

        return 0;
    }




    int TCIMG_TriangleUpdateAET(TrueColorImage* img)
    {
        int i;
        int count = 0;
        for (i = 0; i < 2; ++i)
        {
            if (img->AET->y == img->AET->recorde[i].ymax)
            {
                EdgeTable_Remove(img->AET, i);
                EdgeTable_AddRecorde(img->AET, &img->SET->recorde[0]);
                count++;
            }
        }
        if (count == 2) return count;

        img->AET->y++;
        img->AET->recorde[0].x += img->AET->recorde[0].InverseSlope;
        img->AET->recorde[1].x += img->AET->recorde[1].InverseSlope;

        return 0;
    }




    int TCIMG_TriangleFillColor(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3, RGBColor* color)
    {
        int xmin, xmax, ymin, ymax;
        int i, j;

        p1->z = 1;
        p2->z = 1;
        p3->z = 1;

        xmin = p1->x < p2->x ? p1->x:p2->x;
        xmin = xmin < p3->x ? xmin:p3->x;

        xmax = p1->x > p2->x ? p1->x:p2->x;
        xmax = xmax > p3->x ? xmax:p3->x;

        ymin = p1->y < p2->y ? p1->y:p2->y;
        ymin = ymin < p3->y ? ymin:p3->y;

        ymax = p1->y > p2->y ? p1->y:p2->y;
        ymax = ymax > p3->y ? ymax:p3->y;

        Point2D p;
        p.z = 1;

        for (j = ymin; j <= ymax; ++j)
        {
            for (i = xmin; i <= xmax; ++i)
            {
                /* if (i, j) in the triangle */
                p.x = i;
                p.y = j;
                if ( Point2D_InTriangle(&p, p1, p2, p3) )
                {
                    TCIMG_SetPixelColor(img, &p, color);
                }
            }
        }

        return 0;
    }



    int TCIMG_TriangleFillGradient(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3, RGBColor* c1, RGBColor* c2, RGBColor* c3)
    {
        int xmin, xmax, ymin, ymax;
        int i, j;
        RGBColor cx1, cx2, cx3, color1, color2;
        double d, d1, d2, d3;

        xmin = p1->x < p2->x ? p1->x:p2->x;
        xmin = xmin < p3->x ? xmin:p3->x;

        xmax = p1->x > p2->x ? p1->x:p2->x;
        xmax = xmax > p3->x ? xmax:p3->x;

        ymin = p1->y < p2->y ? p1->y:p2->y;
        ymin = ymin < p3->y ? ymin:p3->y;

        ymax = p1->y > p2->y ? p1->y:p2->y;
        ymax = ymax > p3->y ? ymax:p3->y;

        Point2D p;
        p.z = 1;
        p1->z = 1;
        p2->z = 1;
        p3->z = 1;

        for (j = ymin; j <= ymax; ++j)
        {
            for (i = xmin; i <= xmax; ++i)
            {
                /* if (i, j) in the triangle */
                p.x = i;
                p.y = j;
                if ( Point2D_InTriangle(&p, p1, p2, p3) )
                {
                    d1 = 1.0 / (sqrt((double)Point2D_DistanceSquare(&p, p1))+0.0001);
                    d2 = 1.0 / (sqrt((double)Point2D_DistanceSquare(&p, p2))+0.0001);
                    d3 = 1.0 / (sqrt((double)Point2D_DistanceSquare(&p, p3))+0.0001);
                    d  = d1+d2+d3;

                    if (!(fabs(d)>0.0))
                    {
                        fprintf(stderr, "d1=%f, d2=%f, d3=%f\n", d1, d2, d3);
                    }
                    assert(fabs(d)>0.0);

                    d1 = d1/d;
                    d2 = d2/d;
                    d3 = d3/d;

                    RGBColor_Scale(c1, d1, &cx1);
                    RGBColor_Scale(c2, d2, &cx2);
                    RGBColor_Scale(c3, d3, &cx3);
                    RGBColor_Add(&cx1, &cx2, &color1);
                    RGBColor_Add(&color1, &cx3, &color2);
                    TCIMG_SetPixelColor(img, &p, &color2);
                }
            }
        }


        return 0;
    }


    int TCIMG_Triangle(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3)
    {
        TCIMG_MoveTo(img, p1->x, p1->y);
        TCIMG_LineTo(img, p2->x, p2->y);
        TCIMG_LineTo(img, p3->x, p3->y);
        TCIMG_LineTo(img, p1->x, p1->y);

        return 0;
    }

    /**##########################################################
     *
     * RGBColor
     *
     * #########################################################*/

    RGBColor* RGBColor_Create(UINT8 r, UINT8 g, UINT8 b)
    {
        RGBColor* color = (RGBColor*)malloc(sizeof(RGBColor));
        color->b = b;
        color->g = g;
        color->r = r;
        color->a = 0;

        return color;
    }


    void RGBColor_Destroy(RGBColor* color)
    {
        free(color);
    }

    void RGBColor_Set(RGBColor* color, UINT8 r, UINT8 g, UINT8 b)
    {
        color->b = b;
        color->g = g;
        color->r = r;
        color->a = 0;
    }

    UINT8 RGBColor_GetR(RGBColor* color)
    {
        return color->r;
    }

    UINT8 RGBColor_GetG(RGBColor* color)
    {
        return color->g;
    }

    UINT8 RGBColor_GetB(RGBColor* color)
    {
        return color->b;
    }

    void RGBColor_Scale(RGBColor* color, double scale, RGBColor* xcolor)
    {
        if (!(fabs(scale) <= 1.00001)) fprintf(stderr, "fabs(scale) = %f\n", fabs(scale));
        assert( !isnan(scale) );
        assert( fabs(scale) <= 1.00001);
        assert( scale >= -0.00001);

        xcolor->b = (UINT8)round((double)color->b * scale);
        xcolor->g = (UINT8)round((double)color->g * scale);
        xcolor->r = (UINT8)round((double)color->r * scale);
    }

    void RGBColor_Add(RGBColor* c1, RGBColor* c2, RGBColor* c)
    {
        c->r = c1->r + c2->r;
        c->g = c1->g + c2->g;
        c->b = c1->b + c2->b;
    }


    void RGBColor_LinearGradient(RGBColor* c1, RGBColor* c2, double f, RGBColor* c)
    {
        RGBColor a1, a2;

        if (!(fabs(f) <= 1.00001))
        {
            fprintf(stderr, "%f\n", f);
        }

        assert(fabs(f) <= 1.00001);

        RGBColor_Scale(c1, f, &a1);
        RGBColor_Scale(c2, 1.0-f, &a2);
        RGBColor_Add(&a1, &a2, c);
    }

    void RGBColor_BaryInterpolation(RGBColor* c1, RGBColor* c2, RGBColor* c3, float a1, float a2, float a3, RGBColor* c)
    {
        RGBColor cx1, cx2, cx3;

        assert(!isnan(a1));
        assert(!isnan(a2));
        assert(!isnan(a3));

        RGBColor_Scale(c1, a1, &cx1);
        RGBColor_Scale(c2, a2, &cx2);
        RGBColor_Scale(c3, a3, &cx3);
        RGBColor_Add(&cx1, &cx2, c);
        RGBColor_Add(&cx3, c, c);
    }


    void RGBColor_Random(RGBColor* c)
    {
        RGBColor_Set(c, rand()%256, rand()%256, rand()%256);
    }


    void RGBColor_VSet(RGBColor* r1, RGBColor* r2)
    {
        memcpy(r1, r2, sizeof(RGBColor));
    }

    /**##############################################3
     * Point2D
     **/
    Point2D* Point2D_Create(int x, int y)
    {
        Point2D* p;
        p = (Point2D*)malloc(sizeof(Point2D));
        if (p != NULL)
        {
            p->x = x;
            p->y = y;
            p->z = 1;
        }

        return p;
    }


    void Point2D_Destroy(Point2D* p)
    {
        free(p);
    }

    void Point2D_Minus(Point2D* p1, Point2D* p2, Point2D* p)
    {
        p->x = p1->x - p2->x;
        p->y = p1->y - p2->y;
    }


    void Point2D_Abs(Point2D* p)
    {
        p->x = abs(p->x);
        p->y = abs(p->y);
    }


    int Point2D_PointsOnLine(Point2D* p1, Point2D* p2, Point2D* p)
    {
        Point2D p3;
        double x, y;
        int i, xlen, ylen, len;

        Point2D_Minus(p2, p1, &p3);
        xlen = abs(p3.x);
        ylen = abs(p3.y);

        if (xlen == ylen && xlen == 0)
        {
            p[0] = *p1;
            return 1;
        }

        if ( xlen > ylen )
        {
            x = (double)p3.x / (double)xlen;
            y = (double)p3.y / (double)xlen;
            len = xlen+1;
        }
        else
        {
            x = (double)p3.x / (double)ylen;
            y = (double)p3.y / (double)ylen;
            len = ylen+1;
        }

        p[0] = *p1;

        for (i = 1; i < len; ++i)
        {
            p[i].x = (int)round( (double)p[0].x + x*(double)i );
            p[i].y = (int)round( (double)p[0].y + y*(double)i );
        }

        return len;
    }


    void Point2D_CrossProduct(Point2D *a, Point2D *b, Point2D *c)
    {
        c->x = a->y*b->z - b->y*a->z;
        c->y = a->z*b->x - b->z*a->x;
        c->z = a->x*b->y - b->x*a->y;
    }


    double Point2D_EInnerProduct(Point2D* a, Point2D* b)
    {
        return a->x*b->x + a->y*b->y;
    }

    double Point2D_HInnerProduct(Point2D* a, Point2D* b)
    {
        return a->x*b->x + a->y*b->y + a->z*b->z;
    }


    int Point2D_InTriangle(Point2D* p, Point2D *a, Point2D *b, Point2D *c)
    {
        Point2D p1, p2, p3;
        double i1, i2, i3;

        Point2D_CrossProduct(a, b, &p1);
        Point2D_CrossProduct(b, c, &p2);
        Point2D_CrossProduct(c, a, &p3);

        i1 = Point2D_HInnerProduct(p, &p1);
        i2 = Point2D_HInnerProduct(p, &p2);
        i3 = Point2D_HInnerProduct(p, &p3);

        if ( (i1 >=0 && i2 >=0 && i3 >= 0) ||  (i1 <=0 && i2 <=0 && i3 <= 0) )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    int Point2D_NormSquare(Point2D* p1)
    {
        return p1->x*p1->x + p1->y*p1->y;
    }


    double Point2D_Distance(Point2D* p1, Point2D* p2)
    {
        Point2D p;
        Point2D_Minus(p1, p2, &p);
        return sqrt( (double)Point2D_NormSquare(&p) );
    }


    int Point2D_DistanceSquare(Point2D* p1, Point2D* p2)
    {
        Point2D p;
        Point2D_Minus(p1, p2, &p);
        return Point2D_NormSquare(&p);
    }


    void Point2D_Print(Point2D* p)
    {
        fprintf(stderr, "[%d, %d, %d]\n", p->x, p->y, p->z);
    }

    void Point2D_Set(Point2D* p, int x, int y)
    {
        p->x = x;
        p->y = y;
        p->z = 1;
    }

    void Point2D_VSet(Point2D* p1, Point2D* p2)
    {
        memcpy(p1, p2, sizeof(Point2D));
    }

    void Point2D_Swap(Point2D* p1, Point2D* p2)
    {
        Point2D tmp;
        Point2D_VSet(&tmp, p2);
        Point2D_VSet(p2, p1);
        Point2D_VSet(p1, &tmp);
    }


    void Point2D_SortTriangleX(Point2D* p1, Point2D* p2, Point2D* p3)
    {
        if (p1->x > p2->x) Point2D_Swap(p1, p2);

        if (p3->x < p1->x)
        {
            Point2D_Swap(p2, p3);
            Point2D_Swap(p1, p2);
        }
        else if (p3->x < p2->x)
        {
            Point2D_Swap(p2, p3);
        }
    }


    void Point2D_SortTriangleY(Point2D* p1, Point2D* p2, Point2D* p3)
    {
        if (p1->y > p2->y) Point2D_Swap(p1, p2);

        if (p3->y < p1->y)
        {
            Point2D_Swap(p2, p3);
            Point2D_Swap(p1, p2);
        }
        else if (p3->y < p2->y)
        {
            Point2D_Swap(p2, p3);
        }
    }



    /**
     * PList member functions
     */

    PList* PList_Create(int size)
    {
        PList* plist;
        plist = (PList*)malloc(sizeof(PList));
        plist->len = 0;
        plist->size = size;
        plist->p = (Point2D*)malloc(sizeof(Point2D)*size);

        return plist;
    }

    void PList_Destroy(PList* plist)
    {
        free(plist->p);
        free(plist);
    }

    void PList_Line(PList* plist, Point2D* p1, Point2D* p2)
    {
        int len = Point2D_PointsOnLine(p1, p2, plist->p);
        plist->len = len;
    }


    /**
     * EdgeTable member functions
     * */
    EdgeTable* EdgeTable_Create(int size)
    {
        EdgeTable* et;
        et = (EdgeTable*)malloc( sizeof(EdgeTable) );
        if (!et) return NULL;

        assert(et != NULL);

        et->recorde = (ScanLineEdgeRecorde*)malloc( sizeof(ScanLineEdgeRecorde)*size );
        et->size    = size;
        et->len     = 0;

        return et;
    }

    void EdgeTable_Destroy(EdgeTable* et)
    {
        free(et->recorde);
        free(et);
    }

    int EdgeTable_Add(EdgeTable* et, Point2D* p1, Point2D* p2)
    {
        et->recorde[et->len].x    = (float)( (p1->y < p2->y)? p1->x:p2->x );
        et->recorde[et->len].ymax = (p1->y > p2->y)? p1->y:p2->y;

        et->recorde[et->len].InverseSlope = (float)(p2->x - p1->x) / (float)(p2->y - p1->y);
        et->len++;

        return 0;
    }


    int EdgeTable_AddRecorde(EdgeTable* et, ScanLineEdgeRecorde* recorde)
    {
        et->recorde[et->len] = *recorde;
        et->len++;

        //fprintf(stderr, "AddRecorde: et->len = %d\n", et->len);
        return 0;
    }


    int EdgeTable_Remove(EdgeTable* et, int idx)
    {
        int i;

        et->len--;
        for (i = idx; i < et->len; ++i)
        {
            et->recorde[i] = et->recorde[i+1];
        }

        //fprintf(stderr, "Remove: et->len = %d\n", et->len);
        return 0;
    }

#ifdef __cplusplus
}
#endif



/* EOF */


