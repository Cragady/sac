#ifndef SAC_RENDERERS_CLAY_RENDERER_SDL3_H
#define SAC_RENDERERS_CLAY_RENDERER_SDL3_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <clay.h>

typedef struct {
    SDL_Renderer *renderer;
    TTF_TextEngine *textEngine;
    TTF_Font **fonts;
} Clay_SDL3RendererData;

#endif
