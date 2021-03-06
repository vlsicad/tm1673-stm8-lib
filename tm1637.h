

#include "stm8s.h" 

/// <summary>
/// Sleep for the specified time (in ms). It internally just calls the CLK delay function defined in clock.h
/// </summary>
void usleep(int milliseconds);

/// <summary>
/// Initialize tm1637 with the clock and data pins
/// </summary>
void tm1637Init(void);

/// <summary>
/// Start wire transaction
/// </summary>
 static void tm1637Start(void);

/// <summary>
/// Stop wire transaction
/// </summary>
 static void tm1637Stop(void);

/// <summary>
/// Get data acknowledgement
/// </summary>
 static unsigned char tm1637GetAck(void);

/// <summary>
/// Write a unsigned char to the controller
/// </summary>
 static void tm1637WriteByte(unsigned char b);

/// <summary>
/// Write a sequence of unsigned chars to the controller
/// </summary>
 static void tm1637Write(unsigned char *pData, unsigned char bLen);

/// <summary>
/// Set brightness (0-8)
/// </summary>
void tm1637SetBrightness(unsigned char b);

/// <summary>
/// Display a string of 4 digits and optional colon
/// by passing a string such as "12:34" or "45 67"
/// </summary>
void tm1637ShowDigits(char *pString);