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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "c64_core.h"

// const uint64_t ns_per_cycle = 1000000000 / 985000;

C64_Core *core;

void segfault_handler_extended(int sig, siginfo_t *si, void *unused) {
    printf("SIGSEGV catturato!\n");
    printf("Indirizzo che ha causato l'errore: %p\n", si->si_addr);
    printf("Codice errore: %d\n", si->si_code);

    dump_data(core);
    
    exit(1);
}

int main(int argc, char* argv[]) {

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = segfault_handler_extended;
    
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("=== %s ===\n", APP_NAME);
    printf("Version: %s\n", APP_VERSION);
    printf("Description: %s\n", APP_DESCRIPTION);
    printf("\n--- Build without SDL3 ---\n");
    
    printf("Running without SDL3 support... HIT ENTER TO STEP\n");
    getchar();
    printf("\033[2J\033[1;1H");
    
    core = c64_core_create();
    c64_core_reset(core);
    
    do
    {
      //dump_data(core);
      //c64_log_status(core);
      //getchar();
    }while(c64_core_step(core));

    dump_data(core);

    c64_core_destroy(core);

    return 0;
}

#endif
