#include "lcd.h"
#include "keypad.h"
#include "stdbool.h"
#include "Input_Output.h"


static uint8_t get_Operation(void);
static void ShowTimeDecreasing(uint32_t time);
static uint8_t get_Kilos(uint8_t type);
static void Kilos_Display(uint8_t kilos);
static uint32_t get_cookingTime(void);
static bool Check_Time(uint32_t Total_Seconds);
static void Check_startFlag(void);



uint8_t Start_flag=0; // flag to start counting 
uint8_t  entering_flag = 0; // flag shows that we inside ShowTimeDecreasing function
uint8_t Stop_flag = 0;

void SW_1_2_interruptInit(void);

void main(){
	uint8_t key;
	uint8_t kilos;
	uint32_t time_seconds;
	RGB_LED_INIT();
	SW1_INIT();
	SW2_INIT();
	LCD_init();
	keypad_Init();
  SW_1_2_interruptInit();
	LCD_sendCommand(clear_display);
	LCD_sendCommand(FirstRow);
	genericDelay(500);       // 500ms	

	while(1){
		key = get_Operation();
		switch(key){
			case 'A':
				Check_startFlag();
				LCD_displayString("PopCorn");
				ShowTimeDecreasing(60);
				break;
			case 'B':
				kilos = get_Kilos('B');
				Kilos_Display(kilos);
		   	Check_startFlag();
		 		ShowTimeDecreasing(kilos*30);       // Rate of 0.5mins
				LCD_sendCommand(FirstRow);
				LCD_displayString("Beef Defrosted");
				genericDelay(2000);
				break;
				case 'C':
				kilos = get_Kilos('C');
				Kilos_Display(kilos);
			  Check_startFlag();
				ShowTimeDecreasing(kilos*12);    // Rate of 0.2 mins
				LCD_sendCommand(FirstRow);
				LCD_displayString("ChickenDefrosted");
				genericDelay(2000);
				break;
				case 'D':
				time_seconds = get_cookingTime();
				Check_startFlag();
				ShowTimeDecreasing(kilos*12);
				LCD_sendCommand(clear_display);
				LCD_displayString("Cooked");
				genericDelay(2000);
				break;
		}
		Start_flag=0;
	}

}

static uint8_t get_Operation(void){
		uint8_t key;
	while(1){
		LCD_sendCommand(clear_display);
		LCD_displayString("Please Enter");
		LCD_displayStringRowColumn(1,0,"A,B,C or D: ");
		key = KEYPAD_getPressedKey();
		if(key != 'A' && key != 'B' && key != 'C' && key != 'D'){
			LCD_sendCommand(clear_display);
			LCD_displayString("Err. try again");
			genericDelay(2000); // 2 sec
			continue;
		}
		return key;
	}
}


static void ShowTimeDecreasing(uint32_t time){  // Total time in sec
int i,j;
	uint32_t min, sec;
	min = time/60;
	sec = time%60;
		LCD_moveCursor(1,0);
	  LCD_displayStringRowColumn(1,5,":");
	  entering_flag = 1;
		for(i=min; i>=0; i--){       
		LCD_moveCursor(1,3);
		LCD_intgerToString(i/10);
		LCD_moveCursor(1,4);
		LCD_intgerToString(i%10);
			
			for(j=sec; j>=0; j--){ 
				LCD_moveCursor(1,6);     //00:00
				LCD_intgerToString(j/10);
				LCD_moveCursor(1,7);
				LCD_intgerToString(j%10);
				genericDelay(1000);          //1 SEC
	}
			sec= 59;
}
		entering_flag = 0;
}

static uint8_t get_Kilos(uint8_t type){
		uint8_t kilo;
		
	while(1){
		
			if( type =='B'){
			LCD_displayString("Beef weight?");	
				}
			else if(type =='C'){
					LCD_displayString("Chicken weight?");
					}
				LCD_moveCursor(1,0);
				LCD_displayString("Enter 1->9: ");
				genericDelay(500);  // 500ms 
				kilo = KEYPAD_getPressedKey();
			if((kilo >=1) && (kilo <=9)){
				return kilo;
		}
			else{
			LCD_sendCommand(clear_display);
			LCD_displayString("Err.");
			genericDelay(2000); // 2 sec
			LCD_sendCommand(clear_display);
			}
			
	}
}

