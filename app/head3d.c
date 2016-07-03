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
#include <cube.c>

int main(int argc, char* argv[])
{
    int i;
    int WPixelPERUnit = 400;
    int HPixelPERUnit = 400;
    int Width  = 600;
    int Height = 600;

    assert(argc == 1);
    assert(argv[0] != NULL);

    Camera* cameraL   = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);
    Camera* cameraR   = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);
    SDL_Surface* Screen = SDLAUX_Init(Width*2, Height, "3D demo by Guo Xiaoyong");

    Vector3D_Add(cameraL->position, cameraL->right, cameraR->position);

    /* read obj file and create VList and FList */
    char* objstring;
    objstring = objstring_head;
    float d, d1, d2;

    assert(objstring != NULL);
    VList* vlist = OBJ_VList(objstring);
    FList* flist = OBJ_FList(objstring);
    EList* elist = EList_Create(vlist->len*3);
    EList_FromFList(elist, flist);
    /*##############################################################*/

    int RotateFlag = 0;
    Vector3D Vlow, Vup;
    TCIMG_SetDepthFlag(cameraL->image, 1);
    TCIMG_SetDepthFlag(cameraR->image, 1);

    while (1)
    {
        TCIMG_Clear(cameraL->image);
        TCIMG_Clear(cameraR->image);
        CAM_CompuWRL2CAM(cameraL);
        CAM_CompuWRL2CAM(cameraR);

        RGBColor color, xcolor1, xcolor2;
        RGBColor_Set(&color, 255, 255, 255);
        TCIMG_SetPenColor(cameraL->image, &color);
        TCIMG_SetPenColor(cameraR->image, &color);

        /*##############################################################*/
        CAM_VList_Perspective(cameraL, vlist);
        VList_RangeX(vlist, &Vlow, &Vup);
        d = VEC3TAK(Vup, 2)-VEC3TAK(Vlow, 2);
        for (i = 0; i < elist->len; ++i)
        {
            Vector3D* depth1 = Edge_3DGetX(vlist, &elist->e[i], 0);
            Vector3D* depth2 = Edge_3DGetX(vlist, &elist->e[i], 1);
            d1 = (VEC3TAK(Vup, 2) - VEC3GET(depth1, 2));
            d2 = (VEC3TAK(Vup, 2) - VEC3GET(depth2, 2));
            RGBColor_Scale(&color, d1/d, &xcolor1);
            RGBColor_Scale(&color, d2/d, &xcolor2);
            CAM_Edge_DrawColor(cameraL, &elist->e[i], vlist, &xcolor1, &xcolor2);
        }


        CAM_VList_Perspective(cameraR, vlist);
        VList_RangeX(vlist, &Vlow, &Vup);
        d = VEC3TAK(Vup, 2)-VEC3TAK(Vlow, 2);
        for (i = 0; i < elist->len; ++i)
        {
            Vector3D* depth1 = Edge_3DGetX(vlist, &elist->e[i], 0);
            Vector3D* depth2 = Edge_3DGetX(vlist, &elist->e[i], 1);
            d1 = (VEC3TAK(Vup, 2) - VEC3GET(depth1, 2));
            d2 = (VEC3TAK(Vup, 2) - VEC3GET(depth2, 2));
            RGBColor_Scale(&color, d1/d, &xcolor1);
            RGBColor_Scale(&color, d2/d, &xcolor2);
            CAM_Edge_DrawColor(cameraR, &elist->e[i], vlist, &xcolor1, &xcolor2);
        }




        /*##############################################################*/
        SDL_LockSurface(Screen);
        SDLAUX_ShowCamera(Screen, cameraL, 0, 0, 0);
        SDLAUX_ShowCamera(Screen, cameraR, Width, 0, 0);
        SDL_UnlockSurface(Screen);
        SDL_Flip(Screen);
        SDL_Delay(20);
#include "sdleventx.c"
    }

QUITPOINT:
    CAM_Destroy(cameraL);
    CAM_Destroy(cameraR);

    VList_Destroy(vlist);
    FList_Destroy(flist);
    EList_Destroy(elist);
    return 0;
}



