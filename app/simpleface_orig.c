#include <stdio.h>
#include <math.h>
#include <assert.h>


#include <camera.h>
#include <SDL/SDL.h>


double Theta  [1024];
double Phi    [1024];

Vector3D x[1024];
Vector3D y[1024];

int a[1024];
int b[1024];



int main(int argc, char* argv[])
{

    int i, j, k;
    int numTheta;
    int numPhi;
    double DeltaTheta;
    double DeltaPhi;
    double DeltaGamma = 2*M_PI / 1000;
    double gamma;

    Vector3D* position = Vector3D_Create(0.0f, 0.0f, 2.0f);
    Vector3D* lookat   = Vector3D_Create(0.0f, 0.0f, 0.0f);

    int WPixelPERUnit = 400;
    int HPixelPERUnit = 400;
    int Width  = 400;
    int Height = 400;

    assert(argc == 1);
    assert(argv[0] != NULL);

    Camera* camera = CAM_Create(Width, Height, WPixelPERUnit, HPixelPERUnit);

    if ( SDL_Init( SDL_INIT_VIDEO)  < 0 )
    {
        fprintf(stderr, "Unable to initialize SDL: %s\n",
                SDL_GetError());

        exit(1);
    }

    /* set video mode */
    SDL_Surface *Screen = SDL_SetVideoMode(camera->image->ImgWidth, camera->image->ImgHeight, 24, SDL_SWSURFACE);
    SDL_WM_SetCaption("3D reconstruction demo by Guo Xiaoyong: original", NULL);
    atexit(SDL_Quit);


    /*##############################################################*/

    k = 0;
    Vector3D depth;

    for (;;)
    {
        k++;
        TCIMG_Clear(camera->image);
        gamma = fmod( k*DeltaGamma, 2*M_PI );

        VEC3GET(position, 0) = 3.5 * cos(gamma);
        VEC3GET(position, 1) = 3.5 * sin(gamma);
        VEC3GET(position, 2) = 0.0;

        CAM_SetAllFromPositionLookat(camera, position, lookat);
        CAM_CompuWRL2CAM(camera);

        /*
        for (i = 0; i < 3; ++i)
                for (j = 0; j < 4; ++j)
        printf("wrl2cam(%d, %d) = %f;\n", i+1, j+1, MAT3GET(camera->wrl2cam, i, j));
        */


        TCIMG_SetPenClr(camera->image, 255, 0, 0);

        /*##############################################################*/
        numTheta = 20;
        numPhi   = 20;
        DeltaTheta = (2. * M_PI) / numTheta;
        DeltaPhi   = M_PI / numPhi;

        int numDrawTheta = numTheta/5;
        int numDrawPhi = 12;
        int PhiStart = 5;

        for (i = 0; i < numTheta; ++i)
        {
            Theta[i] = i*DeltaTheta;
        }

        for (i = 0; i < numPhi; ++i)
        {
            Phi[i] = i*DeltaPhi - M_PI/2.0;
        }


        int idx = 0;
        /* draw lattitude */
        for (i = PhiStart; i < PhiStart+numDrawPhi; i++)
        {
            for (j = 0; j < numDrawTheta; j++)
            {
                idx++;
                VEC3TAK(x[j], 0) = cos( Phi[i] ) * cos( Theta[j] );
                VEC3TAK(x[j], 1) = cos( Phi[i] ) * sin( Theta[j] );
                VEC3TAK(x[j], 2) = sin( Phi[i] );

                CAM_Perspective(camera, &x[j], &a[j], &b[j], &depth);
                //printf("x{%d} = [%f; %f; %f];\n", idx, VEC3TAK(x[j], 0), VEC3TAK(x[j], 1), VEC3TAK(x[j], 2));
            }

            TCIMG_Line(camera->image, a, b, numDrawTheta);
        }

#if 1
        /* draw longitude */
        for (j = 0; j < numDrawTheta; j++)
        {
            for (i = PhiStart; i < PhiStart+numDrawPhi; i++)
            {
                VEC3TAK(x[i], 0) = cos( Phi[i] ) * cos( Theta[j] );
                VEC3TAK(x[i], 1) = cos( Phi[i] ) * sin( Theta[j] );
                VEC3TAK(x[i], 2) = sin( Phi[i] );
                CAM_Perspective(camera, &x[i], &a[i-PhiStart], &b[i-PhiStart], &depth);
            }

            TCIMG_Line(camera->image, a, b, numDrawPhi);
        }
#endif

        //TCIMG_SaveToFile(camera->image, "face1.bmp");



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
    return 0;
}





