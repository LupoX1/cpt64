#ifndef LOG_INTERNAL_H
#define LOG_INTERNAL_H

#include "log/log.h"

// Vtable delle operazioni
typedef struct {
    void (*output)(log_level_t level, const char *message);
    void (*destroy)(void);
} log_ops_t;

// Struttura concreta del contesto
struct log_context {
    log_level_t min_level;
    const log_ops_t *ops;
};

#endif // LOG_INTERNAL_H