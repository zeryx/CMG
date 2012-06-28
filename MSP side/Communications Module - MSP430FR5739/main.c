#include <msp430fr5739.h>

#include <stdlib.h>
#include "queue.h"
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





void ConfigEverything(void);
void putbyte(unsigned int c);
int getbyte(void);
void ConfigWDT(void);	// also actually use the WDT when you move off the shitty fraunchpad

//actually do something with error handling
void ErrorHandling(int er_num);


//initalizing the mechanical peripherals
void ResetMotors(void);
void LaserWarmup(void);



// the motor stuff, do it later
void MotorX(int dat);
void MotorY(int dat);
void LaserOff(void);
void LaserOn(void);


void main(void)
{
	ConfigEverything();
	ConfigWDT();

	int end=0; // function cease command
	Queue Qx,Qy;




	//configure data handler
	ResetMotors();
	LaserWarmup();
	putbyte(0xff); // all clear
	int n=0;
	while(1)
	{
		end=getbyte(); // gets a 0 from the computer, unless its the end, then its a 1
		if(end)
		break;
		int StackSize=getbyte();	//creates the queue to the size specified,
		Qx= CreateQueue(StackSize);	// will set perminently later
		Qy= CreateQueue(StackSize);	//

		while(!(IsFull(Qx) || IsFull(Qy))) // while queue is not full, fill er up
		{
			n++;
			int buff=getbyte();	// get buffer byte from PC
			if(n&1)
				Enqueue(buff,Qx);	//put buffer byte in queue x
			else
				Enqueue(buff,Qy);// put buffer byte in queue y

		}



		putbyte(0xff);	// all clear, waiting for start confirmation



		while(!(IsEmpty(Qy) || IsEmpty(Qx)))
		{
			LaserOn();
			MotorX(FrontAndDequeue(Qx));
			MotorY(FrontAndDequeue(Qy));

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

void ErrorHandling(int er_num)
{
	putbyte(er_num);
	P3IFG |=BIT0;

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
    UCA0IFG &= ~(UCRXIFG+UCSTTIFG);
     return c;
}


void ResetMotors(void)
{
	P2OUT&=~SLEEP;								//turn on MCU
	P2OUT |= RESET;								// reset location of MCU
	  _delay_cycles(50);						// wait for reset
	  P2OUT &= ~(RESET);						// turn off reset bit
	  P2OUT|=SLEEP;
}
void LaserWarmup(void)// Fill all these later when I have proper pins to use
{
	_delay_cycles(50);
}

void LaserOff(void)
{

}

void LaserOn(void)	//
{

}

void MotorX(int dat)//
{

}

void MotorY(int dat)//
{

}
