#include "button.h"

#include "utility.h"

struct button create_button(GPIO_TypeDef *gpio, int bit, int target,
                            const char *name) {
  struct button b;
  b.gpio_ = gpio;
  b.bit_ = bit;
  b.target_ = target;
  b.name_ = name;
  GPIOinConfigure(gpio, bit, GPIO_PuPd_UP, EXTI_Mode_Interrupt,
                  EXTI_Trigger_Rising_Falling);
  return b;
}

void initialize_buttons() {
  int idx = 0;
  button[idx++] = create_button(GPIOC, 13, 0, "USER");
  button[idx++] = create_button(GPIOB, 3, 0, "LEFT");
  button[idx++] = create_button(GPIOB, 4, 0, "RIGHT");
  button[idx++] = create_button(GPIOB, 5, 0, "UP");
  button[idx++] = create_button(GPIOB, 6, 0, "DOWN");
  button[idx++] = create_button(GPIOB, 10, 0, "ACTION");
  button[idx++] = create_button(GPIOA, 0, 1, "MODE");
}

void print_state(struct button *button) {
  print_stm("%s %s\r\n", button->name_,
            (button->gpio_->IDR & (1 << button->bit_)) == button->target_
                ? "PRESSED"
                : "RELEASED");
}

