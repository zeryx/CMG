/* great relatively compact port read/writer, use the ReadFile/WriteFile format indicated below to read and write with the port
*/
//if you just want to use it, skip all the top gobbledegook and get to the meat and potatoes at the bottom




// this union splits a short into 2 bytes









#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <queue>
#include <Windows.h>
#include <iostream>



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
using namespace std;



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
        
        long num_of_points=65;
        long n;
        queue<short>ratio_x; //using <queue> to make queues, much more practical than simple arrays
        queue<short>ratio_y;
        queue<short>cnt;
        
        
        for(n=0;n<num_of_points;n++)
        {
            cnt.push(rand()%500+1);
            ratio_x.push(rand()% 30+1);
            ratio_y.push(rand()% 30+1);
        }
        
        int ack[1];
        *ack=0;
        //initialize variables
       variant_t sendx, sendy, sendcnt;
    //================================================
    // main meat and potatoes

    n=0;
    RecieveGo(file, read);                                              
    while(!ratio_x.empty()) //keep going until all the points are sent
    {
         if(n>=STACKSIZE)
         {
               RecieveGo(file, read); // for refilling the buffer automatically
               WriteFile(file, ack, sizeof(ack), &written, NULL);
               RecieveGo(file, read);
         }
                
                
                         
         sendcnt.val.n=cnt.front(); 
         cout << cnt.front() << endl;                      //sends the count for the stack
         SendShort(&sendcnt, file, written, read);
         
         
         sendx.val.n=ratio_x.front();                     //sends the x ratio for the stack
         SendShort(&sendx, file, written, read);
         
         
         sendy.val.n=ratio_y.front();                     // sends the y ratio for the stack
         SendShort(&sendy, file, written, read);
    
         
         ratio_x.pop(); // pops the fronted values in the queues, decreasing their sizes and storing less in ram
         cnt.pop();     //
         ratio_y.pop(); //
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
