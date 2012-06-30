/*
 * Motor_Laser.h
 *
 *  Created on: 2012-06-30
 *      Author: Labuser
 */

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

#define ERROR 10
#define STEPSIZE 16


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

}

void MotorXStep(int dirx)
{
	if(!dirx)// forward step
	{
		P2OUT |= SLEEP;
		P1OUT &= ~DIR_A;
		P1OUT |= STEP_A;
		P1OUT &= ~STEP_A;
		P2OUT &= ~SLEEP;
	}
	if(dirx) // reverse step
	{
		P2OUT |= SLEEP;
		P1OUT |= DIR_A;
		P1OUT |= STEP_A;
		P1OUT &=~STEP_A;
		P2OUT &= ~SLEEP;
	}
}
void MotorYStep(int diry)
{
	if(!diry)// forward step
	{
		P2OUT |= SLEEP;
		P1OUT &= ~DIR_B;
		P1OUT |= STEP_B;
		P1OUT &= ~STEP_B;
		P2OUT &= ~SLEEP;
	}
	if(diry) // reverse step
	{
		P2OUT |= SLEEP;
		P1OUT |= DIR_B;
		P1OUT |= STEP_B;
		P1OUT &=~STEP_B;
		P2OUT &= ~SLEEP;
	}
}


void Motor(int qy, int qx,int dirx, int diry, int sumx, int sumy)// this big function commands the steps and how big each x step is related to y.
{
	int x,y,xcount=sumx,ycount=sumy; // the counter xcount and ycount are used to see how close we are to the target,
									// where x and y are just counters to count up to the X/Y slope value * STEPSIZE

	if(qy>qx)
	{
		int m=qy/qx*STEPSIZE; // m gives you the slope, if Qy is bigger than Qx, make it a Y/X slope

		while(qy>=(ycount+ERROR)||qy>=(ycount-ERROR)||qx>=(xcount+ERROR)||qx>=(xcount-ERROR)) // continue while not within the error margins in either X or Y of the target
		{
			if(ycount>(qy*STEPSIZE) || xcount>(qy*STEPSIZE))
				ErrorHandling(11);
			for(y=0;y<=m;y++)
			{
				MotorYStep(diry);
				if(diry)
					ycount--;
				else
					ycount++;
			}
			MotorXStep(dirx);
			if(dirx)
				xcount--;
			else
				xcount++;
		}
	}
	else if(qy<qx) // if Qx is bigger than Qy, make it a X/Y slope
	{
		int m=qx/qy*STEPSIZE;
		while(qy>=(ycount+ERROR)||qy>=(ycount-ERROR)||qx>=(xcount+ERROR)||qx>=(xcount-ERROR))
				{
				if(ycount>(qy*STEPSIZE) || xcount>(qy*STEPSIZE))
						ErrorHandling(12);
					for(x=0;x<=m;x++)
					{
						MotorXStep(dirx);
						if(dirx)
							xcount--;
						else
							xcount++;
					}
					MotorYStep(diry);
					if(diry)
						ycount--;
					else
						ycount++;
				}
	}

}
