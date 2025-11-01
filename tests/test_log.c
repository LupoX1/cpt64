#include "log/log.h"
#include <stdio.h>

int main(void) {
    printf("Testing log module...\n");
    
    log_debug("This is a debug message");
    log_info("This is an info message");
    log_warn("This is a warning message");
    log_error("This is an error message");
    
    printf("Log tests passed!\n");
    return 0;
}
