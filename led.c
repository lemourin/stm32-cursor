#include "led.h"

#include <stdbool.h>
#include <gpio.h>

#define RED_LED_GPIO GPIOA
#define GREEN_LED_GPIO GPIOA
#define BLUE_LED_GPIO GPIOB
#define GREEN2_LED_GPIO GPIOA
#define RED_LED_PIN 6
#define GREEN_LED_PIN 7
#define BLUE_LED_PIN 0
#define GREEN2_LED_PIN 5

bool led_state[GREEN_LED_PIN + 1];

void LEDon(GPIO_TypeDef *gpio, int pin) {
  gpio->BSRRH = 1 << pin;
  led_state[pin] = true;
}

void LEDoff(GPIO_TypeDef *gpio, int pin) {
  gpio->BSRRL = 1 << pin;
  led_state[pin] = false;
}

void LEDtoggle(GPIO_TypeDef *gpio, int pin) {
  if (led_state[pin])
    LEDoff(gpio, pin);
  else
    LEDon(gpio, pin);
}

void RedLEDon() { LEDon(RED_LED_GPIO, RED_LED_PIN); }

void RedLEDoff() { LEDoff(RED_LED_GPIO, RED_LED_PIN); }

void BlueLEDon() { LEDon(BLUE_LED_GPIO, BLUE_LED_PIN); }

void BlueLEDoff() { LEDoff(BLUE_LED_GPIO, BLUE_LED_PIN); }

void GreenLEDon() { LEDon(GREEN_LED_GPIO, GREEN_LED_PIN); }

void GreenLEDoff() { LEDoff(GREEN_LED_GPIO, GREEN_LED_PIN); }

void Green2LEDon() { LEDoff(GREEN2_LED_GPIO, GREEN2_LED_PIN); }

void Green2LEDoff() { LEDon(GREEN2_LED_GPIO, GREEN2_LED_PIN); }

void RedLEDtoggle() { LEDtoggle(RED_LED_GPIO, RED_LED_PIN); }

void BlueLEDtoggle() { LEDtoggle(BLUE_LED_GPIO, BLUE_LED_PIN); }

void GreenLEDtoggle() { LEDtoggle(GREEN_LED_GPIO, GREEN_LED_PIN); }

void Green2LEDtoggle() { LEDtoggle(GREEN2_LED_GPIO, GREEN2_LED_PIN); }

void initialize_led() {
  RedLEDoff();
  GreenLEDoff();
  BlueLEDoff();
  Green2LEDoff();

  GPIOafConfigure(GPIOA, 2, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_NOPULL,
                  GPIO_AF_USART2);

  GPIOafConfigure(GPIOA, 3, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_UP,
                  GPIO_AF_USART2);

  GPIOoutConfigure(RED_LED_GPIO, RED_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed,
                   GPIO_PuPd_NOPULL);

  GPIOoutConfigure(GREEN_LED_GPIO, GREEN_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed,
                   GPIO_PuPd_NOPULL);

  GPIOoutConfigure(BLUE_LED_GPIO, BLUE_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed,
                   GPIO_PuPd_NOPULL);

  GPIOoutConfigure(GREEN2_LED_GPIO, GREEN2_LED_PIN, GPIO_OType_PP,
                   GPIO_Low_Speed, GPIO_PuPd_NOPULL);
}
