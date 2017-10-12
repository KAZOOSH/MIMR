/*
 * attiny4313_footsense21.c
 *
 * Created: 10.10.2017 18:21:04
 * Author : 25mmHg
 * Fuses: lfuse = 0xDE, hfuse = 0xDF, efuse = 0xFF
 * Circuit without LED and XTAL. 
 * need XTAL only 4 USART@high baudrates
 *
 
        .---------------------------------.
        |		                          |
        |          ATTINY2313A            |
        |                                 |
        |                                 |
        |                                 |
        |     Pin3          PB2   PB0     |
        '------o-------------o-----o------'
               |             |     |
               |             |    .-.
               |             |    | | Rv = 1k  (for full  discharge time Cref = 1.5ms)
               |             |    | |
               |             |    '-'
               o             |     |
           usart TX	         | ||  |     
                             '-||--o-------------.
                               ||                |
                                                 |
               Cref = 0,47uF Folie               |
                                       .---------o---------.
                                       |                   |
                                       |                   |
                                       |                   |
                                       |      A sense      |
                                       |                   |
                                       |                   |
                                       '-------------------'
 *
 * (created by AACircuit v1.28.6 beta 04/19/05 www.tech-chat.de)
 *
 */ 

#define F_CPU 16000000UL

#define USART4DEBUG
#define USART_BAUDRATE 115200UL
#define USART_BAUD_CALC (F_CPU/(USART_BAUDRATE*8UL))-1 // ***without U2X use 16UL
#define USART_MAXNEGATIVESYMBOLS 64

#define TIMER1_TOP_VALUE 9999U //((uint16_t)(F_CPU /8)*(5/1000)-1) 5ms

#define SET_STATE(BIT) (state_OS |=  (1 << (BIT)))
#define TGL_STATE(BIT) (state_OS ^=  (1 << (BIT)))
#define CLR_STATE(BIT) (state_OS &= ~(1 << (BIT)))
#define TST_STATE(BIT) (state_OS &   (1 << (BIT)))

/*
#define SET_REGISTER(REG,BIT) (REG |=  (1 << (BIT)))
#define TGL_REGISTER(REG,BIT) (REG ^=  (1 << (BIT)))
#define CLR_REGISTER(REG,BIT) (REG &= ~(1 << (BIT)))
#define TST_REGISTER(REG,BIT) (REG &   (1 << (BIT)))
*/

#define STATE_DETECT_DYN 0
#define STATE_DETECT_FTH 1
#define STATE_NEGATIVE_DATA2GRAPH 2
#define STATE_CREF_CHARGED 3
#define STATE_CREF_TIMEOUT 4
//#define STATE_USART_STOP_TX 5
#define STATE_CAPSENSE_WARMUP_FINISH 6
#define STATE_TIMER1_INTERRUPT 7

#define CAPSENSE_WARMUP_SAMPLES 0x300U // f1 is stable
#define CAPSENSE_PORT_REG PORTB
#define CAPSENSE_DDR_REG DDRB
#define CAPSENSE_PIN_REG PINB
#define CAPSENSE_CREF_COLD_BIT 2
#define CAPSENSE_CREF_HOT_BIT  0 // via Rv, the sense area is also connected*/
#define CAPSENSE_CSENSE_CHARGE_TIME 1U // us
#define CAPSENSE_CSENSE_DISCHARGE_TIME 1U // us
#define CAPSENSE_CREF_DISCHARGE_TIME 3U // ms
#define CAPSENSE_CHARGECOUNTER_MAX 0x6FFFU //edit max 7FFF for 16bit
#define CAPSENSE_FILTER3_POS_TH 24 // must be signed 
#define CAPSENSE_FILTER3_NEG_TH -16 // must be signed 
#define CAPSENSE_N_FILTER3 8U

#define FLOATING_TH_EPSILON 2

#define LED_PORT_REG PORTD
#define LED_DDR_REG  DDRD
#define LED_PIN_REG  PIND
#define LED_BIT 4
#define CNY17_PORT_REG PORTD
#define CNY17_DDR_REG  DDRD
#define CNY17_BIT 5
#define OC1A_PORT_REG  PORTB
#define OC1A_BIT 3

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

void interrupt_init()
{
	TIMSK  |= (1<<OCIE1A);   // Timer 1A Compare Interrupt enable
	sei();                   // Global interrrupt enable
}

