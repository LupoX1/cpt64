#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "c64.h"
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

static c64_system_t *g_c64 = NULL;

void sigfault_handler(int sig, siginfo_t *si, void *unused)
{
    printf("\n=== SIGSEGV Caught! ===\n");
    printf("Fault address: %p\n", si->si_addr);
    printf("Dumping state...\n");
    
    if (g_c64) {
        c64_dump_state(g_c64, "crash_dump.txt");
        printf("State dumped to crash_dump.txt\n");
    }
    
    exit(1);
}

int main(int argc, char *argv[])
{
    // Setup signal handler
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigfault_handler;
    sigaction(SIGSEGV, &sa, NULL);
    
    printf("=== %s v%s ===\n", APP_NAME, APP_VERSION);
    printf("%s\n", APP_DESCRIPTION);
    printf("Build: Console mode (no SDL)\n\n");
    
    // Crea sistema
    g_c64 = c64_create();
    if (!g_c64) {
        fprintf(stderr, "Failed to create C64 system\n");
        return 1;
    }
    
    // Reset
    printf("Resetting C64...\n");
    c64_reset(g_c64);
    
    // Carica programma se specificato
    if (argc > 1) {
        uint16_t addr = 0x0801;  // Default BASIC start
        if (argc > 2) {
            addr = (uint16_t)strtol(argv[2], NULL, 16);
        }
        
        printf("Loading %s at $%04X\n", argv[1], addr);
        if (!c64_load_program(g_c64, argv[1], addr)) {
            fprintf(stderr, "Failed to load program\n");
            c64_destroy(g_c64);
            return 1;
        }
    }
    
    // Loop principale
    printf("\nRunning... (Press Ctrl+C to stop)\n");
    printf("Cycles: %lu\r", c64_get_cycles(g_c64));
    fflush(stdout);
    
    uint64_t last_report = 0;
    while (c64_step(g_c64)) {
        uint64_t cycles = c64_get_cycles(g_c64);
        
        // Report ogni 100k cicli
        if (cycles - last_report > 100000) {
            printf("Cycles: %lu\r", cycles);
            fflush(stdout);
            last_report = cycles;
        }
    }
    
    printf("\n\nExecution stopped at %lu cycles\n", c64_get_cycles(g_c64));
    
    // Dump finale
    printf("Dumping final state...\n");
    c64_dump_state(g_c64, "final_dump.txt");
    printf("State saved to final_dump.txt\n");
    
    // Cleanup
    c64_destroy(g_c64);
    
    return 0;
}

#endif
