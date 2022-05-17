#include "tm4c123gh6pm.h"
#include <stdint.h>

#define KEYPAD_NUM_COLS 4
#define KEYPAD_FIRST_COLUMN_PIN_ID 4
#define KEYPAD_NUM_ROWS 4

void keypad_Init(void);
uint8_t KEYPAD_getPressedKey(void);
void genericDelay(uint32_t n);
void SysTick_Init(void);
void SysTick_wait(uint32_t delay);
