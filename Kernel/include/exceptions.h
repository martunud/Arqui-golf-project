#include <stdint.h>
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_ID 6

void zero_division();
void invalidOperation();
void exceptionDispatcher(int exception, uint64_t exceptionRegisters[18]);
void printRegStatus(uint64_t exceptionRegisters[18]);

#endif