#define F_CPU 8000000UL //needs to be defined before including the avr/delay.h library
#define SPARK_DELAY_TIME 20
#define FIRST_DIGIT 'C'
#define SECOND_DIGIT '3'

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define state1_alarm_off 020
#define state2_alarm_off 050
#define state3_alarm_off 100
#define state4_alarm_off 120
#define state5_alarm_off 150
#define state6_alarm_off 180

#define state_alarm_on_off 205
#define state2_alarm_on 250
#define state3_alarm_on 280
#define state4_alarm_on 300
#define state5_alarm_on 400
#define state6_alarm_on 450
#define state7_alarm_on 500
#define state8_alarm_on 800

unsigned char light = 0x00;
unsigned int previous_keypad_state = 0; //hold the state of the keyboard 0x0000
int ascii[16];							//Is the ascii code for each key on the keyboard

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
	PORTC = 0x00;
	return pressed_keypad;
}
unsigned int scan_keypad_rising_edge_sim(void)
{
	unsigned int pressed_keypad1, pressed_keypad2, current_keypad_state, final_keypad_state;

	pressed_keypad1 = scan_keypad_sim();
	_delay_ms(SPARK_DELAY_TIME);
	pressed_keypad2 = scan_keypad_sim();
	current_keypad_state = pressed_keypad1 & pressed_keypad2;
	final_keypad_state = current_keypad_state & (~previous_keypad_state);
	previous_keypad_state = current_keypad_state;

	return final_keypad_state;
}
unsigned char keypad_to_ascii_sim(unsigned int final_keypad_state)
{
	volatile int j;
	volatile unsigned int temp;

	for (j = 0; j < 16; j++)
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
unsigned char swapNibbles(unsigned char x)
{
	return ((x & 0x0F) << 4 | (x & 0xF0) >> 4);
}
void write_2_nibbles_sim(unsigned char data)
{
	_delay_us(6000);

	unsigned char temp, Nibble_data;

	temp = PIND;
	temp = temp & 0x0f;
	Nibble_data = data & 0xf0;
	Nibble_data = temp + Nibble_data;
	PORTD = Nibble_data;

	PORTD = PORTD | 0x08;
	PORTD = PORTD & 0xf7;
	_delay_us(6000);

	data = swapNibbles(data);
	Nibble_data = data & 0xf0;
	Nibble_data = Nibble_data + temp;
	PORTD = Nibble_data;

	PORTD = PORTD | 0x08;
	PORTD = PORTD & 0xf7;
	return;
}
void lcd_data_sim(unsigned char data)
{
	PORTD = PORTD | 0x04;
	write_2_nibbles_sim(data);
	_delay_us(43);
	return;
}
void lcd_command_sim(unsigned char data)
{
	PORTD = PORTD & 0xfb;
	write_2_nibbles_sim(data);
	_delay_us(39);
	return;
}
void lcd_init_sim()
{
	_delay_ms(40);
	for (int i = 1; i <= 2; i++)
	{
		PORTD = 0x30;
		PORTD = PORTD | 0x08;
		PORTD = PORTD & 0xf7;

		_delay_us(39);
		_delay_us(1000);
	}

	PORTD = 0x20;
	PORTD = PORTD | 0x08;
	PORTD = PORTD & 0xf7;

	_delay_us(39);
	_delay_us(1000);

	lcd_command_sim(0x28);
	lcd_command_sim(0x0C);
	lcd_command_sim(0x01);

	_delay_us(1530);

	lcd_command_sim(0x06);

	return;
}
void ADC_init()
{ //initialize the ADC with CK/128,Vref=Vcc ,A0 port to take the ADC
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX = (1 << REFS0);
}
void initialize_timer_interrupts()
{
	TCNT1 = 0xfcf3;									  //init to specific number for 0.1sec overflow
	TCCR1B = (1 << CS12) | (0 << CS11) | (1 << CS10); //CLK/1024  // Timer mode with 1024 prescler
	TIMSK = (1 << TOIE1);							  //enable Timer1
}
ISR(TIMER1_OVF_vect) // Timer1 ISR
{

	ADCSRA |= (1 << ADSC); //start the ADC transformation
	_delay_us(10);		   //wait for the transformation
	int Ain, AinLow;
	cli();				   // close the interrupts when we read the ADC
	AinLow = (int)ADCL;	   //read the ADCL
	Ain = (int)ADCH * 256; //read the ADCH and mul with the 256 to correct the number
	sei();				   //enable the interrupts
	Ain = Ain + AinLow;	   //add the 2 ADCL ADCH
	if (Ain >= 205)
	{
		lcd_init_sim(); //show the message to the lsd
		lcd_data_sim('G');
		lcd_data_sim('A');
		lcd_data_sim('S');
		lcd_data_sim(' ');
		lcd_data_sim('D');
		lcd_data_sim('E');
		lcd_data_sim('T');
		lcd_data_sim('E');
		lcd_data_sim('C');
		lcd_data_sim('T');
		lcd_data_sim('E');
		lcd_data_sim('D');

		if (light == 0xff)
		{				   //if the lights are on close them to cause blink
			light = 0x00;  //close the flag
			PORTB &= 0x80; //close the lights
		}
		else
		{
			if (Ain <= state2_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x01; //1 left led on
				light = 0xff;
				//lsb on
			}
			else if (Ain <= state3_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x03; //2 left leds on
				light = 0xff;
			}
			else if (Ain <= state4_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x07; // 3 left leds on
				light = 0xff;
			}
			else if (Ain <= state5_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x0f; // 4 left leds on
				light = 0xff;
			}
			else if (Ain <= state6_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x1f; // 5 left leds on
				light = 0xff;
			}
			else if (Ain <= state7_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x3f; // 6 left leds on
				light = 0xff;
			}
			else if (Ain <= state8_alarm_on)
			{
				PORTB &= 0x80;
				PORTB |= 0x7f; // 6 left leds on
				light = 0xff;
			}
		}
	}
	else
	{
		lcd_init_sim(); //show the message to the LCD
		lcd_data_sim('C');
		lcd_data_sim('L');
		lcd_data_sim('E');
		lcd_data_sim('A');
		lcd_data_sim('R');

		if (Ain <= state1_alarm_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x01; //1 left leds on
			light = 0xff;
			//lsb on
		}
		else if (Ain <= state2_alarm_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x03; //2 left leds on
			light = 0xff;
		}
		else if (Ain <= state3_alarm_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x07; // 3 left leds on
			light = 0xff;
		}
		else if (Ain <= state4_alarm_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x0f; // 4 left leds on
			light = 0xff;
		}
		else if (Ain <= state5_alarm_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x1f; // 5 left leds on
			light = 0xff;
		}
		else if (Ain <= state6_alarm_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x3f; // 6 left leds on
			light = 0xff;
		}

		else if (Ain <= state_alarm_on_off)
		{
			PORTB &= 0x80;
			PORTB |= 0x7f; // 7 left leds on
			light = 0xff;
		}
	}
	TCNT1 = 0xfcf3;
}
ISR(ADC_vect)
{ //just refresh the ADCH,ADCL
}

int main(void)
{

	unsigned char first_number, second_number;

	DDRB = 0Xff; // B for output
	DDRC = 0xf0; // c 4 msb for output and 4 lsb for input
	DDRD = 0xff;

	initialize_ascii();
	ADC_init();
	initialize_timer_interrupts();
	lcd_init_sim();
	sei();

	while (1)
	{
		do
		{
			first_number = read4x4(); // wait for the number to be pushed
		} while (!first_number);

		do
		{
			second_number = read4x4(); // wait for the second number to be pushed
		} while (!second_number);
		// compare it with the given number (here C3)
		if ((first_number == FIRST_DIGIT) & (second_number == SECOND_DIGIT))
		{
			cli(); //close the interrupts when the team is on the room
			lcd_init_sim();
			lcd_data_sim('W');
			lcd_data_sim('E');
			lcd_data_sim('L');
			lcd_data_sim('C');
			lcd_data_sim('O');
			lcd_data_sim('M');
			lcd_data_sim('E');
			lcd_data_sim(' ');
			lcd_data_sim(first_number);
			lcd_data_sim(second_number);

			//if true the just open the leds for 4 sec

			PORTB = 0x00;
			PORTB = PORTB | 0x80;
			_delay_ms(4000);
			PORTB = PORTB & 0x7f;
			sei();
		}
		else
		{ //wrong password
			int i;
			//if false just open and close the leds with T=0.5 sec
			for (i = 0; i < 4; i++)
			{
				PORTB = PORTB | 0x80;
				_delay_ms(500);
				PORTB = PORTB & 0x7f;
				_delay_ms(500);
			}
		}
	}

	return 0;
}