void timer1_init()
{
	TCCR1A |= (1<<COM1A0);    // Toggle Pin15 on Compare Match only for debugging
	TCCR1B |= (1<<WGM12);     // CTC Modus OCR1A = TOP
	TCCR1B |= (1<<CS11);      // Prescaler  8
	OCR1A = TIMER1_TOP_VALUE; // is 5ms
}


uint16_t get_floatingTH(uint16_t data2TH)
{
	static uint16_t maxdata = 0;
	maxdata = (data2TH > maxdata) ? data2TH : maxdata;
	static uint16_t mindata = 0xFFFF;
	mindata = (data2TH < mindata) ? data2TH : mindata;
	return((mindata + maxdata)>>1);
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
		/* WAIT FOR  STRING END  */
		while(*payload_ptr)
		{
			usart_TX_char(*payload_ptr);
			payload_ptr++;
		}
	}

	void usart_process2hex(uint16_t data2hex)
	{
		/*    CONVERT UNSIGNED INTEGER 2 HEX NUMBERS  AND SHOW  IT    */
		const uint16_t mask = 0xF;
		for(uint8_t i=0; i<4; i++)
		{
			uint8_t temp = (uint8_t)((data2hex >> (4*(3-i))) & mask); //edit
			usart_TX_char(temp < 0xA ? temp+'0' : temp-0xA+'A');
		}
	}
	
	void usart_process2bin(uint8_t data2bin)
	{
		/*    CONVERT UNSIGNED INTEGER 2 BINARY AND SHOW  IT    */
		for(int8_t i=7; i>-1; i--)
		{
			if(data2bin & (1<<i))
				usart_TX_char('1');
			else
				usart_TX_char('0');
		}			
	}

	void usart_prozess2graph(int16_t data2graph)
	{
		/*    CONVERT  INTEGER 2 LOG GRAPH WITH 1/4 BIT  ACURACY, PIN A CENTERED "0" AND SHOW  IT VERY NICE    */
		uint8_t symbols = 0;
		CLR_STATE(STATE_NEGATIVE_DATA2GRAPH);
				
		if(data2graph < 0)
		{
			data2graph = (uint32_t)-data2graph;
			SET_STATE(STATE_NEGATIVE_DATA2GRAPH);
		}		
		else
		{
			data2graph = (uint32_t) data2graph;
			CLR_STATE(STATE_NEGATIVE_DATA2GRAPH);
		}
		
		data2graph <<= 2;
		for (uint8_t i=0; data2graph > 3; i++)
		{
			symbols += 4; // "||||"
			if(data2graph < 8)
			{
				if(data2graph & 2)
				symbols += 2; // "||"
				if(data2graph & 1)
				symbols++; // "|"
			}
			data2graph >>= 1;
		}
			
		if(TST_STATE(STATE_NEGATIVE_DATA2GRAPH))		
		{
			for(uint8_t i = 0; i < USART_MAXNEGATIVESYMBOLS-symbols; i++)
				usart_TX_char(' ');
			for(uint8_t i = 0; i < symbols; i++)
				usart_TX_char('|'); // "|||....|||"
			usart_TX_char('0'); // NULL
		}		
		else
		{
			for(int i = 0; i < USART_MAXNEGATIVESYMBOLS; i++)
				usart_TX_char(' ');
			usart_TX_char('0'); // NULL
			for(int i = 0; i < symbols; i++)
				usart_TX_char('|'); // "|||....|||"
		}
	}
#endif

