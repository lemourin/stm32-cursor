#include "accelerometer.h"

#include <gpio.h>
#include <stdio.h>
#include <stm32.h>

#include "utility.h"

#define LIS35DE_ADDR 0x1d
#define I2C_SPEED_HZ 100000
#define PCLK1_MHZ 16

int8_t read_accelerometer_register(int reg) {
  I2C1->CR1 |= I2C_CR1_START;
  wait_until(I2C1->SR1 & I2C_SR1_SB);
  I2C1->DR = LIS35DE_ADDR << 1;
  wait_until(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1->SR2;
  I2C1->DR = reg;
  wait_until(I2C1->SR1 & I2C_SR1_BTF);
  I2C1->CR1 |= I2C_CR1_START;
  wait_until(I2C1->SR1 & I2C_SR1_SB);
  I2C1->DR = (LIS35DE_ADDR << 1) | 1U;
  I2C1->CR1 &= ~I2C_CR1_ACK;
  wait_until(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1->SR2;
  I2C1->CR1 |= I2C_CR1_STOP;
  wait_until(I2C1->SR1 & I2C_SR1_RXNE);
  return I2C1->DR;
}

void write_accelerometer_register(int reg, int value) {
  I2C1->CR1 |= I2C_CR1_START;
  wait_until(I2C1->SR1 & I2C_SR1_SB);
  I2C1->DR = LIS35DE_ADDR << 1;
  wait_until(I2C1->SR1 & I2C_SR1_ADDR);
  I2C1->SR2;
  I2C1->DR = reg;
  wait_until(I2C1->SR1 & I2C_SR1_TXE);
  I2C1->DR = value;
  wait_until(I2C1->SR1 & I2C_SR1_BTF);
  I2C1->CR1 |= I2C_CR1_STOP;
}

void write_accelerometer_register_async(int reg, int value,
                                        void (*callback)()) {
  if (accelerometer.pending_) {
    // print_stm("[FAIL] can't write, other operation pending\r\n");
    return;
  }
  accelerometer.current_register_ = reg;
  accelerometer.current_write_value_ = value;
  accelerometer.write_value_sent_ = false;
  accelerometer.write_ = true;
  accelerometer.pending_ = true;
  accelerometer.callback_write_ = callback;
  I2C1->CR1 |= I2C_CR1_START;
}

void read_accelerometer_register_async(int reg, void (*callback)(uint8_t)) {
  if (accelerometer.pending_) {
    // print_stm("[FAIL] can't read, other operation pending\r\n");
    return;
  }
  accelerometer.current_register_ = reg;
  accelerometer.repeated_start_ = false;
  accelerometer.write_ = false;
  accelerometer.pending_ = true;
  accelerometer.callback_read_ = callback;
  I2C1->CR1 |= I2C_CR1_START;
}

void initialize_accelerometer_aux_4() {
  print_stm("accelerometer initialized\r\n");
}

void initialize_accelerometer_aux_3() {
  write_accelerometer_register_async(0x22, 0b00000100,
                                     initialize_accelerometer_aux_4);
}

void initialize_accelerometer_aux_2() {
  GPIOinConfigure(GPIOA, 1, GPIO_PuPd_NOPULL, EXTI_Mode_Interrupt,
                  EXTI_Trigger_Rising_Falling);
  GPIOinConfigure(GPIOA, 8, GPIO_PuPd_NOPULL, EXTI_Mode_Interrupt,
                  EXTI_Trigger_Rising_Falling);
  write_accelerometer_register_async(0x20, 0b01000111,
                                     initialize_accelerometer_aux_3);
}

void initialize_accelerometer_aux_1() {
  write_accelerometer_register_async(0x22, 0, initialize_accelerometer_aux_2);
}

void initialize_accelerometer() {
  GPIOafConfigure(GPIOB, 8, GPIO_OType_OD, GPIO_Low_Speed, GPIO_PuPd_NOPULL,
                  GPIO_AF_I2C1);
  GPIOafConfigure(GPIOB, 9, GPIO_OType_OD, GPIO_Low_Speed, GPIO_PuPd_NOPULL,
                  GPIO_AF_I2C1);

  I2C1->CR1 = 0;

  I2C1->CR2 = PCLK1_MHZ;
  I2C1->CCR = (PCLK1_MHZ * 1000000) / (I2C_SPEED_HZ << 1);
  I2C1->TRISE = PCLK1_MHZ + 1;
  I2C1->CR1 |= I2C_CR1_PE;
  I2C1->CR2 |= I2C_CR2_ITBUFEN | I2C_CR2_ITERREN | I2C_CR2_ITEVTEN;
  write_accelerometer_register_async(0x20, 0, initialize_accelerometer_aux_1);
}

void read_accelelerometer_register_irq() {
  if (!accelerometer.repeated_start_) {
    if (I2C1->SR1 & I2C_SR1_SB) {
      I2C1->DR = LIS35DE_ADDR << 1;
    } else if (I2C1->SR1 & I2C_SR1_ADDR) {
      I2C1->SR2;
      I2C1->DR = accelerometer.current_register_;
    } else if (I2C1->SR1 & I2C_SR1_BTF) {
      accelerometer.repeated_start_ = true;
      I2C1->CR1 |= I2C_CR1_START;
    }
  } else {
    if (I2C1->SR1 & I2C_SR1_SB) {
      I2C1->DR = (LIS35DE_ADDR << 1) | 1U;
      I2C1->CR1 &= ~I2C_CR1_ACK;
    } else if (I2C1->SR1 & I2C_SR1_ADDR) {
      I2C1->SR2;
      I2C1->CR1 |= I2C_CR1_STOP;
    } else if (I2C1->SR1 & I2C_SR1_RXNE) {
      accelerometer.pending_ = false;
      accelerometer.callback_read_(I2C1->DR);
    }
  }
}

void write_accelerometer_register_irq() {
  if (I2C1->SR1 & I2C_SR1_SB) {
    I2C1->DR = LIS35DE_ADDR << 1;
  } else if (I2C1->SR1 & I2C_SR1_ADDR) {
    I2C1->SR2;
    I2C1->DR = accelerometer.current_register_;
  } else if ((I2C1->SR1 & I2C_SR1_TXE) && !accelerometer.write_value_sent_) {
    I2C1->DR = accelerometer.current_write_value_;
    accelerometer.write_value_sent_ = true;
  } else if (I2C1->SR1 & I2C_SR1_BTF) {
    I2C1->CR1 |= I2C_CR1_STOP;
    accelerometer.pending_ = false;
    accelerometer.callback_write_();
  }
}

void read_accelerometer_state_aux_3(uint8_t value) {
  accelerometer.prev_z_ = accelerometer.z_;
  accelerometer.z_ = value;
}

void read_accelerometer_state_aux_2(uint8_t value) {
  accelerometer.prev_y_ = accelerometer.y_;
  accelerometer.y_ = value;
  read_accelerometer_register_async(0x2d, read_accelerometer_state_aux_3);
}

void read_accelerometer_state_aux_1(uint8_t value) {
  accelerometer.prev_x_ = accelerometer.x_;
  accelerometer.x_ = value;
  read_accelerometer_register_async(0x2b, read_accelerometer_state_aux_2);
}

void read_accelerometer_state() {
  read_accelerometer_register_async(0x29, read_accelerometer_state_aux_1);
}
