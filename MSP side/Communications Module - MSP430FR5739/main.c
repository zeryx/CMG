#include <msp430fr5739.h>

#include <stdlib.h>//standard libraries
#include <stdint.h>

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



#define ACK 0xff



int ContinueCounter=0;
static int InterruptCounter=0;
static int IC=0;
static int EndLoop=0;



#define TIMEOUT 20
#define STACKSIZE 5





typedef struct { // A simple variant data type that can hold byte, word, or long
	uint8_t len; // Length of data received
	union { //
		uint8_t b; // Byte
		uint16_t n;
		uint32_t l; // Long (4 bytes)
	} val; //
} variant_t;

void HandShake(void);




#include "queue.h"// customer headers, I have them lower than the other startup stuff because I want to use the stuff above.
#include "Motor_Laser.h"
#include "Config.h"

void AddToBackOfQueue(Queue ratio_x, Queue ratio_y, Queue dirqx, Queue dirqy, Queue xcnt);



	//=========================================
	//main start


	void main(void) {
	ConfigEverything();
	TIMERCONFIG();
	ConfigureMotorsStep_16();

	unsigned short datx = 0, daty = 0, countx;
	Queue ratio_x, ratio_y, dirqx, dirqy, xcnt;
	unsigned char dirx = 0, diry = 0;

	//==========================================
	//configure data handler



	xcnt =CreateQueue(STACKSIZE);
	ratio_x = CreateQueue(STACKSIZE); 	// change STACKSIZE in define, dictates how big the queue can be.
	ratio_y = CreateQueue(STACKSIZE); 	//
	dirqx = CreateQueue(STACKSIZE); 	//
	dirqy = CreateQueue(STACKSIZE); 	//



	while(1)//main loop start
	{
			ResetMotors();
			int x = 0;
			HandShake();
			LaserWait();
			while (!IsFull(ratio_y)) // while queue is not full, fill er up
			{
				variant_t combine;
				x++;


				if (x & 1) {
					read_var(&combine);
					countx = (combine.val.n);
					Enqueue(countx, xcnt); //put length of step cycle in xcnt
					read_var(&combine);
					dirx = combine.val.n >> 15;
					datx = (combine.val.n & ~0x8000);
					Enqueue(datx, ratio_x);
					Enqueue(dirx, dirqx);
				} else {
					read_var(&combine);
					diry = combine.val.n >> 15;
					daty = (combine.val.n & ~0x8000);
					Enqueue(daty, ratio_y); // put buffer byte in queue y
					Enqueue(diry, dirqy);
				}

			}
			//=======================================
			//end of queue filling, send it all to the motor now



			while (!IsEmpty(ratio_y) && !IsEmpty(ratio_x)) {
				Motor_one_big_step(FrontAndDequeue(ratio_x), FrontAndDequeue(ratio_y), FrontAndDequeue(dirqx), FrontAndDequeue(dirqy), FrontAndDequeue(xcnt));



				//=====================================
				//we're done with the motor now for that stack, lets check to see if there is any more data in the PC queue!



				if ((UCA0IFG & UCSTTIFG))						//figure out how to get this to work
				AddToBackOfQueue(ratio_x, ratio_y, dirqx, dirqy, xcnt);
			}
			LaserOff();
			MotorsOff();


				//======================================= main loop end, restart to the front, but first, standby mode ACTIVATED!

	}
}


void AddToBackOfQueue(Queue ratio_x, Queue ratio_y, Queue dirqx, Queue dirqy, Queue xcnt) //similar to the initial enqueue procedure, but enqueues
																						// in a separate function sepecifically for maintaining max stacksize.
{
	variant_t longx, longy, longc;
	int diry, daty, dirx, datx, countx;



	IC=1;						//this stuff tests to make sure that there is still stuff in the PC queue, and if there isn't, continues.
	read_var(&longc);
	IC=0;
	InterruptCounter=0;


	countx = (longc.val.n );
	Enqueue(countx, xcnt); //put buffer byte in queue x
	read_var(&longx);
	dirx = longx.val.n >> 15;
	datx = longx.val.n & ~0x8000;
	Enqueue(datx, ratio_x);
	Enqueue(dirx, dirqx);
	read_var(&longy);
	diry = longy.val.n >> 15;
	daty = longy.val.n & ~0x8000;
	Enqueue(daty, ratio_y);
	Enqueue(diry, dirqy);
}





void HandShake(void)			//this small function is used to maintain parity with the PC, it sends a acknowledgement byte after each byte recieved,
								//signifying its ready for another byte.
{
	ContinueCounter++;
	putbyte(ContinueCounter);
	putbyte(ACK);
	if(ContinueCounter==255) // just to make it easy, if the ContinueCounter (sent as an int) gets larger than 255, overflow to 0 and count back up
			ContinueCounter=0;
}




