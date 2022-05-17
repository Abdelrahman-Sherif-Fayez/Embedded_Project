#include "lcd.h"

void LCD_init(void){
	
	SYSCTL_RCGCGPIO_R |=  0x02;  								// PortB clock enable
	while((SYSCTL_PRGPIO_R & 0x02) == 0 ){};
	GPIO_PORTB_LOCK_R = 0X4C4F434B;
	GPIO_PORTB_CR_R |= 0xFF;
	GPIO_PORTB_AFSEL_R &= ~0xFF;
	GPIO_PORTB_PCTL_R &= ~0xFFFFFFFF;
	GPIO_PORTB_AMSEL_R &= ~0xFF;
	GPIO_PORTB_DEN_R |= 0xFF;
	GPIO_PORTB_DIR_R |= 0xFF;
	GPIO_PORTB_DATA_R &= ~0xFF;
	SysTick_Init();
	
	LCD_sendCommand(Set5x7FontSize);  /* select 5x7 font size and 2 rows of LCD */
  LCD_sendCommand(Function_set_4bit); /* Select 4-bit Mode of LCD */
  LCD_sendCommand(moveCursorRight); /* shift cursor right */
  LCD_sendCommand(clear_display); /* clear whatever is written on display */
  LCD_sendCommand(cursorBlink);  /* Enable Display and cursor blinking */
		
}


void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_RELOAD_R= 0x00FFFFFF;
	NVIC_ST_CURRENT_R =0;
	NVIC_ST_CTRL_R =0x05;
}

void SysTick_wait(uint32_t delay){
	NVIC_ST_RELOAD_R = delay-1;
	NVIC_ST_CURRENT_R =0;
 	while((NVIC_ST_CTRL_R & 0x00010000)==0){};
}

void genericDelay(uint32_t n){
	uint32_t i;
	for(i=0; i<n; i++){
		SysTick_wait(16000);  //1ms
	}
}


void LCD_Write_Nibble(uint8_t data, uint8_t control)
{
    data &= 0xF0;       /* Extract upper nibble for data */
    control &= 0x0F;    /* Extract lower nibble for control */
    GPIO_PORTB_DATA_R = data | control;       /* Set RS and R/W to zero for write operation */
    GPIO_PORTB_DATA_R = data | control | EN;  /* Provide Pulse to Enable pin to perform wite operation */
    genericDelay(1); // 1 ms 
    GPIO_PORTB_DATA_R = data; /*Send data */
    GPIO_PORTB_DATA_R = 0;	 /* stop writing data to LCD */
}



void LCD_sendCommand(uint8_t command){
	
	LCD_Write_Nibble((command & 0xF0) , 0);
	LCD_Write_Nibble((command<<4), 0);
	
	if (command < 4)
        genericDelay(2);          /* 2ms delay for commands 1 and 2 */
    else
        genericDelay(1);         /* 1ms delay for other commands */
}

void LCD_displayCharacter(uint8_t data)
{
    LCD_Write_Nibble(data & 0xF0, 1);    /* Write upper nibble to LCD and RS = 1 to write data */
    LCD_Write_Nibble(data << 4, 1);      /* Write lower nibble to LCD and RS = 1 to write data */
    genericDelay(1);
}
	

void LCD_displayString (const uint8_t *str)	/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)  /* Send each char of string till the NULL */
	{
		LCD_displayCharacter(str[i]);  /* Call LCD data write */
	}
}

void LCD_moveCursor(uint8_t row,uint8_t col)
{
	uint8_t lcd_memory_address;
	
	/* Calculate the required address in the LCD DDRAM */
	switch(row)
	{
		case 0:
			lcd_memory_address=col;
				break;
		case 1:
			lcd_memory_address=col+0x40;
				break;
	}					
	/* Move the LCD cursor to this specific address */
	LCD_sendCommand(lcd_memory_address | FirstRow);
}

void LCD_displayStringRowColumn(uint8_t row,uint8_t col,const uint8_t *Str)
{
	LCD_moveCursor(row,col); /* go to to the required LCD position */
	LCD_displayString(Str); /* display the string */
}

	
/*
 * Description :
 * Display the required decimal value on the screen
 */
void LCD_intgerToString(int data){
  int  i ,j;
  uint8_t buff[16], t;
	if(data==0){
		buff[0] = (data %10) +'0';
		buff[1] = '\0';
	}
	else	{
  for (i=0 ; data >0 ; i++){
  buff[i] = (data %10) +'0';
  data = data /10;
  }
	buff[i] = '\0';

	
  for (j=0,i-- ; j<i ; j++,i--)
  {
  t= buff[i] ;
  buff[i] = buff[j] ;
  buff[j] = t;
  
  }
}
   LCD_displayString(buff); /* Display the string */
}

/*
 * Description :
 * Send the clear screen command
 */
void LCD_clearScreen(void)
{
	LCD_sendCommand(clear_display); /* Send clear display command */
}
