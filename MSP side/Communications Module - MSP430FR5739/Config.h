void ConfigEverything(void) {

	//configure clocks
	CSCTL0_H = 0xA5; // Unlock register
	CSCTL1 |= DCOFSEL0 + DCOFSEL1; // Set max. DCO setting
	CSCTL2 = SELS_3 + SELM_3; // MCLK = DCO
	CSCTL3 = DIVS_0 + DIVM_0; // set all dividers
	CSCTL0_H = 0x01; // Lock Register

	//configure MCU pins
	P1DIR |= STEP_A + STEP_B + DIR_A + DIR_B + MS1_A + MS1_B + MS2_A + MS2_B;
	P2DIR |= ENABLE + SLEEP + RESET;

	P1SEL0 &=~(STEP_A + STEP_B + DIR_A + DIR_B + MS1_A + MS1_B + MS2_A + MS2_B); // make sure that function is selected as external pin
	P1SEL1 &~(STEP_A + STEP_B + DIR_A + DIR_B + MS1_A + MS1_B + MS2_A + MS2_B); //
																				//
	P2SEL0 &= ~(ENABLE + SLEEP + RESET); //
	P2SEL1 &= ~(ENABLE + SLEEP + RESET); //

	P2OUT &= ~(ENABLE); //turn on enable input to device
	P1OUT &= ~(MS1_A + MS1_B + MS2_A + MS2_B); //initialize the step size to 1/1
	P2OUT |= SLEEP; //turn off MCU while not in use

	//configure LED pins
	PJDIR |= BIT0+BIT1+BIT2+BIT3;
	P3DIR |= BIT3+BIT4+BIT5+BIT6+BIT7;
	P3OUT &= ~(BIT3+BIT4+BIT5+BIT6+BIT7);
	PJOUT &= ~(BIT0+BIT1+BIT2+BIT3);


	//configure UART pins
	P2DIR |= BIT0;
	P2SEL1 |= BIT0 + BIT1;
	P2SEL0 &= ~(BIT0 + BIT1);

	//configure UART
	UCA0CTLW0 |= UCSWRST;
	UCA0BR0 = 52; // 8000000/(9600*16), get the int throw away remainder
	UCA0BR1 = 0; // high byte bit duration is (MCLK/BAUD) / 256, get the int, throw away the remainder
	UCA0MCTLW = 0x4911; // UCBRFx = 1, UCBRSx = 0x49, UCOS16 = 1
	UCA0CTLW0 = UCSSEL_2;
	__bis_SR_register(GIE);
	SFRIE1|=WDTIE;
}


void putbyte(unsigned int c)
{
	while (!(UCA0IFG & UCTXIFG));
	UCA0TXBUF = c;
	UCA0IFG &= ~UCTXIFG;
}



int getbyte(void)
{
	while (!(UCA0IFG & UCSTTIFG)) if(EndLoop) break;
	_delay_cycles(10000);
	const int c = UCA0RXBUF;
	UCA0RXBUF = 0;
	UCA0IFG &= ~(UCRXIFG + UCSTTIFG);
	HandShake();
	return c;
}

void TIMERCONFIG(void)
{
	WDTCTL = WDTPW+WDTTMSEL+WDTIS_4;

}

#pragma vector=WDT_VECTOR
__interrupt void OverTime(void) //WDT, kills if longer than 1 second for a transfer, (IE PC is done sending)
{
	if(IC==1)
	{
		InterruptCounter++;
		if(InterruptCounter==TIMEOUT)
			EndLoop=1;
	}
}


void read_var(variant_t *v) // Get a variant
{
	int n = 2;
	v->len = n; // 2 bytes, a short, always
	uint8_t *b = &v->val.b; // Make pointer to data
	do { //
		*b++ = getbyte(); // Get a byte
	} while (--n); // Until all received
}
