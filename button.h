#ifndef BUTTON_H
#define BUTTON_H

#include <gpio.h>

#define BUTTON_COUNT 7
#define BUTTON_USER (button)
#define BUTTON_LEFT (button + 1)
#define BUTTON_RIGHT (button + 2)
#define BUTTON_UP (button + 3)
#define BUTTON_DOWN (button + 4)
#define BUTTON_ACTION (button + 5)
#define BUTTON_MODE (button + 6)

struct button {
  GPIO_TypeDef *gpio_;
  int bit_;
  int target_;
  const char *name_;
} button[BUTTON_COUNT];

void initialize_buttons();
void print_state(struct button *button);

#endif  // BUTTON_H
