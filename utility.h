#ifndef UTILITY_H
#define UTILITY_H

#include <stm32.h>
#include <stdio.h>

#define HSI_HZ 16000000U
#define PCLK1_HZ HSI_HZ

#define wait_until(expr) \
  while (!(expr))        \
    ;

#define print_stm(...)                   \
  {                                      \
    sprintf(string_buffer, __VA_ARGS__); \
    print(string_buffer);                \
    push_byte();                         \
  }

#define BUFFER_SIZE 1024

char string_buffer[BUFFER_SIZE];

void print(const char*);
void push_byte();

void initialize();

#endif  // UTILITY_H
