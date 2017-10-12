/*
 * attiny4313_basszange2.c
 *
 * Created: 11.10.2017 23:45:51
 * Author : 25mmHg
 * Circuit without LED and XTAL. 
 * need XTAL only 4 USART @ high baudrates
 * 
        .---------------------------------.
        |		                          |
        |          ATTINY4313             |
        |                                 |
        |                                 |
        |                                 |
        |     Pin3          PD5  PD3      |
        '------o-------------o-----o------´
               |             |     |
               |             |    .-.
               |             |    | | Rv = 1k  (for full  discharge time Cref = 2ms)
               |             |    | |
               |             |    '-'
               o             |     | 
            usart TX	     | ||  |     
                             '-||--o
                               ||  |             ___ GND 
                                   |              |
               Cref = 0,47uF MKS4  |              |
                                   |    .---------o--------.
                             .-----o--------------------.  |
                             |           <--------->    |  |
                             |                          |  |
                             |           18...180pF Cs  |  |
                             |                          |  |
                             '-------------------------´   |
                                       '-------------------'
 *
 * (created by AACircuit v1.28.6 beta 04/19/05 www.tech-chat.de)
 *
 */ 

#define F_CPU 16000000UL

//#define USART4DEBUG
#define USART_BAUDRATE 9600U
#define USART_BAUD_CALC (F_CPU/(USART_BAUDRATE*8UL))-1 // ***without U2X use 16UL

#define SET_STATE(BIT) (state_OS |=  (1 << (BIT)))
#define TGL_STATE(BIT) (state_OS ^=  (1 << (BIT)))
#define CLR_STATE(BIT) (state_OS &= ~(1 << (BIT)))
#define TST_STATE(BIT) (state_OS &   (1 << (BIT)))

#define STATE_CREF_CHARGED 3
#define STATE_CREF_TIMEOUT 4

#define CAPSENSE_PORT_REG PORTD
#define CAPSENSE_DDR_REG  DDRD
#define CAPSENSE_PIN_REG PIND
#define CAPSENSE_CREF_COLD_BIT PD5
#define CAPSENSE_CREF_HOT_BIT  PD3 // via Rv, the sense area is also connected
#define CAPSENSE_CSENSE_CHARGE_TIME 1U // us
#define CAPSENSE_CSENSE_DISCHARGE_TIME 1U // us
#define CAPSENSE_CREF_DISCHARGE_TIME 3U // ms
#define CAPSENSE_CHARGECOUNTER_MAX 0xFFFEU

#define CAPSENSE_OFFSETVALUE (5U*0xFF+120) 

#include <avr/io.h>
#include <util/delay.h>

volatile uint8_t state_OS; // systemstate


uint16_t capsense_charge_Cref()
{
	uint16_t chargecounter = 0;
	CLR_STATE(STATE_CREF_CHARGED);
	CLR_STATE(STATE_CREF_TIMEOUT);
	CAPSENSE_PORT_REG &= ~(1 << CAPSENSE_CREF_COLD_BIT); // Cref_cold is always GND or HiZ
	
	while(!(TST_STATE(STATE_CREF_CHARGED)|(TST_STATE(STATE_CREF_TIMEOUT))))
	{
		//   DISCONNECT CREF _COLD
		CAPSENSE_DDR_REG  &= ~(1 << CAPSENSE_CREF_COLD_BIT); // Cref_cold HiZ
		
		//  CHARGE  CSENSE
		CAPSENSE_PORT_REG |= (1 << CAPSENSE_CREF_HOT_BIT);   // Csense HIGH must be first!
		CAPSENSE_DDR_REG  |= (1 << CAPSENSE_CREF_HOT_BIT);   // Csense OUTPUT (fast charging via 1k)
		_delay_us(CAPSENSE_CSENSE_CHARGE_TIME);              // .....
		
		//  CSENSE || CREF,  CHARGE  CREF
		CAPSENSE_DDR_REG  &= ~(1 << CAPSENSE_CREF_HOT_BIT);  // Csense INPUT
		CAPSENSE_PORT_REG &= ~(1 << CAPSENSE_CREF_HOT_BIT);  // Csense HiZ
		CAPSENSE_DDR_REG  |=  (1 << CAPSENSE_CREF_COLD_BIT); // Cref_cold GND
		_delay_us(CAPSENSE_CSENSE_DISCHARGE_TIME);           // .....
		
		//  TEST Cref charged? or chargecounter Overflow?
		if(CAPSENSE_PIN_REG & (1 << CAPSENSE_CREF_HOT_BIT))
		SET_STATE(STATE_CREF_CHARGED);
		if(chargecounter >= CAPSENSE_CHARGECOUNTER_MAX)
		SET_STATE(STATE_CREF_TIMEOUT);
		chargecounter++;
	}
	//     DISCHARGE CREF
	CAPSENSE_DDR_REG  |=   (1 << CAPSENSE_CREF_HOT_BIT); // All OUTPUT
	CAPSENSE_PORT_REG &=  ~(1 << CAPSENSE_CREF_HOT_BIT); // All GND
	_delay_ms(CAPSENSE_CREF_DISCHARGE_TIME);	         // ...................
	return chargecounter;
}

void usart_init()
{
	/*     SET USART BAUDRATE      */
	UBRRH = (uint8_t) ((USART_BAUD_CALC) >> 8);
	UBRRL = (uint8_t) USART_BAUD_CALC;
	/*     ENABLE  DOUBLE PRECISION 4 HIGH BAUDRATES, LOOK ***    */
	UCSRA |= 1 << U2X;
	/*     ENABLE TRANSMITTER     */
	UCSRB |= 1 << TXEN;
	/*    SET  FRAME  FORMAT  8N2  */
	UCSRC |= (1 << USBS) | (3 << UCSZ0);
}

void usart_TX_char(char payload)
{
	/*      WAIT FOR EMPTY BUFFER      */
	while(!(UCSRA & (1 << UDRE)));
	UDR = payload;
}

void usart_byte2hex(uint8_t byte2hex)
{
	/*    CONVERT UNSIGNED INTEGER 2 HEX NUMBERS  AND SHOW  IT    */
	const uint8_t mask = 0xF;
	uint8_t temp2hex = (byte2hex >> 4 & mask);
	usart_TX_char(temp2hex < 0xA ? temp2hex+'0' : temp2hex-0xA+'A');
	temp2hex = byte2hex & mask;
	usart_TX_char(temp2hex < 0xA ? temp2hex+'0' : temp2hex-0xA+'A');
}

void process_data(uint16_t data)
{			
	/*  LOWPASSFILTER   */
	static uint32_t filter1;
	filter1 = filter1 ? ((filter1 << 3) - filter1 + data) >> 3 : CAPSENSE_CHARGECOUNTER_MAX; // simple hotstart
		
	/*  OUTPUT DATA     */
	uint16_t mapped_value = (filter1 - CAPSENSE_OFFSETVALUE) >> 4;
	mapped_value = mapped_value > 0xFF ? 0xFF : mapped_value;
	mapped_value = (char)mapped_value;
	#ifdef USART4DEBUG
		usart_byte2hex(mapped_value);
		usart_TX_char('\n');
	#else
		usart_TX_char(mapped_value);
	#endif
}

int main(void)
{
	/*     INIT     */
	usart_init();
	
	/*     DO JOB     */
    while (1) 
    {			
	uint16_t temp = capsense_charge_Cref();		
	process_data(temp);
    }
}



