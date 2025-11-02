#include "core/emu.h"
#include "log/log.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
    printf("Testing core module...\n");
    
    emu_t *emu = emu_create();
    assert(emu != NULL);
    assert(emu->running == false);
    
    emu_shutdown(emu);
    
    printf("Core tests passed!\n");
    return 0;
}
