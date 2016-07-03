#include <stdio.h>
#include <math.h>
#include <assert.h>


#include <SDL/SDL.h>

#include <camera.h>
#include <meshmo.h>
#include <memstr.h>
#include <objlib.h>
#include <geomet.h>

#include <xwing.c>
#include <../data/sphere.c>
#include <head.c>
#include <cube.c>
#include <bunny.c>
#include <horse1.c>




int main(int argc, char* argv[])
{
    int i, j, k;
    double DeltaGamma = 2.*M_PI / 1000.;
    double gamma;

    Vector3D* position = Vector3D_Create(0.0f, 0.0f, 0.0f);
    Vector3D* lookat   = Vector3D_Create(0.0f, 0.0f, 0.0f);

    int WPixelPERUnit = 400;
    int HPixelPERUnit = 400;
    int Width  = 600;
    int Height = 600;

    assert(argc == 1);
    assert(argv[0] != NULL);

    Camera* camera = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);

    if ( SDL_Init( SDL_INIT_VIDEO)  < 0 )
    {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    /* set video mode */
    SDL_Surface *Screen = SDL_SetVideoMode(camera->image->ImgWidth, camera->image->ImgHeight, 24, SDL_SWSURFACE);
    SDL_WM_SetCaption("3D demo by Guo Xiaoyong: Backface elimination", NULL);
    atexit(SDL_Quit);

    /* read obj file and create VList and FList */
    char* objstring;
    int model = 0;
    float scale = 1.4;

MODEL:
    switch (model)
    {
    case 0:
        objstring = objstring_xwing;
        scale = 1.4;
        break;
    case 1:
        objstring = objstring_sphere;
        scale = 1.9;
        break;
    case 2:
        objstring = objstring_head;
        scale = 1.4;
        break;
    case 3:
        objstring = objstring_cube;
        scale = 4;
        break;
    case 4:
        objstring = objstring_bunny;
        scale = 1.2;
        break;
    case 5:
        objstring = objstring_horse;
        scale = 2.2;
        break;
    }

    assert(objstring != NULL);

    VList* vlist = OBJ_VList(objstring);
    FList* flist = OBJ_FList(objstring);
    FList_FaceNormal(vlist, flist);


    /*##############################################################*/

    k = 0;
    int stop = 0;
    int NoBackFace = 1;
    Vector3D Vlow, Vup;
    float Radius = VList_Range(vlist, &Vlow, &Vup);
    Radius*=scale;

    VEC3GET(position, 2) = 0.0f;

    for (;;)
    {
        if (!stop) k++;
        TCIMG_Clear(camera->image);
        gamma = fmod( k*DeltaGamma, 2*M_PI );

        VEC3GET(position, 0) = Radius * cos(gamma);
        VEC3GET(position, 1) = Radius * sin(gamma);

        CAM_SetAllFromPositionLookat(camera, position, lookat);
        CAM_CompuWRL2CAM(camera);

        TCIMG_SetPenClr(camera->image, 150, 20, 200);

        /*##############################################################*/

        CAM_VList_Perspective(camera, vlist);
        for (i = 0; i < flist->len; ++i)
        {
            if (NoBackFace)
            {
                if ( CAM_IsBackFace(camera, vlist, &flist->f[i]) )
                {
                    CAM_Face_Draw(camera, &flist->f[i], vlist);
                }
            }
            else
            {
                CAM_Face_Draw(camera, &flist->f[i], vlist);
            }
        }

        /*##############################################################*/
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

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYUP:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    goto QUITPOINT;
                }
                else if (event.key.keysym.sym == SDLK_SPACE)
                {
                    stop = !stop;
                }
                else if (event.key.keysym.sym == SDLK_b)
                {
                    NoBackFace = !NoBackFace;
                }
                else if (event.key.keysym.sym == SDLK_UP)
                {
                    VEC3GET(position, 2) += Radius*0.1;
                }
                else if (event.key.keysym.sym == SDLK_DOWN)
                {
                    VEC3GET(position, 2) -= Radius*0.1;
                }
                else if (event.key.keysym.sym == SDLK_1)
                {
                    model = 1;
                    goto MODEL;
                }
                else if (event.key.keysym.sym == SDLK_2)
                {
                    model = 2;
                    goto MODEL;
                }
                else if (event.key.keysym.sym == SDLK_3)
                {
                    model = 3;
                    Radius *= 30;
                    goto MODEL;
                }
                else if (event.key.keysym.sym == SDLK_4)
                {
                    model = 4;
                    goto MODEL;
                }
                else if (event.key.keysym.sym == SDLK_5)
                {
                    model = 5;
                    goto MODEL;
                }
                else if (event.key.keysym.sym == SDLK_0)
                {
                    model = 0;
                    goto MODEL;
                }
                else if (event.key.keysym.sym == SDLK_n)
                {
                    Radius *= 1.1;
                }
                else if (event.key.keysym.sym == SDLK_m)
                {
                    Radius /= 1.1;
                }


                break;
            }
            case SDL_QUIT:
            {
                goto QUITPOINT;
                break; /* useless */
            }
            }
        }



    }



QUITPOINT:
    CAM_Destroy(camera);
    Vector3D_Destroy(position);
    Vector3D_Destroy(lookat);
    VList_Destroy(vlist);
    FList_Destroy(flist);
    return 0;
}





