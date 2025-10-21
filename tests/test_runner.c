#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "c64.h"

// Assumo che tu abbia questi nel tuo emulatore
// Adatta i nomi alle tue strutture
/*
typedef struct {
    // CPU registers
    uint16_t pc;
    uint8_t sp;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sr;  // status register
    
    // Memory
    uint8_t *memory;
    
    // Cycle counter
    uint64_t cycles;
} Core;

void cpu_step(Core *core);
void reset_core(Core *core);
*/

// Carica un file binario in memoria
/*
bool load_test_binary(C64_Core *core, const char *filename, uint16_t addr) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", filename);
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    size_t read = fread(&core->ram[addr], 1, size, f);
    fclose(f);
    
    printf("Loaded %ld bytes at $%04X\n", read, addr);
    return read == size;
}

// Esegue il Klaus Dormann functional test
void run_functional_test(C64_Core *core) {
    printf("\n=== Klaus Dormann 6502 Functional Test ===\n\n");
    
    // Carica il test binary
    if (!load_test_binary(core, "tests/resources/6502_functional_test.bin", 0x0000)) {
        printf("Failed to load test binary!\n");
        return;
    }
    
    // Il test inizia a $0400
    write_program_counter(core->cpu, 0x0400);
    
    uint16_t last_pc = 0;
    int stuck_count = 0;
    uint64_t max_cycles = 100000000;  // 100M cicli = timeout
    uint64_t start_cycles = read_cycles(core->cpu);
    
    printf("Starting test at PC=$%04X\n", read_program_counter(core->cpu));
    printf("Running");
    fflush(stdout);
    
    //log_cpu(core->cpu, core->ram);

    while (1) {
        uint16_t current_pc = read_program_counter(core->cpu);
        
        // Progress indicator ogni milione di cicli
        if ((read_cycles(core->cpu) - start_cycles) % 1000000 == 0) {
            printf(".");
            fflush(stdout);
        }
        
        // Il test entra in un loop infinito quando passa
        // (fa un JMP a se stesso)
        if (current_pc == last_pc) {
            stuck_count++;
            if (stuck_count > 10) {
                printf("\n\n");
                
                // Controlla dove si è fermato
                if (current_pc == 0x3469) {
                    printf("✓✓✓ SUCCESS! ✓✓✓\n");
                    printf("All tests passed at PC=$%04X\n", current_pc);
                    printf("Total cycles: %lu\n", read_cycles(core->cpu) - start_cycles);
                } else {
                    printf("✗✗✗ FAILURE ✗✗✗\n");
                    printf("Total cycles: %lu\n", read_cycles(core->cpu) - start_cycles);
                    printf("Test trapped at PC=$%04X\n", current_pc);
                    printf("This usually means an instruction failed\n");
                    printf("\nCPU State:\n");
                    printf("  A=$%02X X=$%02X Y=$%02X SP=$%02X\n", 
                           read_accumulator(core->cpu), read_xr(core->cpu), read_yr(core->cpu), read_sp(core->cpu));
                    printf("  SR=%c%c%c%c%c%c%c%c\n",
                           (read_sr(core->cpu) & 0x80) ? 'N' : 'n',
                           (read_sr(core->cpu) & 0x40) ? 'V' : 'v',
                           (read_sr(core->cpu) & 0x20) ? '1' : '0',
                           (read_sr(core->cpu) & 0x10) ? 'B' : 'b',
                           (read_sr(core->cpu) & 0x08) ? 'D' : 'd',
                           (read_sr(core->cpu) & 0x04) ? 'I' : 'i',
                           (read_sr(core->cpu) & 0x02) ? 'Z' : 'z',
                           (read_sr(core->cpu) & 0x01) ? 'C' : 'c');
                    printf("\nMemory around PC:\n");
                    printf("  $%04X:", current_pc);
                    for (int i = 0; i < 8; i++) {
                        printf(" %02X", core->ram[current_pc + i]);
                    }
                    printf("\n");
                    
                    // Suggerimenti basati sull'indirizzo
                    if (current_pc < 0x0500) {
                        printf("\nFailed early in test - likely basic instruction problem\n");
                    }
                }
                break;
            }
        } else {
            stuck_count = 0;
        }
        
        last_pc = current_pc;
        
        // Esegui un'istruzione
        c64_core_step(core);
        //if(read_cycles(core->cpu) - start_cycles > 84000000)
        //log_cpu(core->cpu, core->ram);
        
        // Timeout check
        if (read_cycles(core->cpu) - start_cycles > max_cycles) {
            printf("\n✗ Test TIMEOUT after %lu cycles at PC=$%04X\n", 
                   max_cycles, current_pc);
            printf("Possible infinite loop or very slow implementation\n");
            break;
        }
    }
}

// Test degli interrupt
void run_interrupt_test(C64_Core *core) {
    printf("\n=== Klaus Dormann 6502 Interrupt Test ===\n\n");
    
    if (!load_test_binary(core, "tests/bin_files/6502_interrupt_test.bin", 0x0000)) {
        printf("Failed to load interrupt test binary!\n");
        return;
    }
    
    // Il test interrupt inizia a $0400
    write_program_counter(core->cpu , 0x0400);
    
    uint16_t last_pc = 0;
    int stuck_count = 0;
    uint64_t max_cycles = 50000000;
    uint64_t start_cycles = read_cycles(core->cpu);
    
    printf("Starting interrupt test at PC=$%04X\n", read_program_counter(core->cpu));
    printf("Running");
    fflush(stdout);
    
    while (1) {
        uint16_t current_pc = read_program_counter(core->cpu);
        
        if ((read_cycles(core->cpu) - start_cycles) % 1000000 == 0) {
            printf(".");
            fflush(stdout);
        }
        
        if (current_pc == last_pc) {
            stuck_count++;
            if (stuck_count > 10) {
                printf("\n\n");
                if (current_pc == 0x06F5) {  // Success address per interrupt test
                    printf("✓ Interrupt test PASSED at PC=$%04X\n", current_pc);
                } else {
                    printf("✗ Interrupt test FAILED at PC=$%04X\n", current_pc);
                }
                break;
            }
        } else {
            stuck_count = 0;
        }
        
        last_pc = current_pc;
        c64_core_step(core);
        
        if (read_cycles(core->cpu) - start_cycles > max_cycles) {
            printf("\n✗ Interrupt test TIMEOUT\n");
            break;
        }
    }
}

// Main per test standalone
int main(int argc, char **argv) {
    C64_Core *core = c64_core_create();
    
    // Inizializza il core
    //c64_core_reset(core);
    
    if (argc > 1 && strcmp(argv[1], "--interrupt") == 0) {
        run_interrupt_test(core);
    } else {
        // Test completo di default
        run_functional_test(core);
    }
    
    dump_data(core);

    return 0;
}

*/

void main(void)
{}