static void Kilos_Display(uint8_t kilos){
	
	LCD_sendCommand(clear_display);
	LCD_intgerToString(kilos);
	LCD_displayString(" Kg");
	genericDelay(2000); // 2 sec
	LCD_sendCommand(clear_display);
}

static uint32_t get_cookingTime(void){
	uint8_t Time[4] = {0,0,0,0};
	uint8_t counter,cursor,i;
	uint32_t Time_Seconds=0;
	
	while(1){
	LCD_displayString("Cooking time?");
	LCD_displayStringRowColumn(1,3,"00:00");
	genericDelay(500); 
	for (counter=7 ; counter >3; counter--){
		
		while(1){ // Handling Non-integer numbers
			Time[counter-4] = KEYPAD_getPressedKey();
			if((Time[counter-4] <=9 )&& ((Time[counter-4] >=0)) ){
				break;
			}
		}
		
		for (i=0,cursor = 7; cursor >=counter ; cursor--,i++){
			if(cursor ==5 || cursor ==4){5
				cursor--;
		}
			LCD_moveCursor(1,cursor);
			LCD_intgerToString(Time[counter-4+i]);
			genericDelay(250);
			if(cursor ==4){
				cursor++;
			}
		}
	}
	LCD_moveCursor(1,8);
	genericDelay(1000);   // 1sec
	Time_Seconds = (Time[3]*600) + (Time[2]*60) + (Time[1]*10) + Time[0];
	if(Check_Time(Time_Seconds) ==false){
			LCD_sendCommand(clear_display);
			LCD_displayString("Invalid input");
			LCD_displayStringRowColumn(1,0,"Try Again");
			genericDelay(2000);  // 2sec
			LCD_sendCommand(clear_display);
			continue;
		}
	else{
		   NVIC->ISER[0] & =~(1<<30); // because i need pooling method 
			while( SW1_INPUT() != 0  && SW2_INPUT() != 0);
	       	if(  SW1_INPUT() == 0){
			LCD_sendCommand(clear_display);
			Time_Seconds=0;
       continue;
			}
			else if(SW2_INPUT() == 0){
				NVIC->ISER[0] |= (1<<30); // open again  
		      return Time_Seconds;
			}
			
		}
	}
	
}


static bool Check_Time(uint32_t Total_Seconds){
	if((Total_Seconds >1800) || (Total_Seconds <10)){
		return false;
	}
}
static void Check_startFlag(void){
		LCD_displayString("Enter start");
	  LCD_displayStringRowColumn("to cook",1,0);
		while (Start_flag != 1);
}
void SW_1_2_interruptInit(void){
	
	  GPIOF->IS  &= ~(1<<4)|~(1<<0);        /* make bit 4, 0 edge sensitive */
    GPIOF->IBE &=~(1<<4)|~(1<<0);         /* trigger is controlled by IEV */
    GPIOF->IEV &= ~(1<<4)|~(1<<0);        /* falling edge trigger */
    GPIOF->ICR |= (1<<4)|(1<<0);          /* clear any prior interrupt */
    GPIOF->IM  |= (1<<4)|(1<<0);          /* unmask interrupt */
	
	    NVIC->IP[30] = 3 << 5;     /* set interrupt priority to 3 */
    NVIC->ISER[0] |= (1<<30);  /* enable IRQ30 (D30 of ISER[0]) */
}
void GPIOF_Handler(void)
{	
  if (GPIOF->MIS & 0x10) /* check if interrupt causes by PF4/SW2*/
    {   
      Start_flag = 1;
      GPIOF->ICR |= 0x10; /* clear the interrupt flag */
     } 
    else if (GPIOF->MIS & 0x01) /* check if interrupt causes by PF0/SW1 */
    {   
     if (entering_flag == 1){
			 NVIC->ISER[0] & =~(1<<30);
			while( SW1_INPUT() != 0  && SW2_INPUT() != 0);
	       	if(  SW1_INPUT() == 0){
				   LCD_sendCommand(clear_display);
						LCD_displayString("stopped !!");
						main();
			}
			else if(SW2_INPUT() == 0){
				// it will complete show time decreasing normally
		     NVIC->ISER[0] |=(1<<30);
			}
			 
		 }
		 else {
			 // do nothing
			 // it means that the program is not in showtime decrasing function
			 // so what time i can pause????!
		 }
     GPIOF->ICR |= 0x01; /* clear the interrupt flag */
    }
}
