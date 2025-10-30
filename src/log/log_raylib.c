#include "log/log.h"
#include "log_raylib.h"
#include <stdio.h>

void log_callback_raylib(log_level_t level, const char *fmt, va_list args) {
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    // Qui puoi sostituire con il tuo metodo di logging GUI
    // es: GtkTextBuffer, ImGui, Qt console, ecc.
    //gui_append_log(level, buffer);
}