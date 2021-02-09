/*
 * GccApplication12.c
 *
 * Created: 11/21/2020 3:01:57 PM
 * Author : georg
 */ 

#define F_CPU 8000000UL //needs to be defined before including the avr/delay.h library
#define SPARK_DELAY_TIME 20
#define FIRST_DIGIT 'C'
#define SECOND_DIGIT '3'

#include <avr/io.h>
#include <util/delay.h>

unsigned int previous_keypad_state = 0; //hold the state of the keyboard 0x0000
int ascii[16]; //Is the ascii code for each key on the keyboard

unsigned char scan_row_sim(int row)
{	
	unsigned char temp;
	volatile unsigned char pressed_row;

	temp = 0x08;
	PORTC = temp << row;
	_delay_us(500);
	asm("nop");
	asm("nop");
	pressed_row = PINC & 0x0f;

	return pressed_row;
}
unsigned int scan_keypad_sim(void)
{
	volatile unsigned char pressed_row1, pressed_row2, pressed_row3, pressed_row4;
	volatile unsigned int pressed_keypad = 0x0000;

	pressed_row1 = scan_row_sim(1);
	pressed_row2 = scan_row_sim(2);
	pressed_row3 = scan_row_sim(3);
	pressed_row4 = scan_row_sim(4);

	pressed_keypad = (pressed_row1 << 12 | pressed_row2 << 8) | (pressed_row3 << 4) | (pressed_row4);
	PORTC =0x00;
	return pressed_keypad;
}
unsigned int scan_keypad_rising_edge_sim(void)
{
	unsigned int pressed_keypad1, pressed_keypad2, current_keypad_state, final_keypad_state;

	pressed_keypad1 = scan_keypad_sim();
	_delay_ms(SPARK_DELAY_TIME);
	pressed_keypad2 = scan_keypad_sim();
	current_keypad_state = pressed_keypad1 & pressed_keypad2;
	final_keypad_state = current_keypad_state & (~ previous_keypad_state);
	previous_keypad_state = current_keypad_state;

	return final_keypad_state;
}
unsigned char keypad_to_ascii_sim(unsigned int final_keypad_state)
{
	volatile int j;
	volatile unsigned int temp;

	for (j=0; j<16; j++)
	{
		temp = 0x01;
		temp = temp << j;
		if (final_keypad_state & temp) //if you find the only pressed key then return 
		{
			return ascii[j];
		}
	}
	//should not reach here
	return 1;
}
void initialize_ascii(void)
{
	ascii[0] = '*';
	ascii[1] = '0';
	ascii[2] = '#';
	ascii[3] = 'D';
	ascii[4] = '7';
	ascii[5] = '8';
	ascii[6] = '9';
	ascii[7] = 'C';
	ascii[8] = '4';
	ascii[9] = '5';
	ascii[10] = '6';
	ascii[11] = 'B';
	ascii[12] = '1';
	ascii[13] = '2';
	ascii[14] = '3';
	ascii[15] = 'A';
}
unsigned char read4x4(void)
{
	unsigned int keypad_state;
	unsigned char ascii_code;

	keypad_state = scan_keypad_rising_edge_sim(); // read the state of the keyboard
	if (!keypad_state)
	{
		return 0;
	}
	ascii_code = keypad_to_ascii_sim(keypad_state); // encode it to ascii code

	return ascii_code;
}

int main(void)
{
	int i;
	volatile unsigned char first_number, second_number;

	DDRB = 0Xff; // B for output
	DDRC = 0xf0; // c 4 msb for output and 4 lsb for input

	initialize_ascii();

	while (1)
	{
		do
		{
			first_number = read4x4(); // wait for the number to be pushed
		}
		while(!first_number);

		do
		{
			second_number = read4x4(); // wait for the second number to be pushed
		}
		while(!second_number);
		// compare it with the given number (here C3)
		if ((first_number == FIRST_DIGIT) & (second_number == SECOND_DIGIT))
		{ 	
			//if true the just open the leds for 4 sec
			PORTB = 0Xff;
			_delay_ms(4000);
			PORTB = 0X00;
		}
		else
		{ 
			//if false just open and close the leds with T=0.5 sec
			for (i=0; i<4; i++)
			{
				PORTB = 0Xff;
				_delay_ms(500);
				PORTB = 0X00;
				_delay_ms(500);
			}
		}
	}

	return 0;
}











