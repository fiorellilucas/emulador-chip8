#include "memory.h"

Memory::Memory() {
    for (uint16_t mem_index = 0; mem_index < 80; mem_index++) {
        memory[mem_index] = fontset[mem_index];
    }
}