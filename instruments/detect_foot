/*
 * attiny2313a_footsense5.c
 *
 * Created: 02.07.2017 17:08:53
 * Author : 25mmHg
 * Circuit without LED and XTAL:
 *
 
        .---------------------------------.
        |                                 |
        |          ATTINY2313A            |
        |                                 |
        |                                 |
        |                                 |
        |     Pin3           PD3  PD5     |
        '------o-------------o-----o-----'
               |             |     |
               |             |     |  
               o             |     |
         usart TX            |  || |     
                             '--||--o-------------.
                                ||                |
                                                  |
               Cref = 0,47uF Folie                |
                                        .---------o---------.
                                        |                    |
                                        |                    |
                                        |                    |
                                        |     Coke-Can       |
                                        |                    |
                                        |                    |
                                        '--------------------'
 *
 * (created by AACircuit v1.28.6 beta 04/19/05 www.tech-chat.de)
 *
 */ 

//#define USART4DEBUG
#define F_CPU 16000000UL
#define FALSE 0
#define TRUE  1
#define USART_BAUDRATE 115200UL
#define USART_BAUD_CALC (F_CPU/(USART_BAUDRATE*8UL))-1 // ***without U2X use 16UL
#define CAPSENSE_PORT PORTD
#define CAPSENSE_DDR  DDRD
#define CAPSENSE_PIN PIND
#define CAPSENSE_CREF_COLD PD5
#define CAPSENSE_CREF_HOT  PD3 // also connect the Coke-Can
#define CAPSENSE_CSENSE_CHARGE_TIME 8U //  3us 
#define CAPSENSE_CSENSE_DISCHARGE_TIME 2U
#define CAPSENSE_CREF_DISCHARGE_TIME 8U // 30us
#define CAPSENSE_CHARGECOUNTER_MAX 0x4FFFU
#define CAPSENSE_FILTER3_TH 8U

#include <avr/io.h>
#include <util/delay.h>

uint16_t capsense_charge_Cref()
{
	uint16_t chargecounter = 0;
	uint8_t charged = FALSE;
	
	while(!charged)
	{
		/*   DISCONNECT CREF _COLD   */
		CAPSENSE_DDR  &= ~((1 << CAPSENSE_CREF_COLD) | (1 << CAPSENSE_CREF_HOT));
		CAPSENSE_PORT &= ~(1 << CAPSENSE_CREF_COLD);

		/*   CHARGE  CSENSE  VIA INTERNAL PULLUP  */	
		CAPSENSE_PORT |=  (1 << CAPSENSE_CREF_HOT);
		_delay_us(CAPSENSE_CSENSE_CHARGE_TIME);
		
		/*    CSENSE || CREF,  CHARGE  CREF     */
		CAPSENSE_PORT &= ~(1 << CAPSENSE_CREF_HOT);
		CAPSENSE_DDR  |=  (1 << CAPSENSE_CREF_COLD);		
		_delay_us(CAPSENSE_CSENSE_DISCHARGE_TIME);
		
		chargecounter++;	
		charged = (CAPSENSE_PIN & (1 << CAPSENSE_CREF_HOT)) || (chargecounter >= CAPSENSE_CHARGECOUNTER_MAX);
	}    
	/*     DISCHARGE CREF     */
	CAPSENSE_DDR  |= (1 << CAPSENSE_CREF_HOT);
	_delay_us(CAPSENSE_CREF_DISCHARGE_TIME);
	return chargecounter;
}

#ifdef USART4DEBUG
	void usart_init()
	{
		/*     SET USART BAUDRATE      */
		UBRRH = (uint8_t) ((USART_BAUD_CALC) >> 8);
		UBRRL = (uint8_t) USART_BAUD_CALC;
		/*     ENABLE  DOUBLE PRECISION 4 HIGH BAUDRATES, LOOK ***    */
		UCSRA |= 1 << U2X;
		/*     ENABLE TRANSMITTER     */
		UCSRB |= 1 << TXEN;
		/*  SET  FRAME  FORMAT  8N2  */
		UCSRC |= (1 << USBS) | (3 << UCSZ0);
	}

	void usart_TX_char(char payload)
	{
		/* WAIT FOR EMPTY BUFFER  */
		while(!(UCSRA & (1 << UDRE)));
		UDR = payload;
	}

	void usart_TX_string(char *payload_ptr)
	{
		while(*payload_ptr)
		{
			usart_TX_char(*payload_ptr);
			payload_ptr++;
		}
	}

	void usart_process2hex(uint16_t data2hex)
	{
		/*    CONVERT UNSIGNED INTEGER 2 HEX NUMBERS  AND SHOW  IT    */
		uint16_t mask = 0xF;
		for(uint8_t i=0; i<4; i++)
		{
			uint8_t temp = (data2hex >> (4*(3-i))) & mask;
			usart_TX_char(temp < 0xA ? temp+'0' : temp-0xA+'A');
		}
		usart_TX_char('\t'); // Tab or New Line ('\n')
	}

	void usart_prozess2graph(uint32_t data2graph)
	{
		/*    CONVERT UNSIGNED INTEGER 2 LOG GRAPH WITH 1/4 BIT  AND SHOW  IT NICE    */
		data2graph <<= 2;
		for (uint8_t i=0; data2graph > 3; i++)
		{
			usart_TX_string("||||");
			if(data2graph < 8)
			{
				if(data2graph & 2)
				usart_TX_string("||");
				if(data2graph & 1)
				usart_TX_char('|');
			}
			data2graph >>= 1;
		}
		usart_TX_char('\n');
	}
#endif

void process_data(uint16_t data)
{
	/*    BANDPASSFILTER     */	 
	static uint32_t filter1;
	filter1 = filter1 ? ((filter1 << 6) - filter1 + data) >> 6 : data; //simple hotstart
	static uint32_t filter2;
	filter2 = filter2 ? ((filter2 << 3) - filter2 + data) >> 3 : data; //simple hotstart
	int16_t bp_data = filter1 - filter2;
	
	/*    FIRST DERIVATIVE    */	
	static int16_t old_bp_data;
	uint16_t first_derivate_data = old_bp_data > bp_data ? old_bp_data - bp_data : bp_data - old_bp_data;
	static uint32_t filter3;
	filter3 = filter3 ? ((filter3 << 3) - filter3 + first_derivate_data) >> 3 : first_derivate_data; //simple hotstart
	old_bp_data = bp_data;
	
	/*    DATA 2 THRESHOLD    */
	if(filter3 > CAPSENSE_FILTER3_TH)
	{
		if(data < filter1)
		PORTD |= 1 << PD6;
		else
		PORTD &= ~(1<<PD6);
	}
	
	#ifdef USART4DEBUG
		/* OUTPUT DATA 4 DEBUG  */
		usart_TX_string("data:\t");
		usart_process2hex(data);
		usart_TX_string("filter1:\t");
		usart_process2hex(filter1);
		usart_TX_string("filter2:\t");
		usart_process2hex(filter2);
		usart_TX_string("filter3:\t");
		usart_process2hex(filter3);
		usart_TX_string("log2(f3):  ");
		usart_prozess2graph(filter3);
	#endif
}

int main(void)
{
	#ifdef USART4DEBUG
		usart_init();
	#endif
			
	DDRD |= 1 << PD6;
	    
    while (1) 
    {
		process_data(capsense_charge_Cref());
		_delay_us(100);	
    }
}
