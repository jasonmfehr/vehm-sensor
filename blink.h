#ifndef __blink
#define __blink

#define LED_ON  "on"
#define LED_OFF "off"

void blink(int count, int delay);
void blink_error(int code);
bool led_state(void);

#endif