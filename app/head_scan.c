#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <SDL.h>

#include <camera.h>
#include <meshmo.h>
#include <memstr.h>
#include <objlib.h>
#include <geomet.h>
#include <sdlaux.h>

#include <head.c>
#include <xwing.c>



int main(int argc, char* argv[])
{
    int i;
    int WPixelPERUnit = 400;
    int HPixelPERUnit = 400;
    int Width  = 1000;
    int Height = 600;

    assert(argc == 1);
    assert(argv[0] != NULL);

    Camera* camera    = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);
    SDL_Surface* Screen = SDLAUX_Init(Width, Height, "3D demo by Guo Xiaoyong");

    /* read obj file and create VList and FList */
    char* objstring;
    objstring = objstring_head;

    assert(objstring != NULL);
    VList* vlist = OBJ_VList(objstring);
    FList* flist = OBJ_FList(objstring);
    EList* elist = EList_Create(vlist->len*3);
    //EList_FromFList(elist, flist);
    /*##############################################################*/

    int RotateFlag = 0;
    Vector3D Vlow, Vup;
    TCIMG_SetDepthFlag(camera->image, 1);

    while (1)
    {
        TCIMG_Clear(camera->image);
        CAM_CompuWRL2CAM(camera);
        RGBColor color, xcolor1, xcolor2, xcolor3;
        RGBColor_Set(&color, 255, 255, 255);
        TCIMG_SetPenColor(camera->image, &color);
        /*##############################################################*/
        CAM_VList_Perspective(camera, vlist);
        VList_RangeX(vlist, &Vlow, &Vup);

        float d = VEC3TAK(Vup, 2)-VEC3TAK(Vlow, 2);
        float d1, d2, d3;
        for (i = 0; i < flist->len; ++i)
        {
            Vector3D* depth1 = Face_3DGetX(vlist, &flist->f[i], 0);
            Vector3D* depth2 = Face_3DGetX(vlist, &flist->f[i], 1);
            Vector3D* depth3 = Face_3DGetX(vlist, &flist->f[i], 2);

            d1 = (VEC3TAK(Vup, 2) - VEC3GET(depth1, 2));
            d2 = (VEC3TAK(Vup, 2) - VEC3GET(depth2, 2));
            d3 = (VEC3TAK(Vup, 2) - VEC3GET(depth3, 2));

            RGBColor_Scale(&color, d1/d, &xcolor1);
            RGBColor_Scale(&color, d2/d, &xcolor2);
            RGBColor_Scale(&color, d3/d, &xcolor3);

            //CAM_Triangle_FillGradient(camera, &flist->f[i], &xcolor1, &xcolor2, &xcolor3, vlist);
            //CAM_Triangle_FillColor(camera, &flist->f[i], &xcolor1, vlist);
            CAM_Triangle_Fill(camera, &flist->f[i], vlist);
        }

        /*##############################################################*/
        SDLAUX_ShowCamera(Screen, camera, 0, 0, 1);
        SDL_Delay(20);
#include "sdlevent.c"
    }

QUITPOINT:
    CAM_Destroy(camera);

    VList_Destroy(vlist);
    FList_Destroy(flist);
    EList_Destroy(elist);
    return 0;
}



