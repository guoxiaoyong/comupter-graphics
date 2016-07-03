#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <camera.h>
#include <meshmo.h>
#include <memstr.h>
#include <objlib.h>
#include <geomet.h>

#include <head.c>
#include <xwing.c>
#include <bunny.c>


typedef void (*Rotate)(Camera*, float);
Rotate rotate;

int main(int argc, char* argv[])
{
    int i, j, k, Imin, Imax;
    int WPixelPERUnit = 400;
    int HPixelPERUnit = 400;
    int Width  = 600;
    int Height = 400;
    //int Width  = 100;
    //int Height = 100;
    RGBColor color;
    Vector3D ray, t;
    Point2D p;
    int info;
    char* objstring;
    float cosine;
    char filename[256];

    assert(argc == 4);
    Imin = atoi(argv[1]);
    Imax = atoi(argv[2]);

    if ( argv[3][0] == 'r')
    {
        rotate = CAM_RotateRight;
    }
    else
    {
        rotate = CAM_RotateUp;
    }


    Camera* camera = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);
    objstring = objstring_bunny;
    VList* vlist = OBJ_VList(objstring);
    FList* flist = OBJ_FList(objstring);
    FList_NormalizeVN(vlist, flist);
    FList_TriangleBoundingBox(vlist, flist);
    FList_TriangleBaryCoord(vlist, flist);
    FList_BoundingBox(flist);

    Vector3D position;
    Vector3D_Set(&position, VEC3TAK(flist->bbox[2], 0), VEC3TAK(flist->bbox[2], 1), VEC3TAK(flist->bbox[2], 2)-0.2);
    
    TCIMG_Clear(camera->image);
    CAM_SetAllFromPositionLookat(camera, &position, &flist->bbox[2]);
    CAM_CompuWRL2CAM(camera);
    CAM_RotateRight(camera, (float)-M_PI/2.0f);
    CAM_CompuWRL2CAM(camera);

    RGBColor_Set(&color, 255, 255, 0);
    RGBColor xcolor;
    TCIMG_SetPenColor(camera->image, &color);

    int iter = 0;
    for (iter = 0; iter <= 1000; ++iter)
    {
        rotate(camera, 2.0f*M_PI/1000.0f);
        CAM_CompuWRL2CAM(camera);
        FList_UpdateFaceBboxshift(flist, camera->position);
        TCIMG_Clear(camera->image);
        TCIMG_Depth_Clear(camera->image);

        printf("iter = %d\n", iter);
        if (iter < Imin || iter > Imax) continue;

        for (i = 0; i < Width; ++i)
        {
            p.x = i;
            for (j = 0; j < Height; ++j)
            {
                p.y = j;
                CAM_Pixel2Ray(camera, i, j, &ray);
                Vector3D_Normalize(&ray);
                cosine = (VEC3TAK(ray,0)*VEC3TAK(ray,0)+VEC3TAK(ray,1)*VEC3TAK(ray,1)) / Vector3D_NormSquare(&ray);
                xcolor.r = (unsigned char)(127*cosine);
                xcolor.g = (unsigned char)(153*cosine);
                xcolor.b = (unsigned char)(255*cosine);
                TCIMG_SetPixelColor(camera->image, &p, &xcolor);

                info = GEOMET_RayFListBoundingBox(&ray, flist);
                if (info)
                {
                    for (k = 0; k < flist->len; k++)
                    {
                        info = GEOMET_RayTriangle(vlist, &flist->f[k], camera->position, &ray, &t);
                        if (info)
                        {
                            cosine = Vector3D_InnerProduct(&flist->f[k].vn, &ray);
                            RGBColor_Scale(&color, fabs(cosine*cosine*cosine), &xcolor);
                            TCIMG_Depth_SetPixelColor(camera->image, &p, &xcolor, VEC3TAK(t, 0));
                        }
                    }
                }
            }
            if (!(i%10)) printf("i = %d\n", i);
        }

        sprintf(filename, "%04d.bmp", iter);
        TCIMG_SaveToFile(camera->image, filename);
    }

    CAM_Destroy(camera);
    VList_Destroy(vlist);
    FList_Destroy(flist);
    return 0;
}



