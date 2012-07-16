/* great relatively compact port read/writer, use the ReadFile/WriteFile format indicated below to read and write with the port
*/
//if you just want to use it, skip all the top gobbledegook and get to the meat and potatoes at the bottom




// this union splits a short into 2 bytes







#include <math.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <queue>
#include <Windows.h>
#include <iostream>
using namespace std;


typedef struct {                        // union for  turning a short or long into a series of bytes for UART transfer, use SendShort for utilization.
	    uint8_t len;                        // Length of data received
	    union {                             //
	        uint8_t b;                      // Byte
	        uint16_t n;                     // Short
	        uint32_t l;                     // Long
	                                        //
	    } val;                              //
	} variant_t;                            //


  short ContinueCounter=0;                    //global handshake counter








#include "system_constants.h"
#include "steps.h"



#define STACKSIZE 5




void system_error(char *name) {
// Retrieve, format, and print out a message from the last error.  The 
// `name' that's passed should be in the form of a present tense noun 
// (phrase) such as "opening file".
//
    char *ptr = NULL;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        0,
        GetLastError(),
        0,
        (char *)&ptr,
        1024,
        NULL);

    fprintf(stderr, "\nError %s: %s\n", name, ptr);
    LocalFree(ptr);
}








    int primenumber(int number);
    void SendShort(variant_t *v, HANDLE file, DWORD written, DWORD read);  //my function handler stuff
    void finished(HANDLE file,DWORD written);
    void RecieveGo(HANDLE file, DWORD read);
    






     // main start, most of this is for the UART COM port stuff
     int main(int argc, char **argv) {
    int ch;
    srand(time(NULL));
    HANDLE file;
    COMMTIMEOUTS timeouts;
    DWORD read, written;
    DCB port;
    HANDLE keyboard = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE screen = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    char port_name[128] = "\\\\.\\COM4";
    char init[] = ""; // e.g., "ATZ" to completely reset a modem.

    if ( argc > 2 )
        sprintf(port_name, "\\\\.\\COM%c", argv[1][0]);

    // open the comm port.
    file = CreateFile(port_name,
        GENERIC_READ | GENERIC_WRITE,
        0, 
        NULL, 
        OPEN_EXISTING,
        0,
        NULL);

   // get the current DCB, and adjust a few bits to our liking.
    memset(&port, 0, sizeof(port));
    port.DCBlength = sizeof(port);
    if ( !GetCommState(file, &port))
        system_error("getting comm state");
    if (!BuildCommDCB("baud=9600 parity=n data=8 stop=1", &port))
        system_error("building comm DCB");
    if (!SetCommState(file, &port))
        system_error("adjusting port settings");

    // set short timeouts on the comm port.
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 1;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;
    if (!SetCommTimeouts(file, &timeouts))
        system_error("setting port time-outs.");

    // set keyboard to raw reading.
    if (!GetConsoleMode(keyboard, &mode))
        system_error("getting keyboard mode");
    mode &= ~ ENABLE_PROCESSED_INPUT;
    if (!SetConsoleMode(keyboard, mode))
        system_error("setting keyboard mode");

    if (!EscapeCommFunction(file, CLRDTR))
        system_error("clearing DTR");
    Sleep(200);
    if (!EscapeCommFunction(file, SETDTR))
        system_error("setting DTR");
        
        //============================================== function starts
        //this stuff prepares the queue and all variables, the step.h stuff will replace the rand values
        
    
    queue<short>qratio_x; //using <queue> to make queues, much more practical than simple arrays
    queue<short>qratio_y;
    queue<short>qcnt;
        double inputx, inputy, pathx=0, pathy=0;
    int n, i, GCD, temp, tempa, tempb, a_r, b_r, stpx, stpy, min;
    short rat_x, rat_y, cnt;
    srand(time(NULL));
    
    
    
        for(n=0;n<=30;n++)
        {
                               
              
              inputx=(float)rand()/(float)RAND_MAX*rand()/1000;
              inputy=-((float)rand()/(float)RAND_MAX*rand())/1000;
              cout<<inputx << endl;
              cout<< inputy << endl;
              
              
              
              
              
              
              stpx=(short)stepsX(inputx, inputy, pathx);
              stpy=(short)stepsY(inputy, pathy);
              pathx=pathx+inputx;
              pathy=pathy+inputy;
              
    //end of first bit of code
              
        
        
        //start of second bit of code
        if(primenumber(stpx))
        {
            stpx=stpx+1;
            cout <<"stpx was prime"<< endl;
        }
        if(primenumber(stpy))
        {
            stpy=stpy+1;
            cout <<"stpy was prime"<< endl;
        }
        
        
        
        if((stpx % 2!=0) && (stpx % 3 !=0) && (stpx % 5 !=0))
        stpx=stpx+1;
       if((stpy % 2!=0) && (stpy % 3 !=0) && (stpy % 5 !=0))
        stpy=stpy+1;  
        
        
        
        a_r=stpx;
        b_r=stpy;
		if(abs(stpx)>abs(stpy))
		min=abs(stpy);
		else if(abs(stpx)<abs(stpy))
		min=abs(stpx);
		rat_x=abs(stpx);
		rat_y=abs(stpy);
		if(stpx!=0 || stpy!=0) // if neither are zero, then lets divide
		{
            for(i=1;i<min;i++) // for this scope, lets just divide by 2, screw the other numbers
                {
                
                    if(rat_x % i == 0 && rat_y % i == 0) // once we've gotten to the point where both x, or y
                                                         // are divisible by i, set GCD equal to that
                    {
                        GCD=i;
                    }


                }
                
                rat_x=rat_x/GCD;
                rat_y=rat_y/GCD;
                cout << " x = " << stpx << endl<< " y = " << stpy << endl;
                cout << "the greatest common denominator is " << GCD << endl;
                
            
            

            

            
            if(abs(stpx)>abs(stpy))
                cnt=abs(stpx)/rat_x;
                
            if(abs(stpx)<abs(stpy))
                cnt=abs(stpy)/rat_y;
    
                if(stpx<0)
                rat_x=-rat_x;
                if(stpy<0)
                rat_y=-rat_y;
            }
            else if(stpx==0)
            {
                cnt=1;
                rat_x=stpx;
                rat_y=0;  
                cout << "the greatest common denominator is " << "null y" << endl;
            }
            else if(stpy==0)
            {
                 cnt=1;
                 rat_y=stpy;
                 rat_y=0;
                 cout << "the greatest common denominator is " << "null x" << endl;
            }
            
            cout << rat_x << endl << rat_y << endl << cnt << endl<< endl;
            
            //==========================================================
            
            
            
            
            qratio_x.push(rat_x);
            qratio_y.push(rat_y);
            qcnt.push(cnt);
            }

        
        int ack[1];
        *ack=0;
        //initialize variables
       variant_t sendx, sendy, sendcnt;
    //================================================
    // main meat and potatoes
     n=0;
    RecieveGo(file, read);                                              
    while(!qratio_x.empty()) //keep going until all the points are sent
    {

                
                
                         
         sendcnt.val.n=qcnt.front(); 
         cout << qcnt.front() << endl;                      //sends the count for the stack
         SendShort(&sendcnt, file, written, read);
         
         if(qratio_x.front()<0)
         {
             qratio_x.front()=-qratio_x.front();
             qratio_x.front()=qratio_x.front() | 0x8000;
         }
         sendx.val.n=qratio_x.front();                     //sends the x ratio for the stack
         SendShort(&sendx, file, written, read);
         
                  if(qratio_y.front()<0)
         {
             qratio_y.front()=-qratio_y.front();
             qratio_y.front()=qratio_y.front() | 0x8000;
         }
         sendy.val.n=qratio_y.front();                     // sends the y ratio for the stack
         SendShort(&sendy, file, written, read);
    
         
         qratio_x.pop(); // pops the fronted values in the queues, decreasing their sizes and storing less in ram
         qcnt.pop();     //
         qratio_y.pop(); //
         n++;
    }
   
    

    return 0;
}






