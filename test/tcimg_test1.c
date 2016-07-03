#include <stdio.h>
#include <tcimg.h>
#include <math.h>


int main(void)
{
    TrueColorImage* image = TCIMG_Create(400, 400);

    TCIMG_FillClr(image, 0, 0, 0);
    TCIMG_SetPenClr(image, 255, 0, 0);

#if 0
    TCIMG_MoveTo(image, 100, 100);
    TCIMG_LineTo(image, 200, 200);

    TCIMG_MoveTo(image, 100, 100);
    TCIMG_LineTo(image, 200, 300);

    TCIMG_MoveTo(image, 100, 100);
    TCIMG_LineTo(image, 200, 150);

    TCIMG_MoveTo(image, 100, 100);
    TCIMG_LineTo(image, 150,  50);

    TCIMG_MoveTo(image, 100, 100);
    TCIMG_LineTo(image, 150,  80);

    TCIMG_MoveTo(image, 100, 100);
    TCIMG_LineTo(image, 150,  10);
#endif

    Point2D a, b, c;

    /*
    Point2D_Set(&a, 0, 0);
    Point2D_Set(&b, 100, 100);
    Point2D_Set(&c, 20, 100);
    TCIMG_TriangleFill(image, &a, &b, &c);
    */


    Point2D_Set(&a, 0, 0);
    Point2D_Set(&b, 100, 0);
    Point2D_Set(&c, 20, 100);
    TCIMG_TriangleFill(image, &a, &b, &c);

    TCIMG_SetPenClr(image, 255, 255, 0);
    Point2D_Set(&a, 0, 0);
    Point2D_Set(&b, 2, 2);
    Point2D_Set(&c, 20, 100);
    TCIMG_TriangleFill(image, &a, &b, &c);



    TCIMG_SaveToFile(image, "a.bmp");
    TCIMG_Destroy(image);
    return 0;
}



