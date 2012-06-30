#include <msp430fr5739.h>

#include <stdlib.h>
#include <stdint.h>
#include "queue.h"
#include "Motor_Laser.h"
//define bits as MCU pins

#define STEP_A	BIT0
#define STEP_B	BIT1
#define DIR_A	BIT2
#define DIR_B	BIT3
#define MS1_A	BIT4
#define MS1_B	BIT5
#define MS2_A	BIT6
#define MS2_B	BIT7
#define ENABLE	BIT3
#define SLEEP	BIT4
#define RESET	BIT5

typedef struct {                        // A simple variant data type that can hold byte, word, or long
	    uint8_t len;                        // Length of data received
	    union {                             //
	        uint8_t b;                      // Byte
	        uint16_t n;                     // Word (2 bytes)
	        uint32_t l;                     // Long (4 bytes)
	    } val;                              //
	} variant_t;




void ConfigEverything(void);
void putbyte(unsigned int c);
int getbyte(void);
void ConfigWDT(void);	// also actually use the WDT when you move off the shitty fraunchpad

//actually do something with error handling


//initalizing the mechanical peripherals

void read_var(variant_t *v);


void main(void)
{
	ConfigEverything();
	ConfigWDT();
	ConfigureMotorsStep_16();


	int end=0,datx,daty,sumx,sumy; // function cease command
	Queue Qx,Qy,dirqx,dirqy;
	unsigned char dirx,diry;


	//configure data handler
	ResetMotors();
	putbyte(0xff); // all clear
	while(1)
	{
		int x=0;
		end=getbyte(); // gets a 0 from the computer, unless its the end, then its a 1
		if(end)
		break;


		int StackSize=getbyte();	//creates the queue to the size specified,
		Qx= CreateQueue(StackSize);	// will set perminently later
		Qy= CreateQueue(StackSize);	//
		dirqx=CreateQueue(StackSize);
		dirqy=CreateQueue(StackSize);

		while(!(IsFull(Qx) || IsFull(Qy))) // while queue is not full, fill er up
		{
			variant_t combine;
			x++;
			read_var(&combine);


			if(combine.len==4)
			{



				if(x&1)
				{
					dirx=0x1&combine.val.l;
					datx=combine.val.l << 1;
					Enqueue(datx, Qx);	//put buffer byte in queue x
					Enqueue(dirx, dirqx);
				}
				if(!x&1)
				{
					diry=0x1&combine.val.l;
					daty=combine.val.l >> 1;
					Enqueue(daty,Qy);// put buffer byte in queue y
					Enqueue(diry, dirqy);
				}
			}
			if(combine.len==2)
			{



					if(x&1)
					{
						dirx=0x1&combine.val.n;
						datx=combine.val.n >> 1;
						Enqueue(datx,Qx);	//put buffer byte in queue x
						Enqueue(dirx, dirqx);
					}
					if(!x&1)
					{
						diry=0x1&combine.val.n;
						daty=combine.val.n >> 1;
						Enqueue(daty,Qy);// put buffer byte in queue y
						Enqueue(diry, dirqy);
					}
			}
			if(combine.len==1)
						{


							if(x&1)
							{
								dirx=0x1&combine.val.b;
								datx=combine.val.b >> 1;
								Enqueue(datx,Qx);	//put buffer byte in queue x
								Enqueue(dirx, dirqx);
							}
							if(!x&1)
							{
								diry=0x1&combine.val.b;
								daty=combine.val.b >> 1;
								Enqueue(daty,Qy);// put buffer byte in queue y
								Enqueue(diry, dirqy);
							}
						}

		}



		putbyte(0xff);	// all clear, waiting for start confirmation



		while(!(IsEmpty(Qy) || IsEmpty(Qx)))
		{
			if(diry)
				sumy=sumy-Front(Qy);
			if(!diry)
				sumy=sumy+Front(Qy);
			if(dirx)
				sumx=sumx-Front(Qx);
			if(!dirx)
				sumx=sumx+Front(Qx);

			Motor(FrontAndDequeue(Qx), FrontAndDequeue(Qy), FrontAndDequeue(dirqx), FrontAndDequeue(dirqy), sumx, sumy);

		}
		LaserOff();

	}
}










	void ConfigWDT(void)
	{
		WDTCTL = WDTPW + WDTHOLD;
	 }


	#pragma vector=PORT4_VECTOR
	__interrupt void BUTTON(void)		//start on button interrupt
	{
		P4IFG &=~(BIT0);
		__bic_SR_register_on_exit(LPM3_bits);
	}



	#pragma vector=PORT3_VECTOR
	__interrupt void PORT3(void)		//kill if error catch is bad
	{
		__bis_SR_register(LPM4_bits);
		_no_operation();
}












