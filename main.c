#include <delay.h>
#include <errno.h>
#include <gpio.h>
#include <irq.h>
#include <lcd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stm32.h>
#include <string.h>
#include <unistd.h>

#include "accelerometer.h"
#include "button.h"
#include "lcd_util.h"
#include "utility.h"

void DMA1_Stream6_IRQHandler() {
  uint32_t isr = DMA1->HISR;
  if (isr & DMA_HISR_TCIF6) {
    DMA1->HIFCR = DMA_HIFCR_CTCIF6;
    push_byte();
  }
}

void EXTI0_IRQHandler() {
  EXTI->PR = EXTI_PR_PR0;
  print_state(BUTTON_MODE);
}

void EXTI1_IRQHandler() {
  EXTI->PR = EXTI_PR_PR1;
  read_accelerometer_state();
  lcd_move(accelerometer.x_ / 5, accelerometer.y_ / 5);
}

void EXTI2_IRQHandler() { EXTI->PR = EXTI_PR_PR2; }

void EXTI3_IRQHandler() {
  EXTI->PR = EXTI_PR_PR3;
  print_state(BUTTON_LEFT);
}

void EXTI4_IRQHandler() {
  EXTI->PR = EXTI_PR_PR4;
  print_state(BUTTON_RIGHT);
}

void EXTI9_5_IRQHandler() {
  if (EXTI->PR & EXTI_PR_PR5) print_state(BUTTON_UP);
  if (EXTI->PR & EXTI_PR_PR6) print_state(BUTTON_DOWN);
  EXTI->PR = EXTI_PR_PR5 | EXTI_PR_PR6;
}

void EXTI15_10_IRQHandler() {
  if (EXTI->PR & EXTI_PR_PR10) print_state(BUTTON_ACTION);
  if (EXTI->PR & EXTI_PR_PR13) print_state(BUTTON_USER);
  EXTI->PR = EXTI_PR_PR10 | EXTI_PR_PR13;
}

void I2C1_EV_IRQHandler() {
  if (accelerometer.write_)
    write_accelerometer_register_irq();
  else
    read_accelelerometer_register_irq();
}

void I2C1_ER_IRQHandler() { print_stm("error %lx\r\n", I2C1->SR1); }

int main() {
  initialize();
  print_stm("initialized\r\n");

  int current_x = 0, current_y = 0;
  while (true) {
    lcd_update(&current_x, &current_y);
  }
}
