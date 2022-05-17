#include <stdint.h>
#include "tm4c123gh6pm.h"
#include <stdlib.h>

#define EN 0x04    // PortB PB2 Mask

/* LCD commands */
#define clear_display     0x01
#define returnHome        0x02
#define moveCursorRight   0x06
#define moveCursorLeft    0x08
#define shiftDisplayRight 0x1C
#define shiftDisplayLeft  0x18
#define cursorBlink       0x0F
#define cursorOff         0x0C
#define cursorOn          0x0E
#define Function_set_4bit 0x28
#define Function_set_8bit 0x38
#define Entry_mode        0x06
#define Function_8_bit    0x32
#define Set5x7FontSize    0x20
#define FirstRow          0x80

void LCD_init(void);                   /* LCD initialization function */
void LCD_sendCommand(uint8_t command); /*Used to send commands to LCD */
void LCD_displayString(const uint8_t *Str);     /* Send string to LCD function */
void LCD_displayCharacter(uint8_t data);           /* Writes ASCII character */
void LCD_Write_Nibble(uint8_t data, uint8_t control); /* Writes 4-bits */
void LCD_moveCursor(uint8_t row,uint8_t col);
void LCD_displayStringRowColumn(uint8_t row,uint8_t col,const uint8_t *Str);
void LCD_intgerToString(int data);
void LCD_clearScreen(void);

void genericDelay(uint32_t n);
void SysTick_Init(void);
void SysTick_wait(uint32_t delay);
