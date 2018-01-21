#include "utility.h"

#include <gpio.h>
#include <irq.h>
#include <stdbool.h>
#include <string.h>

#include "accelerometer.h"
#include "button.h"
#include "lcd.h"
#include "lcd_util.h"
#include "led.h"

#define USART_Mode_Rx_Tx (USART_CR1_RE | USART_CR1_TE)
#define USART_Enable USART_CR1_UE
#define USART_WordLength_8b 0x0000
#define USART_WordLength_9b USART_CR1_M
#define USART_Parity_No 0x0000
#define USART_Parity_Even USART_CR1_PCE
#define USART_Parity_Odd (USART_CR1_PCE | USART_CR1_PS)

#define USART_StopBits_1 0x0000
#define USART_StopBits_0_5 0x1000
#define USART_StopBits_2 0x2000
#define USART_StopBits_1_5 0x3000

#define USART_FlowControl_None 0x0000
#define USART_FlowControl_RTS USART_CR3_RTSE
#define USART_FlowControl_CTS USART_CR3_CTSE

char buffer_write[BUFFER_SIZE];
int buffer_write_size;
int buffer_write_position;

void print(const char *ptr) {
  int p = IRQprotectAll();
  if (buffer_write_size + strlen(ptr) < BUFFER_SIZE)
    while (*ptr && buffer_write_size + 1 < BUFFER_SIZE) {
      buffer_write[buffer_write_position] = *ptr++;
      buffer_write_position = (buffer_write_position + 1) % BUFFER_SIZE;
      buffer_write_size++;
    }
  IRQunprotectAll(p);
}

void push_byte() {
  if (!(DMA1_Stream6->CR & DMA_SxCR_EN) && buffer_write_size > 0) {
    char *start =
        buffer_write +
        (buffer_write_position - buffer_write_size + BUFFER_SIZE) % BUFFER_SIZE;
    char *tmp = buffer_write + buffer_write_position;
    char *end = tmp >= start ? tmp : buffer_write + BUFFER_SIZE;
    buffer_write_size -= end - start;
    DMA1_Stream6->M0AR = (uint32_t)start;
    DMA1_Stream6->NDTR = end - start;
    DMA1_Stream6->CR |= DMA_SxCR_EN;
  }
}

void initialize() {
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                  RCC_AHB1ENR_GPIOCEN | RCC_APB1ENR_USART2EN |
                  RCC_APB1ENR_TIM2EN | RCC_AHB1ENR_DMA1EN;
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN | RCC_APB1ENR_I2C1EN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  USART2->CR1 = USART_Mode_Rx_Tx | USART_WordLength_8b | USART_Parity_No |
                USART_Enable | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  USART2->CR2 = USART_StopBits_1;
  USART2->CR3 = USART_FlowControl_None | USART_CR3_DMAT | USART_CR3_DMAR;

  uint32_t const baudrate = 9600U;
  USART2->BRR = (PCLK1_HZ + (baudrate / 2U)) / baudrate;

  DMA1_Stream6->CR =
      4U << 25 | DMA_SxCR_PL_1 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
  DMA1_Stream6->PAR = (uint32_t)&USART2->DR;
  DMA1_Stream5->CR = 4U << 25 | DMA_SxCR_PL_1 | DMA_SxCR_MINC | DMA_SxCR_TCIE;
  DMA1_Stream5->PAR = (uint32_t)&USART2->DR;
  DMA1->HIFCR = DMA_HIFCR_CTCIF6 | DMA_HIFCR_CTCIF5;

  IRQsetPriority(DMA1_Stream6_IRQn, 0, 0);
  IRQsetPriority(EXTI0_IRQn, 0, 0);
  IRQsetPriority(EXTI1_IRQn, 0, 0);
  IRQsetPriority(EXTI2_IRQn, 0, 0);
  IRQsetPriority(EXTI3_IRQn, 0, 0);
  IRQsetPriority(EXTI4_IRQn, 0, 0);
  IRQsetPriority(EXTI9_5_IRQn, 0, 0);
  IRQsetPriority(EXTI15_10_IRQn, 0, 0);

  NVIC_EnableIRQ(DMA1_Stream6_IRQn);
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_EnableIRQ(EXTI2_IRQn);
  NVIC_EnableIRQ(EXTI3_IRQn);
  NVIC_EnableIRQ(EXTI4_IRQn);
  NVIC_EnableIRQ(EXTI9_5_IRQn);
  NVIC_EnableIRQ(EXTI15_10_IRQn);
  NVIC_EnableIRQ(I2C1_EV_IRQn);
  NVIC_EnableIRQ(I2C1_ER_IRQn);

  initialize_buttons();
  initialize_led();
  initialize_lcd();
  initialize_accelerometer();
}
