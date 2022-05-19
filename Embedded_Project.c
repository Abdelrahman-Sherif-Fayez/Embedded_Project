/*******************************************************************************
 *                                definition                                   *
 *******************************************************************************/
#include "lcd.h"
#include "keypad.h"
#include "stdbool.h"
#include "Input_Output.h"

/*******************************************************************************
 *                          private Functions Prototypes                        *
 *******************************************************************************/
static uint8_t get_Operation(void);
static void ShowTimeDecreasing(uint32_t time);
static uint8_t get_Kilos(uint8_t type);
static void Kilos_Display(uint8_t kilos);
static uint32_t get_cookingTime(void);
static bool Check_Time(uint32_t Total_Seconds);
static void Check_startFlag(void);
void SW_1_2_interruptInit(void);
void Disable_Systick(void);
void Enable_Systick(void);


/*******************************************************************************
 *                          global variables                                   *
 *******************************************************************************/
uint8_t Start_flag=0; // flag to start counting
uint8_t  Counter_flag = 0; // flag shows that we inside ShowTimeDecreasing function
uint8_t Stop_flag = 0;     
uint8_t Second_Check_flag = 0;
uint8_t Clear_Time_flag =0;     // flag to clear the time displayed on lcd


void GPIOF_Handler(void){
if (GPIO_PORTF_MIS_R & 0x01) /* check if interrupt causes by PF0/SW2*/
    {
			if(Second_Check_flag== 0)
      Start_flag = 1;
			else if(Second_Check_flag== 1)
			{
				Start_flag = 0;
				Second_Check_flag = 0;
				Enable_Systick();
			}

      GPIO_PORTF_ICR_R = 0x01; /* clear the interrupt flag */
     }
else if (GPIO_PORTF_MIS_R & 0x10) /* check if interrupt causes by PF4/SW1 */
    {
     if (Counter_flag == 1) {
							Second_Check_flag ++;
							if(Second_Check_flag==2){   // If Pressed Switch1 for 2nd time
								
								Enable_Systick();
								Second_Check_flag=0;
								Start_flag = 0;
								Clear_Time_flag=1;
								Stop_flag=1;
							}
							else{
							genericDelay(500);   // 500ms delay between 1st and 2nd Presses
 							Disable_Systick();
							}
						}
		 else {
			 // do nothing
			 // it means that the program is not in showtime decrasing function
			 // so what time i can pause????!
					}
			 GPIO_PORTF_ICR_R = 0x10; /* clear the interrupt flag */
		 }
 }


int main(){
    /*******************************************************************************
	 *                             local variables                                 *
	 *******************************************************************************/
	uint8_t key; //to store the value of the pressed key on the keypad
	uint8_t kilos; //to store the number of kilos
	uint32_t time_seconds; //to store cooking time in seconds
	RGB_LED_INIT(); //initialization of the LEDs
	SW1_INIT(); //initialization of switch 1
	SW2_INIT(); //initialization of switch 2
	LCD_init(); // configure the lcd to be ready for display a messages
	keypad_Init(); //initialization of the keypad
  SW_1_2_interruptInit(); //initialization of interrupts
	LCD_sendCommand(clear_display); //clears the lcd screen
	LCD_sendCommand(FirstRow); //starts display from the first row
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
				if(Stop_flag ==0 ){
					LCD_sendCommand(FirstRow);
					LCD_displayString("Beef Defrosted");
					genericDelay(2000);
				}
				break;
				case 'C':
				kilos = get_Kilos('C');
				Kilos_Display(kilos);
			  Check_startFlag();
				ShowTimeDecreasing(kilos*12);    // Rate of 0.2 mins
				if(Stop_flag ==0 ){
					LCD_sendCommand(FirstRow);
					LCD_displayString("ChickenDefrosted");
					genericDelay(2000);
				}
				break;
				case 'D':
				time_seconds = get_cookingTime();
				ShowTimeDecreasing(time_seconds);
				if(Stop_flag ==0 ){
					LCD_sendCommand(clear_display);
					LCD_displayString("Cooked");
					genericDelay(2000);
				}
				break;
		}
		if(Stop_flag ==1){
			LCD_sendCommand(clear_display);
			LCD_displayString("Cooking Failed");
			genericDelay(2000);
		}
		Start_flag=0;
	}

}
/************************************************************************************
 * Function Name: get_Operation

 * Description: responsible to get a mode of operation
 **********************************************************************************/
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
		LCD_sendCommand(clear_display);
		return key;
	}
}

