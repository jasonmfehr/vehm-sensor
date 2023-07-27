#include "blink.h"
#include "pico/cyw43_arch.h"

void blink(int count, int delay) {
  for(int i=0; i<count; i++){
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(delay);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    if (i < count-1) {
      sleep_ms(delay);
    }
  }

  sleep_ms(delay);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

void blink_error(int code) {
  while(true) {
      blink(code, 250);
      sleep_ms(2000);
    }
}

bool led_state(void) {
  return cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
}