/*
 * Motor_Laser.h
 *
 *  Created on: 2012-06-30
 *      Author: Labuser
 */
#define DELAY_TERM 100000// this term is important, delays the steps by a certain amount of cycles



#ifndef MOTOR_LASER_H_
#define MOTOR_LASER_H_
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

#define ERROR 100

#endif /* MOTOR_LASER_H_ */

void ConfigEverything(void);
void putbyte(unsigned int c);
int getbyte(void);
void ResetMotors(void)
{
	P2OUT&=~SLEEP;								//turn on MCU
	P2OUT |= RESET;								// reset location of MCU
	  _delay_cycles(50);						// wait for reset
	  P2OUT &= ~(RESET);						// turn off reset bit
	  P2OUT|=SLEEP;
}


void ErrorHandling(int er_num) // errors out, gives a error number and dies
{
	putbyte(er_num);
	P3IFG |=BIT0; //the P3 interrupt kills the program

}


void ConfigureMotorsStep_16(void)
{
	P2OUT |= SLEEP;
	P1OUT|= MS1_A + MS2_A+MS1_B+MS2_B;
	P2OUT &= ~SLEEP;
}
void ConfigureMotorsStep_4(void)
{
	P2OUT |= SLEEP;
	P1OUT|= MS2_A+MS2_B;
	P1OUT&= ~(MS1_A+MS1_B);
	P2OUT &= ~SLEEP;
}

void ConfigureMotorsStep_2(void)
{
	P2OUT |= SLEEP;
	P1OUT&= ~ (MS2_A+MS2_B);
	P1OUT|= MS1_A+MS1_B;
	P2OUT &= ~SLEEP;
}
void LaserWait(void)// Fill all these later when I have proper pins to use
{
	_delay_cycles(50);
}

void LaserOff(void)
{
	_delay_cycles(50);
}






void MotorXStep(int dirx)
{
	_delay_cycles(DELAY_TERM);
	if(!dirx)// forward step
	{
		_delay_cycles(DELAY_TERM);
		PJOUT &= ~BIT1;
		_delay_cycles(DELAY_TERM);
		P3OUT &=~BIT3;
		_delay_cycles(DELAY_TERM);
		P3OUT |= BIT3;
		_delay_cycles(DELAY_TERM);
		P3OUT &= ~BIT3;
		_delay_cycles(DELAY_TERM);

	}
	if(dirx) // reverse step
	{

		_delay_cycles(DELAY_TERM);
		PJOUT |= BIT1;
		_delay_cycles(DELAY_TERM);
		P3OUT &=~BIT3;
		_delay_cycles(DELAY_TERM);
		P3OUT |= BIT3;
		_delay_cycles(DELAY_TERM);
		P3OUT &= ~BIT3;
		_delay_cycles(DELAY_TERM);

	}
}
void MotorYStep(int diry)
{
	if(!diry)// forward step
	{

		_delay_cycles(DELAY_TERM);
		PJOUT &= ~BIT2;
		_delay_cycles(DELAY_TERM);
		P3OUT &= ~BIT4;
		_delay_cycles(DELAY_TERM);
		P3OUT |= BIT4;
		_delay_cycles(DELAY_TERM);
		P3OUT &= ~BIT4;
		_delay_cycles(DELAY_TERM);

	}
	if(diry) // reverse step
	{
		_delay_cycles(DELAY_TERM);
		PJOUT |= BIT2;
		_delay_cycles(DELAY_TERM);
		P3OUT &= ~BIT4;
		_delay_cycles(DELAY_TERM);
		P3OUT |= BIT4;
		_delay_cycles(DELAY_TERM);
		P3OUT &= ~BIT4;
		_delay_cycles(DELAY_TERM);

	}
}


void Motor_one_big_step(int qy,int qx,int dirx,int diry)// this big function commands the steps and how big each x step is related to y.
{
															// sumx and sumy for the first trial are set to 0.

	volatile int n;
	volatile int xcount;
	volatile int ycount;
				if(dirx)
				{

					for(xcount=0;xcount<qx;xcount++)
					{
						MotorXStep(dirx);
					}
				}
				else if(!dirx)
				{
					for(xcount=0;xcount<qx;xcount++)
					{
						MotorXStep(dirx);
					}
				}







				if(diry)
				{
					for(ycount=0;ycount<qy;ycount++)
					{
						MotorYStep(diry);
					}
				}
				else if(!diry)
				{
					for(ycount=0;ycount<qy;ycount++)
					{
						MotorYStep(diry);
					}
				}
}
