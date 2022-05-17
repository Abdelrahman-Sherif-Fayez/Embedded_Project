#include "keypad.h"

static uint8_t KEYPAD_4x4_adjustKeyNumber(uint8_t button_number);

void keypad_Init(void)
{
  SYSCTL_RCGCGPIO_R |= 0x14;        //Enable clock to PORTC and PORTE  
  while ((SYSCTL_PRGPIO_R&0x14)==0);  //wait for clock to be settled
	GPIO_PORTC_LOCK_R = 0X4C4F434B;
  GPIO_PORTC_CR_R |= 0xF0;             //Allow settings for all pins of PORTC
	GPIO_PORTE_LOCK_R = 0X4C4F434B;
  GPIO_PORTE_CR_R |= 0x0F;             //Allow settings for all pins of PORTE
  GPIO_PORTE_DIR_R &= ~0x0F;             //PE0-PE3 are used with row and set them as digital input pins
  GPIO_PORTE_PDR_R |= 0x0F;             //Enable pull down resistor on PORTE
	GPIO_PORTC_DIR_R |= 0XF0;							//PC4-PC7 are used with columns and set them as digital output pins
  GPIO_PORTC_DEN_R |= 0xF0;             //Set PORTC as digital pins
  GPIO_PORTE_DEN_R |= 0x0F;             //Set PORTE as digital pins
}

uint8_t KEYPAD_getPressedKey(void){
	
	uint8_t col,row;
	uint8_t keypad_port_value = 0;
	while(1)
	{
		for(col=0;col<KEYPAD_NUM_COLS;col++) /* loop for columns */{
			/* Set the column output pin and clear the rest pins value */
			keypad_port_value = (1<<(KEYPAD_FIRST_COLUMN_PIN_ID+col));
			GPIO_PORTC_DATA_R = keypad_port_value;

			for(row=0;row<KEYPAD_NUM_ROWS;row++) /* loop for rows */
			{
				/* Check if the switch is pressed in this row */
				if( (GPIO_PORTE_DATA_R & 0x0F) & (1<<row))	{
						return KEYPAD_4x4_adjustKeyNumber((row*KEYPAD_NUM_COLS)+col+1);
				}
			}
		}
	}	
}

/*
 * Description :
 * Update the keypad pressed button value with the correct one in keypad 4x4 shape
 */
static uint8_t KEYPAD_4x4_adjustKeyNumber(uint8_t button_number){
	uint8_t keypad_button = 0;
	switch(button_number)
	{
		case 1: keypad_button = 1;
				break;
		case 2: keypad_button = 2;
				break;
		case 3: keypad_button = 3;
				break;
		case 4: keypad_button = 'A'; 
				break;
		case 5: keypad_button = 4;
				break;
		case 6: keypad_button = 5;
				break;
		case 7: keypad_button = 6;
				break;
		case 8: keypad_button = 'B'; 
				break;		
		case 9: keypad_button = 7;
				break;
		case 10: keypad_button = 8;
				break;
		case 11: keypad_button = 9;
				break;
		case 12: keypad_button = 'C';
				break;
		case 13: keypad_button = '*';
				break;			
		case 14: keypad_button = 0;
				break;
		case 15: keypad_button = '#'; 
				break;
		case 16: keypad_button = 'D'; 
				break;
		default: keypad_button = button_number;
				break;
	}
	return keypad_button;
}
