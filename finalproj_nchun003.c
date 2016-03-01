/*
 * LED_MATRIX.c
 *
 * Created: 8/19/2015 7:47:55 PM
 *  Author: Nichelle
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>



volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;

//unsigned short NeutTone = 500;

unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
void TimerOn()
{
	// AVR timer/counter controller register TCCR1
	// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TC	NT1 register will count at 125,000 ticks/s
	TCCR1B =0x0B;
	// AVR output compare register OCR1A.
	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1	ms has passed. Thus, we compare to 125.
	OCR1A=125;
	// AVR timer interrupt mask register
	// bit1: OCIE1A 	--	enables compare match interrupt
	TIMSK1 	=0x02;	//Initialize avr counter
	TCNT1=0;
	// TimerISR will be called every _avr_timer_cntcurr	milliseconds
	_avr_timer_cntcurr	=_avr_timer_M;
	//Enable global interrupts	:	0x80: 1000000
	SREG=0x80;
}

void TimerOff()
{
	// bit3bit1bit0=000: timer off
	TCCR1B 	=	0x00;
}
void TimerISR()
{
	TimerFlag=1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1
	//	(every 1 ms per TimerOn settings)
	// Count down to 0 rather than up to TOP	(	res	ults in a more	efficient comparison)
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0)
	{
		// Call the ISR that the user uses
		TimerISR();
		_avr_timer_cntcurr 	= _avr_timer_M;
	}
}

void TimerSet(unsigned long M)
{
	_avr_timer_M =	M;
	_avr_timer_cntcurr  = _avr_timer_M;
}

void transmit_data(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTD = 0x08;
		// set SER = next bit of data to be sent.
		PORTD |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTD |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTD |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTD = 0x00;
}

void transmit_data2(unsigned char data) {
	int i;
	for (i = 0; i < 8  ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTB = 0x08;
		// set SER = next bit of data to be sent.
		PORTB |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTB |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTB |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTB = 0x00;
}

void transmit_data3(unsigned char data) {
	int i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x02;
	}
	// set RCLK = 1. Rising edge copies data from “Shift” register to “Storage” register
	PORTC |= 0x04;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	// in Free Running Mode, a new conversion will trigger
	// whenever the previous conversion completes.
}

const unsigned char patterns_orange [] = {0x01, 0x24, 0x42, 0x3C};
const unsigned char col_orange [] = {0x7F, 0x40, 0x10, 0x08};
const unsigned char col_orange2 [] = {0x7F, 0x02, 0x08, 0x10};
unsigned char m = 0;
enum OrangeSmileyFace_SM {/*INIT, Button_Press, */Orange_Smile_Display} OrangeSmile;
int Orange_Tick() {

	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	static unsigned char column_sel2 = 0x7F;
	unsigned char Button = ~PINA & 0x02;
	//static unsigned char column_val2 0x01;
	//static unsigned char column_sel2 = 0x7F;
	// === Transitions ===
	m++;
	switch (OrangeSmile) {

		case Orange_Smile_Display:
		{
			break;
		}
		default:
		{
			OrangeSmile = Orange_Smile_Display;
			break;
		}
	}

	// === Actions ===
	switch (OrangeSmile) {

		case Orange_Smile_Display: // If illuminated LED in bottom right corner
		{
			if(m != 4)
			{
				column_val = patterns_orange[m];
				column_sel = col_orange[m];
				column_sel2 = col_orange2[m];
			}
			else{
				m = 0;
			}
			break;
		}
		default: break;
	}


	transmit_data(column_val);
	PORTC = ~column_sel; // PORTB selects column to display pattern
	//transmit_data3(~column_sel);
	PORTB = ~column_sel2;
	//transmit_data2(column_sel2);
	return OrangeSmile;
};

