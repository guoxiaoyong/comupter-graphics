SDL_Event event;
while (SDL_PollEvent(&event))
{
    switch (event.type)
    {
    case SDL_KEYDOWN:
    {
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            goto QUITPOINT;
        }
        else if (event.key.keysym.sym == SDLK_b)
        {
            CAM_SetBackFace(camera, !camera->BackFaceFlag);
        }
        else if (event.key.keysym.sym == SDLK_UP)
        {
            if (RotateFlag)
                CAM_RotateUp(camera, 0.02);
            else
                CAM_MoveUp(camera, step);
        }
        else if (event.key.keysym.sym == SDLK_DOWN)
        {
            if (RotateFlag)
                CAM_RotateUp(camera, -0.02);
            else
                CAM_MoveUp(camera, -step);
        }
        else if (event.key.keysym.sym == SDLK_LEFT)
        {
            if (RotateFlag)
                CAM_RotateRight(camera, -0.02);
            else
                CAM_MoveRight(camera, -step);
        }
        else if (event.key.keysym.sym == SDLK_RIGHT)
        {
            if (RotateFlag)
                CAM_RotateRight(camera, +0.02);
            else
                CAM_MoveRight(camera, step);
        }
        else if (event.key.keysym.sym == SDLK_w)
        {
            CAM_MoveForwardFixLookat(camera, step);
        }
        else if (event.key.keysym.sym == SDLK_s)
        {
            CAM_MoveForwardFixLookat(camera, -step);
        }
        else if (event.key.keysym.sym == SDLK_r)
        {
            RotateFlag = !RotateFlag;
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
            goto MODEL;
        }
        else if (event.key.keysym.sym == SDLK_4)
        {
            model = 4;
            goto MODEL;
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
