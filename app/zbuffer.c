#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <SDL/SDL.h>

#include <camera.h>
#include <meshmo.h>
#include <memstr.h>
#include <objlib.h>
#include <geomet.h>
#include <sdlaux.h>

#include <head.c>
#include <xwing.c>
#include <bunny.c>
#include <horse.c>


int RotateFlag = 1;
int model = 1;

int main(int argc, char** argv)
{
    assert(argc == 1);
    assert(argv != NULL);

    int i, j, idx1, idx2, idx3;
    int WPixelPERUnit = 400;
    int HPixelPERUnit = 400;
    int Width  = 600;
    int Height = 400;
    RGBColor color, xcolor;
    float cosine;
    Point2D p;
    Vector3D ray;

    VList* vlist = NULL;
    FList* flist = NULL;
    char* objstring = NULL;
    SDL_Surface* Screen;
    Camera* camera;

    Screen = SDLAUX_Init(Width, Height, "z-buffer demo by Guo Xiaoyong");
    camera = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);

MODEL:
    switch (model)
    {
        case 1: objstring = objstring_head;  break;
        case 2: objstring = objstring_bunny; break;
        case 3: objstring = objstring_xwing; break;
        case 4: objstring = objstring_horse; break;
    }

    if (vlist) VList_Destroy(vlist);
    if (flist) FList_Destroy(flist);

    vlist = OBJ_VList(objstring);
    flist = OBJ_FList(objstring);
    FList_NormalizeVN(vlist, flist);
    FList_TriangleBoundingBox(vlist, flist);
    FList_TriangleBaryCoord(vlist, flist);
    FList_BoundingBox(flist);
    VList_AverageNormal(vlist, flist);

    Vector3D position;
    Vector3D_Set(&position, VEC3TAK(flist->bbox[2], 0), VEC3TAK(flist->bbox[2], 1), VEC3TAK(flist->bbox[2], 2)-5*VEC3TAK(flist->bbox[3], 2));

    float step = VEC3TAK(flist->bbox[3], 2) / 10.0f;
    
    CAM_SetAllFromPositionLookat(camera, &position, &flist->bbox[2]);
    CAM_CompuWRL2CAM(camera);
    CAM_RotateRight(camera, (float)-M_PI / 2.0f);
    CAM_CompuWRL2CAM(camera);

    RGBColor_Set(&color, 255, 255, 0);
    TCIMG_SetPenColor(camera->image, &color);

    while(1)
    {
        //TCIMG_Clear(camera->image);
        TCIMG_Depth_Clear(camera->image);
        CAM_CompuWRL2CAM(camera);
        CAM_VList_Perspective(camera, vlist);
        CAM_RotateRight(camera, 2.0f*M_PI/1000.0f);

        for (i = 0; i < Width; ++i)
        {
            p.x = i;
            for (j = 0; j < Height; ++j)
            {
                p.y = j;
                CAM_Pixel2Ray(camera, i, j, &ray);
                Vector3D_Normalize(&ray);
                cosine = (VEC3TAK(ray,0)*VEC3TAK(ray,0)+VEC3TAK(ray,1)*VEC3TAK(ray,1));
                xcolor.r = (unsigned char)(127*cosine);
                xcolor.g = (unsigned char)(153*cosine);
                xcolor.b = (unsigned char)(255*cosine);
                TCIMG_SetPixelColor(camera->image, &p, &xcolor);
            }
        }

    
        for (i = 0; i < flist->len; ++i)
        {
            idx1 = flist->f[i].v[0];
            idx2 = flist->f[i].v[1];
            idx3 = flist->f[i].v[2];

            cosine = Vector3D_InnerProduct(&flist->f[i].vn, camera->look);
            RGBColor_Scale(&color, fabs(cosine*cosine*cosine), &xcolor);
            CAM_DrawTriangleColor(camera, &vlist->p[idx1], &vlist->p[idx2], &vlist->p[idx3], 
                            &xcolor, VEC3TAK(vlist->vx[idx1], 2), VEC3TAK(vlist->vx[idx2], 2), VEC3TAK(vlist->vx[idx3], 2));
            
        }


        SDL_LockSurface(Screen);

        Uint8 r, g, b;
        for (j = 0; j < Height; ++j)
        {
            for (i = 0; i < Width; ++i)
            {
                TCIMG_GetPixelClr(camera->image, i, Height-1-j, &r, &g, &b);
                Uint8* buf = (Uint8*)Screen->pixels;
                buf[j*Screen->pitch + 3*i + 0] = b;
                buf[j*Screen->pitch + 3*i + 1] = g;
                buf[j*Screen->pitch + 3*i + 2] = r;
            }
        }

        SDL_UnlockSurface(Screen);
        SDL_Flip(Screen);
        SDL_Delay(20);

#include "sdlevent.c"
    }

QUITPOINT:
    CAM_Destroy(camera);
    VList_Destroy(vlist);
    FList_Destroy(flist);

    return 0;
}



