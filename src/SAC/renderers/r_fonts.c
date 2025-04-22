#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <mouse_commands.h>
#include <key_commands.h>

#include "SAC/sac.h"
#include "SAC/renderers/r_fonts.h"
// #include "SAC/renderers/renderer_SDL3.h"
#include "SAC/time/time.h"
#include "SAC/vulkan/setup.h"

int setup_r_fonts(AppState *state) {

  // TODO: Figure out how you want to handle text
  printf("SETUP: Load fonts\n");
  printf("TODO: configure fonts correctly\n");
  // Text Shit
  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
  // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
  //IM_ASSERT(font != nullptr);
  // TODO: figure if wanted?
  // state->rendererData.textEngine =
  //     TTF_CreateRendererTextEngine(state->rendererData.renderer);
  // if (!state->rendererData.textEngine) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR,
  //                "Failed to create text engine from renderer: %s",
  //                SDL_GetError());
  //   state->sdl_result = SDL_APP_FAILURE;
  //   return 1;
  // }

  // state->rendererData.fonts = SDL_calloc(1, sizeof(TTF_Font *));
  // if (!state->rendererData.fonts) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR,
  //                "Failed to allocate memory for the font array: %s",
  //                SDL_GetError());
  //   state->sdl_result = SDL_APP_FAILURE;
  //   return 1;
  // }

  // TTF_Font *font = TTF_OpenFont("assets/Roboto-Regular.ttf", 24);
  // if (!font) {
  //   SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s",
  //                SDL_GetError());
  //   state->sdl_result = SDL_APP_FAILURE;
  //   return 1;
  // }

  // state->rendererData.fonts[SAC_RENDERERS_R_FONTS_H_FONT_ID] = font;

  return 0;
}