void ConfigEverything(void)
{

		//configure clocks
		CSCTL0_H = 0xA5; // Unlock register
	  CSCTL1 |= DCOFSEL0 + DCOFSEL1; // Set max. DCO setting
	  CSCTL2 = SELS_3 + SELM_3; // MCLK = DCO
	  CSCTL3 =  DIVS_0 + DIVM_0; // set all dividers
	  CSCTL0_H = 0x01; // Lock Register


	  //configure MCU pins
	  P1DIR|=STEP_A+STEP_B+DIR_A+DIR_B+MS1_A+MS1_B+MS2_A+MS2_B;
	  P2DIR|=ENABLE+SLEEP+RESET;

	  P1SEL0 &= ~(STEP_A+STEP_B+DIR_A+DIR_B+MS1_A+MS1_B+MS2_A+MS2_B);// make sure that function is selected as external pin
	  P1SEL1 &= ~(STEP_A+STEP_B+DIR_A+DIR_B+MS1_A+MS1_B+MS2_A+MS2_B);//
	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	  	 //
	  P2SEL0 &= ~(ENABLE+SLEEP+RESET);								 //
	  P2SEL1 &= ~(ENABLE+SLEEP+RESET);								 //

	  P2OUT &= ~(ENABLE); 									//turn on enable input to device
	  P1OUT &= ~(MS1_A+MS1_B+MS2_A+MS2_B);		//initialize the step size to 1/1
	  P2OUT|=SLEEP;								//turn off MCU while not in use




	  //configure button pins for initial interrupt enable
	  P4DIR &=~(BIT0);
	  P4IE|=BIT0;

	  // configure P3 pins for generic error message interrupt
	  P3DIR &=~(BIT0);
	  P3IE|=BIT0;




	//configure UART pins
	P2DIR |= BIT0;
	P2SEL1 |= BIT0 + BIT1;
	P2SEL0 &= ~(BIT0+BIT1);


	//configure UART
	UCA0CTLW0|= UCSWRST;
    UCA0BR0 = 52; // 8000000/(9600*16), get the int throw away remainder
    UCA0BR1 = 0; // high byte bit duration is (MCLK/BAUD) / 256, get the int, throw away the remainder
    UCA0MCTLW = 0x4911; // UCBRFx = 1, UCBRSx = 0x49, UCOS16 = 1
    UCA0CTLW0 = UCSSEL_2;





}


void read_var(variant_t *v)             // Get a variant
{                                       //
    unsigned n = getbyte();                // First byte is length
    v->len = n;                         // Save it
    if(n < 1 || n > 4) return;          // Exit if out of range
    uint8_t *b = &v->val.b;             // Make pointer to data
    do {                                //
        *b++ = getbyte();                  // Get a byte
    } while(--n);                       // Until all received
}




void putbyte(unsigned int c)
{
    while(!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = c;
    UCA0IFG &= ~UCTXIFG;
}



int getbyte(void)
{
    while(!(UCA0IFG & UCSTTIFG));
    _delay_cycles(50000);
    const int c = UCA0RXBUF;
    UCA0RXBUF=0;
    UCA0IFG &= ~(UCRXIFG+UCSTTIFG);
     return c;
}


