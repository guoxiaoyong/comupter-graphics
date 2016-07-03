/*###################################################################
 *
 * tcimg.h tcimg.c - tcimg stands for True Color Image. This library
 *                   provides a set of routines to manipulating 24bit
 *                   Bitmap files (widely used Microsoft V3.0). This
 *                   library should work on both small endian machine
 *                   and big endian machine.
 *
 * Copyright (c) 2002-2011, Guo Xiaoyong
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
 *         http://www.guoxiaoyong.net
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


#ifndef   __MSDIBV3_H__
#define   __MSDIBV3_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <math.h>


    /* basic data type definition */
    typedef int              INT32;
    typedef unsigned int     UINT32;
    typedef unsigned short   UINT16;
    typedef unsigned char    UINT8 ;


    /*###################################################################
     *
     * BMP File Header:
     *       Stores general information about the BMP file.
     *
     * Bitmap Information (DIB header):
     *         Stores detailed information about the bitmap image.
     *
     * Color Palette:
     *        Stores the definition of the colors being
     *	 used for indexed color bitmaps.
     *
     * Bitmap Data:
     *         Stores the actual image, pixel by pixel.
     *
     *##################################################################*/




    /*####################################################################
     *
     *  MSDIB_FileHeader: the size is 14 bytes. the struct in memory
     *                    may take up 16 bytes if it is not packed
     *                    To write this struct into file, it is better
     *                    to write one field by one field. The order
     *                    of the fields are important.
     *
     *####################################################################*/

    typedef
    struct stru_MSDIB_FileHeader
    {
        /* bfType = {'B' = 0x42 = 66, 'M' = 0x4D = 77} = 0x4D42 = 19778
         * bfType = [bfType1, bfType2] */
        UINT16  bfType;

        /* the size of the file */
        UINT32  bfSize;

        /* actual value depend on the appplication created them,
         * usually set to zero bfReserved = [bfReserved1, bfReserved2] */
        UINT32   bfReserved;

        /* the starting address of the bitmap data */
        UINT32  bfOffBits;
    }
    MSDIB_FileHeader;


    /*####################################################################
     *
     * MSDIB_InfoHeader_V3: the size is 40 bytes. Like BITMAPFILEHEADER,
     *                      to write this struct to file, it is better to
     *                      write one field by one field, not to write
     *                      them as a whole. The order of the fields are
     *                      Important.
     *
     *
     * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
     * biCompress Field:
     *
     * Value | Identified by  |  Compression method
     * 0       BI_RGB            none
     * 1       BI_RLE8RLE        8-bit/pixel
     * 2       BI_RLE4 RLE       4-bit/pixel
     * 3       BI_BITFIELDS      Bit field
     * 4       BI_JPEGJPEG 	     The bitmap contains a JPEG image
     * 5       BI_PNGPNG 	     The bitmap contains a PNG image
     *
     *
     * Value | Comment
     * 0       Most common
     * 1       Can be used only with 8-bit/pixel bitmaps
     * 2       Can be used only with 4-bit/pixel bitmaps
     * 3       Can be used only with 16 and 32-bit/pixel bitmaps.
     * 4       The bitmap contains a JPEG image
     * 5       The bitmap contains a PNG image
     *
     * Also BI_JPG and BI_PNG are for printer drivers
     * and are not supported when rendering to the screen.
     *
     *####################################################################*/

    typedef
    struct stru_V3MSDIB_InfoHeader
    {
        /* size of the BITMAPINFOHEADER, always 40 */
        UINT32 biSize;

        /* image width/height */
        INT32  biWidth;
        INT32  biHeight;

        /* the number of color planes being used. Must be set to 1 */
        UINT16  biPlanes;

        /* bits per pixel, typical values are 1, 4, 8, 16, 24, 32 */
        UINT16  biBitCount;

        /* compression method */
        UINT32 biCompression;

        /* the size of the raw bitmap data,
         * for BI_RGB image, it can be set to 0 */
        UINT32 biSizeImage;

        /* pixel per meter in herizontal vertical direction */
        INT32 biXPelsPerMeter;
        INT32 biYPelsPerMeter;

        /* the number of colors in the color palette,
         * or 0 to default to 2^n */
        UINT32 biClrUsed;

        /* the number of important colors that is used,
         * if all colors are important,
         * it should be set to 0. This field is usually
         * ignored */
        UINT32 biClrImportant;
    }
    V3MSDIB_InfoHeader;




    /*###################################################################
     *
     * A DIB always uses the RGB color model. In this model,
     * a color is terms of different intensities (from 0 to 255)
     * of the additive primary colors red (R), green (G),
     * and blue (B). A color is thus defined using the 3 values
     * for R, G and B (though stored in backwards order in
     * each palette entry).
     *
     * The number of entries in the palette is either 2^n or
     * a smaller number specified in the header.
     * Each entry contains four bytes.
     *
     * The color palette is not used when the bitmap is
     * 16-bit or higher; there are no palette bytes in
     * those BMP files.
     *
     *#################################################################*/


    typedef struct RGBColor
    {
        UINT8 b;
        UINT8 g;
        UINT8 r;
        UINT8 a;
    } RGBColor;


    typedef struct Point2D
    {
        int x;
        int y;
        int z;
    } Point2D;


    typedef struct PList
    {
        Point2D* p;
        int len;
        int size;
    } PList;


    typedef struct PixEdge
    {
        Point2D p1;
        Point2D p2;
        int active;
    } PixEdge;


    typedef struct ScanLineEdgeRecorde
    {
        float x;
        int   ymax;
        float InverseSlope;
    } ScanLineEdgeRecorde;


    typedef struct EdgeTable
    {
        ScanLineEdgeRecorde* recorde;
        int y;
        int len;
        int size;
    } EdgeTable;

    /*#################################################################
     *
     * Define 24bit TrueColor Image, the storage is compatible with
     * the disk image of BMP file
     *
     * ################################################################*/

    typedef
    struct struct_TrueColorImage
    {
        UINT8   *ImgData;
        float   *DepthBuffer;

        /* dimension */
        INT32    ImgWidth;
        INT32    ImgHeight;

        /* these data can be computed from all the above
         * values, stored here to be ready to use */
        INT32    ImgByteWidth;
        INT32    ImgTotalPixels;
        INT32    ImgTotalBytes;

        /* drawing */
        INT32 XcurrPos;
        INT32 YcurrPos;

        /* pen color */
        UINT8 RPenClr;
        UINT8 GPenClr;
        UINT8 BPenClr;

        /* fill color */
        UINT8 RFillClr;
        UINT8 GFillClr;
        UINT8 BFillClr;



        int DepthFlag;
        PList* PL0;
        PList* PL1;
        PList* PL2;
        PList* PL3;

        EdgeTable* SET;
        EdgeTable* AET;

    } TrueColorImage;








    /*##############################################################
     *
     * IsLittleEndian: test whether the ENDIANNESS of the
     *                 machine currently working on.
     *
     * ############################################################*/

    int IsLittleEndian();

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
                             const void* const data);

    void SEWriteINT32ToFile( FILE* const fp,
                             const char endianness,
                             const void* const data);


    /*##############################################################
     *
     * SEReadINT16FromFile and SEReadINT32FromFile:
     *             Read 2-byte integer and 4-byte integer stored
     *             in little endian order.
     *
     * ############################################################*/


    void SEReadINT16FromFile( FILE* const fp,
                              const char endianness,
                              void* const data);

    void SEReadINT32FromFile( FILE* const fp,
                              const char endianness,
                              void* const data);

    /*#############################################################
     *
     * TCIMG_Init
     *
     * return value:  0: successful
     *                1: file open error
     *                2: file writing error with fputc
     *                3: file writing error with fwrite
     *
     * ############################################################*/

    int TCIMG_Init(TrueColorImage* img, INT32 Width, INT32 Height);

    void TCIMG_Release(TrueColorImage* img);

    /**
     * @brief create a TrueColorImage object
     */
    TrueColorImage* TCIMG_Create(INT32 Width, INT32 Height);

    /**
     * @brief destructor of the TrueColorImage object
     */
    void TCIMG_Destroy(TrueColorImage* img);
    int TCIMG_IsInit(TrueColorImage* img);

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
    int TCIMG_SaveToFile   (TrueColorImage* img, char filename[]);
    int TCIMG_ReadFromFile (TrueColorImage* img, char filename[]);

    int TCIMG_SetPixelClr  (TrueColorImage* img, INT32 x, INT32 y, UINT8 R, UINT8 G, UINT8 B );
    int TCIMG_GetPixelClr  (TrueColorImage* img, INT32 x, INT32 y, UINT8 *R, UINT8 *G, UINT8 *B );
    int TCIMG_FillClr      (TrueColorImage* img, UINT8 R, UINT8 G, UINT8 B);
    int TCIMG_Clear        (TrueColorImage* img);
    int TCIMG_SetPenClr    (TrueColorImage* img, UINT8 R, UINT8 G, UINT8 B);
    int TCIMG_MoveTo       (TrueColorImage* img, INT32 x, INT32 y);
    int TCIMG_LineTo       (TrueColorImage* img, INT32 x, INT32 y);
    int TCIMG_Line         (TrueColorImage* img, INT32 x[], INT32 y[], int len);
    int TCIMG_Polygon      (TrueColorImage* img, INT32 x[], INT32 y[], int len);
    int TCIMG_PolygonFill  (TrueColorImage* img, INT32 x[], INT32 y[], int len);
    int TCIMG_SetDepthFlag (TrueColorImage* img, int flag);


    int TCIMG_SetPixelColor  (TrueColorImage* img, Point2D* p, RGBColor* color);
    int TCIMG_SetPenColor    (TrueColorImage* img, RGBColor* color);
    int TCIMG_FillColor      (TrueColorImage* img, RGBColor* c);
    int TCIMG_LineToGradient (TrueColorImage* img, Point2D* x, Point2D* y, RGBColor* c1, RGBColor* c2);
    int TCIMG_TriangleFill   (TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3);
    int TCIMG_TriangleFillColor(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3, RGBColor* color);
    int TCIMG_TriangleFillGradient(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3, RGBColor* c1, RGBColor* c2, RGBColor* c3);
    int TCIMG_Triangle(TrueColorImage* img, Point2D* p1, Point2D* p2, Point2D* p3);
    int TCIMG_TriangleAETLine(TrueColorImage* img);
    int TCIMG_TriangleUpdateAET(TrueColorImage* img);


    void    TCIMG_Depth_Clear(TrueColorImage* img);
    float*  TCIMG_Depth_GetPixelDepth(TrueColorImage* img, Point2D* p);
    int     TCIMG_Depth_SetPixelColor(TrueColorImage* img, Point2D* p, RGBColor* color, float depth);



    /**
     * RGBColor
     */
    RGBColor* RGBColor_Create(UINT8 r, UINT8 g, UINT8 b);
    void  RGBColor_Destroy(RGBColor* color);
    void  RGBColor_Set(RGBColor* color, UINT8 r, UINT8 g, UINT8 b);
    UINT8 RGBColor_GetR(RGBColor* color);
    UINT8 RGBColor_GetG(RGBColor* color);
    UINT8 RGBColor_GetB(RGBColor* color);
    void  RGBColor_Scale(RGBColor* color, double scale, RGBColor* xcolor);
    void  RGBColor_Add(RGBColor* c1, RGBColor* c2, RGBColor* c);
    void  RGBColor_LinearGradient(RGBColor* c1, RGBColor* c2, double f, RGBColor* c);
    void  RGBColor_Random(RGBColor* c);
    void  RGBColor_VSet(RGBColor* r1, RGBColor* r2);
    void  RGBColor_BaryInterpolation(RGBColor* c1, RGBColor* c2, RGBColor* c3, float a1, float a2, float a3, RGBColor* c);


    /**
     * Point2D
     */
    Point2D* Point2D_Create(int x, int y);
    void   Point2D_Destroy(Point2D* p);
    int    Point2D_PointsOnLine(Point2D* p1, Point2D* p2, Point2D* p);
    void   Point2D_Minus(Point2D* p1, Point2D* p2, Point2D* p);
    void   Point2D_Abs(Point2D* p);
    void   Point2D_Set(Point2D* p, int x, int y);
    void   Point2D_VSet(Point2D* p1, Point2D* p2);
    void   Point2D_CrossProduct(Point2D *a, Point2D *b, Point2D *c);
    int    Point2D_InTriangle(Point2D* p, Point2D *a, Point2D *b, Point2D *c);
    double Point2D_EInnerProduct(Point2D* a, Point2D* b);
    double Point2D_HInnerProduct(Point2D* a, Point2D* b);
    int    Point2D_NormSquare(Point2D* p1);
    int    Point2D_DistanceSquare(Point2D* p1, Point2D* p2);
    double Point2D_Distance(Point2D* p1, Point2D* p2);
    void   Point2D_Print(Point2D* p);
    void   Point2D_Swap(Point2D* p1, Point2D* p2);
    void   Point2D_SortTriangleX(Point2D* p1, Point2D* p2, Point2D* p3);
    void   Point2D_SortTriangleY(Point2D* p1, Point2D* p2, Point2D* p3);


    /**
     * PList
     **/
    PList* PList_Create(int size);
    void PList_Destroy(PList* plist);
    void PList_Line(PList* plist, Point2D* p1, Point2D* p2);


    /**
     */
    EdgeTable* EdgeTable_Create(int size);
    void EdgeTable_Destroy(EdgeTable* et);
    int  EdgeTable_Add(EdgeTable* et, Point2D* p1, Point2D* p2);
    int  EdgeTable_AddRecorde(EdgeTable* et, ScanLineEdgeRecorde* recorde);
    int  EdgeTable_Remove(EdgeTable* et, int idx);

#ifdef __cplusplus
}
#endif

#endif


/* EOF */