const unsigned char patterns_orange_talk [] = {0x01, 0x24, 0x42, 0x7E, 0x42, 0x24, 0x18};
const unsigned char orange_talk_col [] = {0x7F, 0x40, 0x10, 0x08, 0x04, 0x02, 0x01};
const unsigned char orange_talk_col2 [] = {0x7F, 0x02, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned char n = 0;
enum OrangeSmileyTalkFace_SM {OrangeSmileTalk_Display} OrangeSmileTalk;
int Orange_Talk_Tick() {

	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	static unsigned char column_sel2 = 0x7F;
	//static unsigned char column_val2 0x01;
	//static unsigned char column_sel2 = 0x7F;
	// === Transitions ===
	n++;
	switch (OrangeSmileTalk) {
		case OrangeSmileTalk_Display:
		{
			break;
		}
		default:
		{
			OrangeSmileTalk = OrangeSmileTalk_Display;
			break;
		}
	}

	// === Actions ===
	switch (OrangeSmileTalk) {
		case OrangeSmileTalk_Display: // If illuminated LED in bottom right corner
		{
			if(n != 7)
			{
				column_val = patterns_orange_talk[n];
				column_sel = orange_talk_col[n];
				column_sel2 = orange_talk_col2[n];
			}
			else{
				n = 0;
			}
			break;
		}
		default: break;
	}

	transmit_data(column_val);
	PORTC = ~column_sel; // PORTB selects column to display pattern
	//transmit_data3(~column_sel);
	PORTB = ~column_sel2;
	//transmit_data2(column_sel2);
	return OrangeSmileTalk;
};

const unsigned char patterns_green [] = {0x01, 0x66, 0x42, 0x3C};
const unsigned char col [] = /*{0x7F, 0x02, 0x08, 0x10};*/ {0x7F, 0x40, 0x10, 0x08};
unsigned char i = 0;
enum GreenSmileyFace_SM {Smile_Display} Smile;
int Green_Tick() {
DDRB = 0x00;
	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	//static unsigned char column_val2 0x01;
	//static unsigned char column_sel2 = 0x7F;
	// === Transitions ===
	i++;
	switch (Smile) {
		case Smile_Display:
		{
			 break;
		}
		default: 
		{
			Smile = Smile_Display;
			break;
		}
	}

	// === Actions ===
	switch (Smile) {
		case Smile_Display: // If illuminated LED in bottom right corner
		{
			if(i != 4)
			{
				column_val = patterns_green[i];
				column_sel = col[i];
			}
			else{
				i = 0;
			}
			break;
		}
		default: break;
	}
	transmit_data(column_val);
	//PORTD = column_val; // PORTA displays column pattern
	PORTC = ~column_sel; // PORTB selects column to display pattern
	//transmit_data3(~column_sel);
	return Smile;
};

const unsigned char patterns_green_talk [] = {0x01, 0x66, 0x42, 0x7E, 0x42, 0x24, 0x18};
const unsigned char green_talk_col [] = {0x7F, 0x40, 0x10, 0x08, 0x04, 0x02, 0x01};
unsigned char j = 0;
enum GreenSmileyTalkFace_SM {SmileTalk_Display} SmileTalk;
int Green_Talk_Tick() {
DDRB = 0x00;
	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	//static unsigned char column_val2 0x01;
	//static unsigned char column_sel2 = 0x7F;
	// === Transitions ===
	j++;
	switch (SmileTalk) {
		case SmileTalk_Display:
		{
			break;
		}
		default:
		{
			SmileTalk = SmileTalk_Display;
			break;
		}
	}

	// === Actions ===
	switch (SmileTalk) {
		case SmileTalk_Display: // If illuminated LED in bottom right corner
		{
			if(j != 7)
			{
				column_val = patterns_green_talk[j];
				column_sel = green_talk_col[j];
			}
			else{
				j = 0;
			}
			break;
		}
		default: break;
	}

	//PORTD = column_val; // PORTA displays column pattern
	transmit_data(column_val);
	PORTC = ~column_sel; // PORTB selects column to display pattern
	//transmit_data3(~column_sel);
	return SmileTalk;
};

const unsigned char patterns_red [] = {0x01, 0x66, 0x7E};
const unsigned char col_red [] = {0x7F, 0x02, 0x10};
unsigned char k = 0;
enum RedSmileyFace_SM {Red_Smile_Display} RedSmile;
int Red_Tick() {

	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	//static unsigned char column_val2 0x01;
	//static unsigned char column_sel2 = 0x7F;
	// === Transitions ===
	k++;
	switch (RedSmile) {
		case Red_Smile_Display:
		{
			break;
		}
		default:
		{
			RedSmile = Red_Smile_Display;
			break;
		}
	}

	// === Actions ===
	switch (RedSmile) {
		case Red_Smile_Display: // If illuminated LED in bottom right corner
		{
			if(k != 3)
			{
				column_val = patterns_red[k];
				column_sel = col_red[k];
			}
			else{
				k = 0;
			}
			break;
		}
		default: break;
	}

//USART_Flush(1);
//PORTD = column_val; // PORTA displays column pattern
//if(USART_IsSendReady(1))
//{
//	USART_Send(column_val,1);
//}
	transmit_data(column_val);
	PORTB = ~column_sel;
	//transmit_data2(column_sel);
	return RedSmile;
};

const unsigned char patterns_red_talk [] = {0x01, 0x66, 0x7E, 0x42, 0x24, 0x18};
const unsigned char red_talk_col [] = {0x7F, 0x02, 0x10, 0x20, 0x40, 0x80};//0x80};
unsigned char l = 0;
enum RedSmileyTalkFace_SM {RedSmileTalk_Display} RedSmileTalk;
int Red_Talk_Tick() {

	// === Local Variables ===
	static unsigned char column_val = 0x01; // sets the pattern displayed on columns
	static unsigned char column_sel = 0x7F; // grounds column to display pattern
	//static unsigned char column_val2 0x01;
	//static unsigned char column_sel2 = 0x7F;
	// === Transitions ===
	l++;
	switch (RedSmileTalk) {
		case RedSmileTalk_Display:
		{
			break;
		}
		default:
		{
			RedSmileTalk = RedSmileTalk_Display;
			break;
		}
	}

	// === Actions ===
	switch (RedSmileTalk) {
		case RedSmileTalk_Display: // If illuminated LED in bottom right corner
		{
			if(l != 6)
			{
				column_val = patterns_red_talk[l];
				column_sel = red_talk_col[l];
			}
			else{
				l = 0;
			}
			break;
		}
		default: break;
	}


	transmit_data(column_val);
	PORTB = ~column_sel;
	//transmit_data2(column_sel);
	return RedSmileTalk;
};




int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	DDRB = 0xFF; PORTB = 0x00;

	ADC_init();
	TimerSet(1);
	TimerOn();
	OrangeSmile = Orange_Smile_Display;
	OrangeSmileTalk = OrangeSmileTalk_Display;
	Smile = Smile_Display;
	SmileTalk = SmileTalk_Display;
	RedSmile = Red_Smile_Display;
	RedSmileTalk = RedSmileTalk_Display;
	unsigned short my_short;
	unsigned short MAX;
	MAX = 120.999997;//116;//231.999999; //226;
	unsigned int green_talk_count = 0;
	unsigned int orange_talk_count = 0;
	unsigned int red_talk_count = 0;
	unsigned int tempgreen = 0;
	unsigned int temporange = 0;
	unsigned int tempred = 0;	
	unsigned int begin = 0;
	unsigned char button = ~PINA & 0x02;
	while(1)
	{
		//Orange_Tick();
		my_short = ADC;
		//MAX = NeutTone;
		//if (MAX < my_short)
		//{
		//	MAX = my_short;
		//}																																																																																																																																																											
		if((my_short >= (MAX+15/*116*/)) && ((temporange >= 1000) || ((tempred >= 150) || (tempgreen >0)) || (begin ==0)))
		{
			if(begin == 0)
			{
				begin = 1;
			}
			DDRB = 0x00; DDRC = 0xFF;
			//Green_Tick();
			Green_Talk_Tick();
			//Orange_Talk_Tick();
			green_talk_count = 150;
			orange_talk_count = 0;
			red_talk_count = 0;
			//temp = 1;
			tempred = 0;
			temporange = 0;
			if(tempgreen >= 1000)
			{
				tempgreen = 0;
			}
			tempgreen++;
		}		
		else if((my_short >= (MAX+10/*8*//*42*/)) && ((tempgreen >= 1000) || ((tempred >=150) || (temporange >0)) || (begin ==0)))
		{
			//Green_Talk_Tick();
			if(begin == 0)
			{
				begin = 1;
			}
			DDRB = 0xFF; DDRC = 0xFF;
			Orange_Talk_Tick();
			orange_talk_count = 150;
			green_talk_count = 0;
			red_talk_count = 0;
			//temp = 0;
			tempgreen = 0;
			tempred = 0;
			if(temporange >= 1000)
			{
				temporange = 0;
			}
			temporange++;
		}
		else if ((my_short >= (MAX)) && ((tempgreen >= 1000)  || ((temporange >= 1000) || (tempred >0)) || (begin ==0)))
		{
			//Green_Talk_Tick();
			if(begin ==0)
			{
				begin = 1;
			}
			DDRB = 0xFF; DDRC = 0x00;
			Red_Talk_Tick();
			red_talk_count = 150;
			green_talk_count = 0;
			orange_talk_count = 0;
			//temp = 2;
			tempgreen = 0;
			temporange = 0;
			if(tempred >= 150)
			{
				tempred = 0;
			}
			tempred++;
		}
		if(green_talk_count > 0)// && tempgreen != 0)
		{
				DDRB = 0x00; DDRC = 0xFF;
				Green_Talk_Tick();
				green_talk_count--;
				tempgreen++;
		}	
		else if(orange_talk_count > 0)// && temporange != 0)
		{
				DDRB = 0xFF; DDRC = 0xFF;
				Orange_Talk_Tick();
				orange_talk_count--;
				temporange++;
		}
		else if(red_talk_count > 0)// && tempred != 0)
		{
			DDRB = 0xFF; DDRC = 0x00;
			Red_Talk_Tick();
			red_talk_count--;
			tempred++;
		}	
		else
		{
			//Red_Tick();
			//Green_Tick();
			if(tempgreen ==0 && temporange==0 && tempred==0)
			{
				DDRB = 0xFF; DDRC = 0xFF;
				Orange_Tick();
			}
			else if(tempgreen != 0)
			{
				DDRB = 0x00; DDRC = 0xFF;
				Green_Tick();
			}
			else if(temporange != 0){
				DDRB = 0xFF; DDRC = 0xFF;
				Orange_Tick();
			}
			else if(tempred != 0){
				DDRB = 0xFF; DDRC = 0x00;
				Red_Tick();
			}


		}
		
		while(!TimerFlag);
		TimerFlag = 0;	
	}
	return 0;
}