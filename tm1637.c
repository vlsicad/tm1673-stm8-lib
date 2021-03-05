


#include "STM8S.h"
#include "tm1637.h"
#include "stm8s_delay.h"

#define CLOCK_DELAY 1

#define LOW 0
#define HIGH 1


// Table which translates a digit into the segments
const unsigned char cDigit2Seg[] = {0x3f, 0x6, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};



/// <summary>
/// Sleep for the specified time (in ms). It internally just calls the CLK delay function defined in clock.h
/// </summary>
void usleep(int milliseconds)
 {
	delay_us(milliseconds);
 }

/// <summary>
/// Initialize tm1637 with the clock and data pins
/// </summary>
void tm1637Init(void)
{

	GPIO_DeInit(GPIOB);
	GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
//  GPIO_Init (GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_IT);
	GPIO_DeInit(GPIOB);
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
//  GPIO_Init (GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_IT);

	//return 0;
}

/// <summary>
/// Start wire transaction
/// </summary>
static void tm1637Start(void)
{
	GPIO_WriteHigh(GPIOB, GPIO_PIN_4); // datapin
	GPIO_WriteHigh(GPIOB, GPIO_PIN_5); // clkpin
	usleep(CLOCK_DELAY);
	GPIO_WriteLow(GPIOB, GPIO_PIN_5); // clkpin
}

/// <summary>
/// Stop wire transaction
/// </summary>
static void tm1637Stop(void)
{
	// clock low
	GPIO_WriteLow(GPIOB, GPIO_PIN_5);
	usleep(CLOCK_DELAY);
	// data low
	GPIO_WriteLow(GPIOB, GPIO_PIN_4);
	usleep(CLOCK_DELAY);
	// clock high
	GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
	usleep(CLOCK_DELAY);
	// data high
	GPIO_WriteHigh(GPIOB, GPIO_PIN_4);
} 

/// <summary>
/// Get data ack
/// </summary>
static unsigned char tm1637GetAck(void)
{
	unsigned char bAck = 1;

	// read ack
	// clock to low
	GPIO_WriteLow(GPIOB, GPIO_PIN_5);
	// data as input
	
	usleep(CLOCK_DELAY);

	// clock high
	GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
	usleep(CLOCK_DELAY);
	// clock to low
	GPIO_WriteLow(GPIOB, GPIO_PIN_5);
	return bAck;
}


/// <summary>
/// Write a unsigned char to the controller
/// </summary>
static void tm1637WriteByte(unsigned char b)
{
	unsigned char i;

	for (i=0; i<8; i++)
	{
		// clock low
		GPIO_WriteLow(GPIOB, GPIO_PIN_5);
		// LSB to MSB
		if (b & 1) 
			GPIO_WriteHigh(GPIOB, GPIO_PIN_4);
		else
			GPIO_WriteLow(GPIOB, GPIO_PIN_4);
		usleep(CLOCK_DELAY);
		// clock high
		GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
		usleep(CLOCK_DELAY);
		b >>= 1;
	}
}

/// <summary>
/// Write a sequence of unsigned chars to the controller
/// </summary>
static void tm1637Write(unsigned char *pData, unsigned char bLen)
{
	unsigned char b, bAck;
	bAck = 1;
	tm1637Start();
	for (b=0; b<bLen; b++)
	{
		tm1637WriteByte(pData[b]);
		bAck &= tm1637GetAck();
	}
	tm1637Stop();
} 

/// <summary>
/// Set brightness (0-8)
/// </summary>
void tm1637SetBrightness(unsigned char b)
{
	unsigned char bControl;
	if (b == 0) 
		// display off
		bControl = 0x80; 
	else
	{
		if (b > 8) b = 8;
		bControl = 0x88 | (b - 1);
	}
	tm1637Write(&bControl, 1);
} 

/// <summary>
/// Display a string of 4 digits and optional colon
/// by passing a string such as "12:34" or "45 67"
/// </summary>
void tm1637ShowDigits(char *pString)
{
	// commands and data to transmit
	unsigned char b, bTemp[16]; 
	unsigned char i, j;

	j = 0;
	// memory write command (auto increment mode)
	bTemp[0] = 0x40;
	tm1637Write(bTemp, 1);

	// set display address to first digit command
	bTemp[j++] = 0xc0;
	for (i=0; i<5; i++)
	{
   	// position of the colon
		if (i == 2) 
		{
	  	// turn on correct bit
			if (pString[i] == ':')
			{
		 	// second digit high bit controls colon LEDs
				bTemp[2] |= 0x80;
			}
		}
		else
		{
			b = 0;
			if (pString[i] >= '0' && pString[i] <= '9')
			{
			// segment data
				b = cDigit2Seg[pString[i] & 0xf]; 
			}
			bTemp[j++] = b;
		}
	}
	// send to the display
	tm1637Write(bTemp, j); 
}