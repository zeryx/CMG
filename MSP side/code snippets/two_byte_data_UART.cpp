// two byte packet transfer over UART
// I prefer this method because I made it myself, for larger packets use the one devin provided
// to do list, get the one devin linked



unsigned short temp;
unsigned char int;
   	  	 	   TXByte=(temp & 0xff);
    			putchar(TXByte);
    			TXByte=(temp >> 8) & 0xff;
    			putchar(TXByte);
    			
    			// temp gets its data from ADC10MEM, which is a long, remember 
				//you can't use floats on a shitty MSP, wait for the TMS to start that
