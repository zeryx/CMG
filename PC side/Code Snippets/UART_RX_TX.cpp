/* great relatively compact port read/writer, use the ReadFile/WriteFile format indicated below to read and write with the port
*/
//if you just want to use it, skip all the top gobbledegook and get to the meat and potatoes at the bottom




// this union splits a short into 2 bytes




#include <stdint.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#define STRICT
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinBase.h>
using namespace std;
#include <iostream>


typedef struct {                        // A simple variant data type that can hold byte, word, or long
	    uint8_t len;                        // Length of data received
	    union {                             //
	        uint8_t b;                      // Byte
	        uint16_t n;                     // Short
	        uint32_t l;                     // Long
	                                        //
	    } val;                              //
	} variant_t;                            //




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




void SendShort(variant_t *v, HANDLE file, DWORD written);  

int main(int argc, char **argv) {
    int ch;
     
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


  
  /* since were using USB and its almost always com 4, we can use this really simple windows.h 
  API stuff to read/write to ports, no reason to change anything since it works as it should,
  should be good enough for brandon to work with */
  //to send bytes, use this format:
  //unsigned char tx_variable[1];
  // *tx_variable=25;
  //WriteFile(file, tx_variable, sizeof(tx_variable), &written, NULL);
  variant_t sendx, sendy;

        unsigned char ack[1];
        *ack=0;
        WriteFile(file, ack, sizeof(ack), &written, NULL);
        sendx.val.n=(0xffff&4000)| 0x1; // 4000 with a direction of -
        sendy.val.n=(0xffff&7000);// 7000 with a direction of +
        SendShort(&sendx, file, written);
        SendShort(&sendy, file, written);
        
        
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  // to read bytes, use this format:
  // unsigned char rx_variable[1];
  //ReadFile(file, rx_variable, sizeof(rx_variable), &read, NULL);
  // cout << *rx_variable << endl;
  // that prints out the value stored in rx_variable, which is the line buffer.







    return 0;
}

void SendShort(variant_t *v, HANDLE file, DWORD written)             
{                                      
    
    
    
    
    
	int n=2;
	unsigned char array;
    uint8_t *b = &v->val.b;             
    do {
        *array=*b++;
        WriteFile(file, array, sizeof(array), &written, NULL);
        Sleep(500);                
    } while(--n);                      
}
