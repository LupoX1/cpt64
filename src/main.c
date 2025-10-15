#include <stdio.h>
#include "config.h"

#ifdef USE_SDL

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)
{
  SDL_Log("%s launching with %d params %s", APP_NAME, argc, argv[0]);
  SDL_SetLogPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

  if (!SDL_SetAppMetadata(APP_NAME, APP_VERSION, APP_ID)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot set metadata: %s", SDL_GetError());
      return SDL_APP_FAILURE;
  }
  
  if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
      SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL init failed: %s", SDL_GetError());
      return SDL_APP_FAILURE;
  }

  SDL_Log("Initialization complete");
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate)
{
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
  if (event->type == SDL_EVENT_QUIT) {
      SDL_Log("Quit event received");
      return SDL_APP_SUCCESS;
  }
  
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    SDL_Log("Shutting down with (%s)", result == SDL_APP_SUCCESS ? "success" : "error");
}

#else

#include <stdio.h>

#include "c64_core.h"

int main(int argc, char* argv[]) {
    printf("=== %s ===\n", APP_NAME);
    printf("Version: %s\n", APP_VERSION);
    printf("Description: %s\n", APP_DESCRIPTION);
    printf("\n--- Build without SDL3 ---\n");
    
    // Placeholder per logica senza SDL
    printf("Running without SDL3 support...\n");

    C64_Core *core = c64_core_create();
    c64_core_reset(core);

    for(uint8_t i = 0; i<100; i++)
    {
      c64_core_step(core);
    }

    dump_data(core);

    c64_core_destroy(core);

    return 0;
}

#endif
