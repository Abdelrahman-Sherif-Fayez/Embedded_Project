#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "Io.h"


void RGB_LED_INIT(void);
void SW1_INIT(void);
void SW2_INIT(void);
void SW3_INIT(void);
uint8_t SW1_INPUT(void);
uint8_t SW2_INPUT(void);
uint8_t SW3_INPUT(void);
void RGB_OUTPUT(uint8_t led);
