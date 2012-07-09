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






typedef struct {                        // A simple variant data type that can hold byte, word, or long
	    uint8_t len;                        // Length of data received
	    union {                             //
	        uint8_t b;                      // Byte
	        uint16_t n;                     // Short
	        uint32_t l;                     // Long
	                                        //
	    } val;                              //
	} variant_t;                            //


  short ContinueCounter=0;



void SendShort(variant_t *v, HANDLE file, DWORD written, DWORD read);  

void finished(HANDLE file,DWORD written);




void RecieveGo(HANDLE file, DWORD read);


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
        long random=65;
        long n;
        queue<short>ratio_x;
        queue<short>ratio_y;
        queue<short>cnt;
        for(n=0;n<random;n++)
        {
        cnt.push(rand()%500+1);
            ratio_x.push(rand()% 30+1);
            ratio_y.push(rand()% 30+1);
            cout << n << endl;
        }
        
        int ack[1];
        *ack=0;
        //initialize variables
       variant_t sendx, sendy, sendcnt;
//================================================

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
                
                
                         
         sendcnt.val.n=cnt.front();                       //sends the count for the stack
         SendShort(&sendcnt, file, written, read);
         
         
         sendx.val.n=ratio_x.front();                     //sends the x ratio for the stack
         SendShort(&sendx, file, written, read);
         
         
         sendy.val.n=ratio_y.front();                     // sends the y ratio for the stack
         SendShort(&sendy, file, written, read);
    
         
         ratio_x.pop();
         cnt.pop();
         ratio_y.pop();
         n++;
    }

         
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    return 0;
}

void SendShort(variant_t *v, HANDLE file, DWORD written, DWORD read)             
{                                      
    
    
    
    int test1, test2;
    uint8_t array1[1];
    uint8_t array2[1];
    uint8_t *b = &v->val.b; 
            
    for(int n=0;n<2;n++)
    {
            if(n==0)
            {
                    test1=*b++;

            }
            if(n==1)
            {
                    test2=*b;
            }
      }
      
      
        *array1=test1;
        *array2=test2;
        WriteFile(file, array1, sizeof(array1), &written, NULL);
        RecieveGo(file, read);
        WriteFile(file, array2, sizeof(array2), &written, NULL);
        RecieveGo(file, read);
}
void RecieveGo(HANDLE file, DWORD read)
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
     cout << test << endl;
}
