#ifndef _COMMON_16BIT_COLORS_H_
#define _COMMON_16BIT_COLORS_H_
/*
Many libraries uses 16bit colors so I decided to share this table
between libraries and will be included just once.
NOTE: This version include the ILI9341_t3 colors for compatibility...
*/

#define	BLACK		(uint16_t)0x0000
#define	WHITE   	(uint16_t)0xFFFF
#define	NAVY   		(uint16_t)0x000F
#define	DARKGREEN   (uint16_t)0x03E0
#define	DARKCYAN   	(uint16_t)0x03EF
#define	MAROON   	(uint16_t)0x7800
#define	PURPLE   	(uint16_t)0x780F
#define	ORANGE   	(uint16_t)0xFD20
#define	PINK   		(uint16_t)0xF81F
#define	OLIVE   	(uint16_t)0x7BE0
#define	BLUE    	(uint16_t)0x001F
#define	RED     	(uint16_t)0xF800
#define	GREEN   	(uint16_t)0x07E0
#define	CYAN    	(uint16_t)0x07FF
#define	MAGENTA 	(uint16_t)0xF81F
#define	YELLOW  	(uint16_t)0xFFE0
#define	BRIGHT_RED	(uint16_t)0xF810
#define	LIGHT_GREY	(uint16_t)0xC618//0x8410;
#define	DARK_GREY	(uint16_t)0x7BEF//0x4208;  
#define	GREENYELLOW	(uint16_t)0xAFE5
    
#endif
