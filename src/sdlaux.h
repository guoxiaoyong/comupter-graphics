#ifndef __SDLAUX_H__
#define __SDLAUX_H__

#ifdef __cplusplus
extern "C" {
#endif

    SDL_Surface* SDLAUX_Init(int Width, int Height, char WCaption[]);
    void SDLAUX_ShowCamera(SDL_Surface* Screen, Camera* camera, int UpLeftX, int UpLeftY, int FlipFlag);

#ifdef __cplusplus
}
#endif


#endif
