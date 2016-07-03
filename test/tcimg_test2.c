#include <stdio.h>
#include <tcimg.h>
#include <math.h>


int main(void)
{
    TrueColorImage* image = TCIMG_Create(40, 40);

    TCIMG_Clear(image);
    TCIMG_SetPenClr(image, 255, 0, 0);

    Point2D a, b, c;
    Point2D_Set(&a, 0, 0);
    Point2D_Set(&b, 10, 5);
    Point2D_Set(&c, 2, 2);

    RGBColor c1, c2, c3;
    RGBColor_Set(&c1, 10, 10, 200);
    RGBColor_Set(&c2, 200, 255, 0);
    RGBColor_Set(&c3, 10, 10, 10);

    TCIMG_Triangle(image, &a, &b, &c);
    //TCIMG_TriangleFill(image, &a, &b, &c);
    //TCIMG_TriangleFillGradient(image, &a, &b, &c, &c1, &c2, &c3);

    TCIMG_SaveToFile(image, "a.bmp");
    return 2;
}



