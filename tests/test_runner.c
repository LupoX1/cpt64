/**
 * tests/test_runner.c - Test Runner per Klaus Dormann Test Suite
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c64.h"

#define TEST_MAX_CYCLES 100000000UL

// === Helper Functions ===

static bool load_test_binary(c64_system_t *c64, const char *path, uint16_t addr)
{
    printf("Loading test: %s at $%04X\n", path, addr);
    return c64_load_program(c64, path, addr);
}

static void print_cpu_state(c64_system_t *c64)
{
    c64_bus_t *bus = c64_get_bus(c64);
    cpu_t *cpu = bus_get_cpu(bus);
    
    cpu_state_t state;
    cpu_get_state(cpu, &state);
    
    printf("\nCPU State at failure:\n");
    printf("  PC: $%04X  SP: $%02X\n", state.pc, state.sp);
    printf("  A:  $%02X    X: $%02X    Y: $%02X\n", state.a, state.x, state.y);
    printf("  SR: $%02X [%c%c-%c%c%c%c%c]\n", 
           state.sr,
           (state.sr & FLAG_N) ? 'N' : 'n',
           (state.sr & FLAG_V) ? 'V' : 'v',
           (state.sr & FLAG_B) ? 'B' : 'b',
           (state.sr & FLAG_D) ? 'D' : 'd',
           (state.sr & FLAG_I) ? 'I' : 'i',
           (state.sr & FLAG_Z) ? 'Z' : 'z',
           (state.sr & FLAG_C) ? 'C' : 'c');
    printf("  Cycles: %lu\n", state.cycles);
    
    // Mostra memoria intorno a PC
    memory_t *ram = bus_get_ram(bus);
    printf("\nMemory at PC:\n  ");
    for (int i = 0; i < 16; i++) {
        printf("%02X ", ram[state.pc + i]);
    }
    printf("\n");
}

// === Functional Test ===

static bool run_functional_test(c64_system_t *c64)
{
    printf("\n=== Klaus Dormann 6502 Functional Test ===\n\n");
    
    if (!load_test_binary(c64, "tests/resources/6502_functional_test.bin", 0x0000)) {
        printf("ERROR: Failed to load test binary\n");
        return false;
    }
    
    // Imposta PC a $0400 (test entry point)
    c64_bus_t *bus = c64_get_bus(c64);
    cpu_t *cpu = bus_get_cpu(bus);
    cpu_state_t state;
    cpu_get_state(cpu, &state);
    state.pc = 0x0400;
    cpu_set_state(cpu, &state);
    
    printf("Starting test at PC=$%04X\n", 0x0400);
    printf("Running");
    fflush(stdout);
    
    uint16_t last_pc = 0;
    int stuck_count = 0;
    uint64_t start_cycles = c64_get_cycles(c64);
    
    while (1) {
        cpu_get_state(cpu, &state);
        uint16_t current_pc = state.pc;
        
        // Progress indicator
        if ((state.cycles - start_cycles) % 1000000 == 0) {
            printf(".");
            fflush(stdout);
        }
        
        // Detect infinite loop (test completion)
        if (current_pc == last_pc) {
            stuck_count++;
            if (stuck_count > 10) {
                printf("\n\n");
                
                // Success address: $3469
                if (current_pc == 0x3469) {
                    printf("✓✓✓ SUCCESS! ✓✓✓\n");
                    printf("All tests passed at PC=$%04X\n", current_pc);
                    printf("Total cycles: %lu\n", state.cycles - start_cycles);
                    return true;
                } else {
                    printf("✗✗✗ FAILURE ✗✗✗\n");
                    printf("Test trapped at PC=$%04X\n", current_pc);
                    printf("Expected success at $3469\n");
                    printf("Total cycles: %lu\n", state.cycles - start_cycles);
                    print_cpu_state(c64);
                    return false;
                }
            }
        } else {
            stuck_count = 0;
        }
        
        last_pc = current_pc;
        
        // Execute one instruction
        if (!c64_step(c64)) {
            printf("\n\nERROR: Execution failed at PC=$%04X\n", current_pc);
            print_cpu_state(c64);
            return false;
        }
        
        // Timeout check
        if ((state.cycles - start_cycles) > TEST_MAX_CYCLES) {
            printf("\n\nTIMEOUT: Test exceeded %lu cycles\n", TEST_MAX_CYCLES);
            printf("Stopped at PC=$%04X\n", current_pc);
            print_cpu_state(c64);
            return false;
        }
    }
}

// === Interrupt Test ===

static bool run_interrupt_test(c64_system_t *c64)
{
    printf("\n=== Klaus Dormann 6502 Interrupt Test ===\n\n");
    
    if (!load_test_binary(c64, "tests/resources/6502_interrupt_test.bin", 0x0000)) {
        printf("ERROR: Failed to load test binary\n");
        return false;
    }
    
    // Imposta PC a $0400
    c64_bus_t *bus = c64_get_bus(c64);
    cpu_t *cpu = bus_get_cpu(bus);
    cpu_state_t state;
    cpu_get_state(cpu, &state);
    state.pc = 0x0400;
    cpu_set_state(cpu, &state);
    
    printf("Starting interrupt test at PC=$%04X\n", 0x0400);
    printf("Running");
    fflush(stdout);
    
    uint16_t last_pc = 0;
    int stuck_count = 0;
    uint64_t start_cycles = c64_get_cycles(c64);
    
    while (1) {
        cpu_get_state(cpu, &state);
        uint16_t current_pc = state.pc;
        
        if ((state.cycles - start_cycles) % 1000000 == 0) {
            printf(".");
            fflush(stdout);
        }
        
        if (current_pc == last_pc) {
            stuck_count++;
            if (stuck_count > 10) {
                printf("\n\n");
                
                // Success address: $06F5
                if (current_pc == 0x06F5) {
                    printf("✓ Interrupt test PASSED\n");
                    printf("Total cycles: %lu\n", state.cycles - start_cycles);
                    return true;
                } else {
                    printf("✗ Interrupt test FAILED at PC=$%04X\n", current_pc);
                    printf("Total cycles: %lu\n", state.cycles - start_cycles);
                    print_cpu_state(c64);
                    return false;
                }
            }
        } else {
            stuck_count = 0;
        }
        
        last_pc = current_pc;
        
        if (!c64_step(c64)) {
            printf("\n\nERROR: Execution failed\n");
            print_cpu_state(c64);
            return false;
        }
        
        if ((state.cycles - start_cycles) > TEST_MAX_CYCLES / 2) {
            printf("\n\nTIMEOUT: Interrupt test exceeded cycles\n");
            print_cpu_state(c64);
            return false;
        }
    }
}

// === Main ===

int main(int argc, char **argv)
{
    printf("=== C64 Emulator Test Suite ===\n");
    printf("Using Klaus Dormann's 6502 test suite\n");
    
    // Crea sistema C64
    c64_system_t *c64 = c64_create();
    if (!c64) {
        fprintf(stderr, "Failed to create C64 system\n");
        return 1;
    }
    
    bool all_passed = true;
    
    // Scelta test
    if (argc > 1 && strcmp(argv[1], "--interrupt") == 0) {
        all_passed = run_interrupt_test(c64);
    } else {
        // Default: functional test completo
        all_passed = run_functional_test(c64);
    }
    
    // Dump finale
    printf("\nDumping final state to test_dump.txt...\n");
    c64_dump_state(c64, "test_dump.txt");
    
    // Cleanup
    c64_destroy(c64);
    
    printf("\n=== Test Suite %s ===\n", all_passed ? "PASSED" : "FAILED");
    return all_passed ? 0 : 1;
}