void SendShort(variant_t *v, HANDLE file, DWORD written, DWORD read) //Sends a signed short  to the UART comms, splits the short into two bytes
{                                      
    
    
    
    int test1, test2;
    uint8_t array1[1]; // first byte array created for transport via UART
    uint8_t array2[1]; // second byte array created for transport via UART
    uint8_t *b = &v->val.b; // pointer of b is set to the address of val.b in the variant_t struct
            
    for(int n=0;n<2;n++)
    {
            if(n==0)
            {
                    test1=*b++;//incriment the pointer of b so that it points to the second byte        

            }
            if(n==1)
            {
                    test2=*b;//sets the value of the second 
            }
      }
      
      
      
      
        *array1=test1; //set the value of test1 equal to array1[0]
        *array2=test2;//likewise for test2 and array2
        
        
        WriteFile(file, array1, sizeof(array1), &written, NULL);//sends shit out via UART
        RecieveGo(file, read);                                  //waits for recieve go to send the second byte
        WriteFile(file, array2, sizeof(array2), &written, NULL);//after recievego, send second byte
        RecieveGo(file, read);                                  //before continuing, makes sure that the MSP has everything
}





void RecieveGo(HANDLE file, DWORD read)// this handshake knows when to send information to the MSP, when it doesn't recieve go from the MSP,
                                          //it will stay in the loop and wait for death or an eventual recieve go command, it incriments so that there isn't any "erronous go" commands
{
      unsigned char RX[1]={0};
     ContinueCounter=ContinueCounter+1;
     int test;
     do
     {
      test=0;
      *RX=0;
         ReadFile(file, RX, sizeof(RX), &read, NULL);
         test=RX[0];
         Sleep(5);
     }while(test!=ContinueCounter);
     if(ContinueCounter==255)
		ContinueCounter=0;
}

int primenumber(int number)
{
    int truth;
    for(int i=1; i<number; i++)
    {
       if(number%i!=0)
          return 1;
       else 
          return 0;
    } 
}
