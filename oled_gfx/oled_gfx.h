#include "driver.h"
#include "device.h"
#include "i2c-dev.h"

#ifndef _GFXFONT_H_
#define _GFXFONT_H_
typedef struct {
	uint16_t bitmapOffset;     ///< Pointer into GFXfont->bitmap
	uint8_t  width;            ///< Bitmap dimensions in pixels
    uint8_t  height;           ///< Bitmap dimensions in pixels
	uint8_t  xAdvance;         ///< Distance to advance cursor (x axis)
	int8_t   xOffset;          ///< X dist from cursor pos to UL corner
    int8_t   yOffset;          ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct { 
	uint8_t  *bitmap;      ///< Glyph bitmaps, concatenated
	GFXglyph *glyph;       ///< Glyph array
	uint8_t   first;       ///< ASCII extents (first char)
    uint8_t   last;        ///< ASCII extents (last char)
	uint8_t   yAdvance;    ///< Newline distance (y axis)
} GFXfont;
#endif

class OLED_GFX : public Device {
	private:
		enum {	s_detect, s_cmd_init, s_idle, s_error, s_wait } state;
		I2CDev *i2cPtr;
		TickType_t tickcnt;
		uint8_t flag, set_flag, clr_flag;
		bool sh1106;
		// properties required for print
		int sRow, sCol, mRow, mCol, gRow, gCol, hPos;	// cursor positions for small fixed, medium fixed, and gfx fonts
		uint8_t bufr[135], *bufPtr;				// buffer to output a line: 6 bytes start address, 0x40, 128 bytes data
		uint8_t buf2[135], *bf2Ptr;				// for second line of Large characters
		// properties required for printGFX
		uint16_t bufferPtr, bufferLen, minY, maxY;
		uint64_t gfx_buffer[160];
		const GFXfont *fnt;

	public:
		// constructor
		OLED_GFX(int bus_ch, int dev_addr);
		void init(void);
		void process(Driver *drv);

		//unused but seemingly required methods - stubbed here in header
		int prop_count(void){return 0;}
		bool prop_name(int index, char *name){return false;}
		bool prop_unit(int index, char *unit){return false;}
		bool prop_attr(int index, char *attr){return false;}
		bool prop_read(int index, char *value){return false;}
		bool prop_write(int index, char *value){return false;}

		// methods	
		void clear(int chipSet, int orientation);									// blockly
		void setCursor (int row, int col, int lines);
		void print5x7  (char chr);
		void print9x15 (char chr);
		void printSmall(int row, int col, char *str);
		void printLarge(int row, int col, char *str);
		void print     (int row, int col, char *str, int sz);						// blockly
		void bufferChar(uint8_t c);
		void printGFX  (int row, int col, char *str);								// blockly
		static char *num2str(double val, uint8_t wid, uint8_t dig, uint8_t fmt);	// blockly
};

