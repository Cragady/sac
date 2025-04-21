#ifndef SAC_RENDERERS_RENDERER_SDL3_H_
#define SAC_RENDERERS_RENDERER_SDL3_H_

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

typedef struct {
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    TTF_Font **fonts;
} SDL3RendererData;

#endif