uint8_t process_data(uint16_t data)
{		
	static uint16_t floatingTH;
	static uint16_t sample;
	sample++;
	
	/*   STACKET  LOWPASSFILTER FAST AND SLOW    */	 
	static uint32_t filter2;
	filter2 = filter2 ? ((filter2 << 4) - filter2 +    data) >> 4 : CAPSENSE_CHARGECOUNTER_MAX; // simple hotstart
	static uint32_t filter1;
	filter1 = filter1 ? ((filter1 << 4) - filter1 + filter2) >> 4 : filter2; // simple hotstart
	filter1 = filter1 < filter2 ? filter1 + 1 : filter1; // adjust filter1 from filter2
		
	/*    FIRST DERIVATIVE     */
	static int16_t old_bp_data;
	int16_t first_derivate_data = filter1 - filter2 - old_bp_data;
	old_bp_data = filter1 - filter2;
	
	/*    FILTER3  WITH   RINGBUFFER     */
	int16_t filter3 = 0;
	static int16_t ring3[CAPSENSE_N_FILTER3];
	static uint8_t writepos;	
	ring3[writepos++] = first_derivate_data;
	if(writepos >= CAPSENSE_N_FILTER3)
	    writepos = 0;
	for(uint8_t readpos = 0; readpos < CAPSENSE_N_FILTER3; readpos++)
	    filter3 += ring3[readpos];
			
	/*    GET FLOATING THRESHOLD     */	
	if(sample == CAPSENSE_WARMUP_SAMPLES)
		SET_STATE(STATE_CAPSENSE_WARMUP_FINISH);	
	if(TST_STATE(STATE_CAPSENSE_WARMUP_FINISH) && (!filter3))
		floatingTH = get_floatingTH(filter2);	
		
	/*    DATA 2 FLOATING THRESHOLD    */
	if(filter2 < floatingTH - FLOATING_TH_EPSILON)
		SET_STATE(STATE_DETECT_FTH);
	else if(filter2 > floatingTH + FLOATING_TH_EPSILON)
		CLR_STATE(STATE_DETECT_FTH);
	
	/*    DATA 2 DYNAMIK THRESHOLD    */
	if((filter3 > CAPSENSE_FILTER3_POS_TH)||(filter3 < CAPSENSE_FILTER3_NEG_TH))
		SET_STATE(STATE_DETECT_DYN);
	else	
		CLR_STATE(STATE_DETECT_DYN);		
		
	#ifdef USART4DEBUG
	/*     OUTPUT DATA 4 DEBUG      */
	usart_TX_string("cnt: ");
	usart_process2hex(sample);
	usart_TX_string(" \traw: ");
	usart_process2hex(data);
	usart_TX_string(" \tTH: ");
	usart_process2hex(floatingTH);
	usart_TX_string(" \tstate_OS: ");
	usart_process2bin(state_OS);
	usart_TX_string(" \tf1: ");
	usart_process2hex(filter1);
	usart_TX_string("  \tf2: ");
	usart_process2hex(filter2);
	usart_TX_string("  \tlog2(f3):");
	usart_prozess2graph(filter3);
	usart_TX_char('\n');
	#endif	
	
	return(TST_STATE(STATE_DETECT_FTH) || TST_STATE(STATE_DETECT_DYN));
}

ISR(TIMER1_COMPA_vect)
{
	TGL_STATE(STATE_TIMER1_INTERRUPT);
}

int main(void)
{
	/*     INIT     */
	timer1_init();
	interrupt_init();
	#ifdef USART4DEBUG
		usart_init();
	#endif			
	LED_DDR_REG   |= (1 <<   LED_BIT);
	CNY17_DDR_REG |= (1 << CNY17_BIT);
	OC1A_PORT_REG |= (1 << OC1A_BIT);

	/*     DO JOB     */
    while (1) 
    {			
		/*    50HZ NOTCH, GET 2 CHARGES AT 180° PHASE DIFFERENCE    */   
		uint16_t sum = 0;
		for(uint8_t i = 0; i < 2; i++)
		{			
			while(TST_STATE(STATE_TIMER1_INTERRUPT));
			while(!(TST_STATE(STATE_TIMER1_INTERRUPT)));
			CLR_STATE(STATE_TIMER1_INTERRUPT);
			cli();	
				sum += capsense_charge_Cref();
			sei();
		}
		
		cli();
		/*    GENERATE OUTPUT    */		
		if(process_data(sum))
		{
			LED_PORT_REG   &= ~(1 <<   LED_BIT);
			CNY17_PORT_REG |=  (1 << CNY17_BIT);
		}
		else
		{
			LED_PORT_REG   |=  (1 <<   LED_BIT);
			CNY17_PORT_REG &= ~(1 << CNY17_BIT);
		}

		/*     WARMUP BLINGBLING     */
		if(!(TST_STATE(STATE_CAPSENSE_WARMUP_FINISH)))		
		{
			LED_PIN_REG    |=  (1 << LED_BIT);
			_delay_ms(5);
			LED_PIN_REG    |=  (1 << LED_BIT);
		}
		sei();
    }
}