/************************************************************************************
 * Function Name: ShowTimeDecreasing

 * Description: responsible to show the timer on the screen
 **********************************************************************************/
static void ShowTimeDecreasing(uint32_t time){  // Total time in sec
int i,j;
	uint32_t min, sec;
	min = time/60;
	sec = time%60;
		LCD_moveCursor(1,0);
	  LCD_displayStringRowColumn(1,5,":");
	  Counter_flag = 1;
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
				if(i ==0 && j ==0){
					Clear_Time_flag =0;
				}
	}
			sec= 59;
}
		Start_flag = 0;
		Counter_flag = 0;
}
/************************************************************************************
 * Function Name: get_Kilos

 * Description: responsible for getting a valid number of kilos to cook
 **********************************************************************************/
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
/************************************************************************************
 * Function Name: Kilos_Display

 * Description: responsible for showing the number of kilos to cook on the screen
 **********************************************************************************/
static void Kilos_Display(uint8_t kilos){

	LCD_sendCommand(clear_display);
	LCD_intgerToString(kilos);
	LCD_displayString(" Kg");
	genericDelay(2000); // 2 sec
	LCD_sendCommand(clear_display);
}
/************************************************************************************
 * Function Name: get_cookingTime

 * Description: responsible for getting a valid time for cooking
 **********************************************************************************/
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
			if(cursor ==5 || cursor ==4){
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
		   NVIC_EN0_R &= ~(1<<30); 												// because i need polling method
			while( SW1_INPUT() != 0  && SW2_INPUT() != 0){};
	    if(  SW1_INPUT() == 0){
			LCD_sendCommand(clear_display);
			Time_Seconds=0;
       continue;
			}
			else if(SW2_INPUT() == 0){
					NVIC_EN0_R |= (1<<30); 		// open again
		      return Time_Seconds;
			}

		}
	}

}

/************************************************************************************
 * Function Name: Check_Time

 * Description: to check if the entered time is valid or not
 **********************************************************************************/
static bool Check_Time(uint32_t Total_Seconds){
	if((Total_Seconds >1800) || (Total_Seconds <10)){
		return false;
	}
}
/************************************************************************************
 * Function Name: Check_startFlag

 * Description: to check if the start switch is pressed
 **********************************************************************************/
static void Check_startFlag(void){
		LCD_sendCommand(clear_display);
		LCD_displayString("Enter start");
	  LCD_displayStringRowColumn(1,0,"to cook");
		while (Start_flag != 1);
		LCD_sendCommand(clear_display);
}
/************************************************************************************
 * Function Name: SW_1_2_interruptInit

 * Description: to initialize the interrupts of switch 1 and switch 2
 **********************************************************************************/
void SW_1_2_interruptInit(void){

	  GPIO_PORTF_IS_R  &= (1<<4)|(1<<0);        /* make bit 4, 0 edge sensitive */
    GPIO_PORTF_IBE_R &=(1<<4)|(1<<0);         /* trigger is controlled by IEV */
    GPIO_PORTF_IEV_R &= (1<<4)|(1<<0);        /* falling edge trigger */
    GPIO_PORTF_ICR_R |= (1<<4)|(1<<0);          /* clear any prior interrupt */
    GPIO_PORTF_IM_R  |= (1<<4)|(1<<0);          /* unmask interrupt */

	  NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00600000 ;     /* set interrupt priority to 3 */
		NVIC_EN0_R |= (1<<30);  /* enable IRQ30 */
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
		SysTick_wait(16000);		//1ms
		if(Clear_Time_flag ==1){
			i=n;
		}
	}
}

 void Disable_Systick(void){
			NVIC_ST_CTRL_R = 0x00;
		}

void Enable_Systick(void){
			NVIC_ST_CTRL_R = 0x05;
		}
