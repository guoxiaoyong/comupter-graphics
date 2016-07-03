#ifdef __cplusplus
extern "C" {
#endif

#include <camera.h>
#include <SDL.h>


    SDL_Surface* SDLAUX_Init(int Width, int Height, char WCaption[])
    {
        if ( SDL_Init( SDL_INIT_VIDEO)  < 0 )
        {
            fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
            exit(1);
        }


        SDL_EnableKeyRepeat(100, SDL_DEFAULT_REPEAT_INTERVAL);

        /* set video mode */
        SDL_Surface *Screen = SDL_SetVideoMode(Width, Height, 24, SDL_SWSURFACE);
        SDL_WM_SetCaption(WCaption, NULL);
        atexit(SDL_Quit);

        return Screen;
    }


    void SDLAUX_ShowCamera(SDL_Surface* Screen, Camera* camera, int UpLeftX, int UpLeftY, int FlipFlag)
    {
        int i, j;
        Uint8 r, g, b;

        if (FlipFlag)
        {
            SDL_LockSurface(Screen);
        }

        for (j = 0; j < camera->image->ImgHeight; ++j)
        {
            for (i = 0; i < camera->image->ImgWidth; ++i)
            {
                TCIMG_GetPixelClr(camera->image, i, camera->image->ImgHeight-1-j, &r, &g, &b);
                Uint8* buf = (Uint8*)Screen->pixels + UpLeftY*Screen->pitch+3*UpLeftX;
                buf[j*Screen->pitch + 3*i + 0] = b;
                buf[j*Screen->pitch + 3*i + 1] = g;
                buf[j*Screen->pitch + 3*i + 2] = r;
            }
        }

        if (FlipFlag)
        {
            SDL_UnlockSurface(Screen);
            SDL_Flip(Screen);
        }
    }


#ifdef __cplusplus
}
#endif

