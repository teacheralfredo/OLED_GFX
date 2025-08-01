
uint8_t initCmdSSD1306[] = {// initialization for an SSD1306 controller
	0x80,0xae,				// turn off difplay
	0x80,0xd4,0x80,0x80,	// setDisplayClockDiv	
	0x80,0xa8,0x80,0x3f,	// setMultiplex, 63 lines
	0x80,0xd3,0x80,0x00,	// display offset
	0x80,0x40,				// set display start line register to 0
	0x80,0xda,0x80,0x12,	// setcompins, default
	0x80,0x20,0x80,0x02,	// page addressing 0x20, 0x02
	0x80,0x8d,				// set charge pump
	0x80,0x14,				// enable charge pump
	0x80,0xa4,				// set display normal display mode
	0x80,0xa6,				// set display normal display mode
	0x80,0xaf,				// turn on display
	0x80,0xa1,0x80,0xc8		// assuming pins on top
};

#define OLED_GFX_RENDER_FLAG				0x01

OLED_GFX::OLED_GFX(int bus_ch, int dev_addr) {
	channel = bus_ch;
	address = dev_addr;
	polling_ms = 100;
}

void OLED_GFX::init(void) {
	flag = 0;
	set_flag = 0;
	clr_flag = 0;
	state = s_detect;
}

void OLED_GFX::process(Driver *drv) {
	I2CDev *i2c = i2cPtr = (I2CDev *)drv;
	switch (state) {
		case s_detect:				// detect i2c device
			if (i2c->detect(channel, address) == ESP_OK) 	state = s_cmd_init;
			else 	state = s_error;
			break;

		case s_cmd_init:
			if (i2cPtr->write(0, address, initCmdSSD1306, sizeof(initCmdSSD1306)) == ESP_OK) {
				error = false;		// clear error flag
				initialized = true;	// set initialized flag
				state = s_idle;		// goto idle state
			}else state = s_error;
			break;

		case s_idle:
			// check set/clr flag
			if (clr_flag & 0x7f) {
				FLAG_CLR(flag, clr_flag & 0x7f);
				clr_flag = 0;
			}
			if (set_flag & 0x7f) {
				FLAG_SET(flag, set_flag & 0x7f);
				set_flag = 0;
			}

			// check pending flag
			if (IS_FLAG_SET(flag, OLED_GFX_RENDER_FLAG)) {
				FLAG_CLR(flag, OLED_GFX_RENDER_FLAG);
				// render
//				if (render(i2c) != ESP_OK) {
//					state = s_error;
//				}
			}
			break;

		case s_error:
			error = true;			// set error flag
			initialized = false;	// clear initialized flag
			tickcnt = get_tickcnt();// get current tickcnt
			state = s_wait;			// goto wait and retry with detect state
			break;

		case s_wait:
			// delay 1s then retry detect
			if (is_tickcnt_elapsed(tickcnt, 1000)) {
				state = s_detect;
			}
			break;
	}
}

void OLED_GFX::clear(int chipSet, int orientation) {
	sh1106 = chipSet == 1;
	initCmdSSD1306[sizeof(initCmdSSD1306)-3] = orientation ? 0xa0 : 0xa1;	// flip display ?
	initCmdSSD1306[sizeof(initCmdSSD1306)-1] = orientation ? 0xc0 : 0xc8;
	i2cPtr->write(0, address, initCmdSSD1306, sizeof(initCmdSSD1306));		// reinitialize
	memset(bufr, 0, sizeof(bufr));	// clear buffer for 128 spaces
	for(int i=0; i<8; i++){			// foreach of 8 lines
		setCursor(i,0,0);			// set the address to the start of line
		*bufPtr++ = 0x40;			// data to follow
		bufPtr += 128;				// advance the pointer to clear whole line
		i2cPtr->write(0, address, bufr, bufPtr-bufr);	// write set cursor cmd and 128 0s
	}
	sRow = sCol = mRow = mCol = gRow = gCol = 0;	// reset all cursors
	set_flag |= (0x80 | OLED_GFX_RENDER_FLAG);
}

void OLED_GFX::setCursor(int row, int col, int lines){
	hPos = (lines == 1 ? 6 : 9) * (col % (lines == 1 ? 21 : 14));
	if( sh1106)	hPos += 2;			// shift 2 bits if SH1106
//	printf("mrow %d, mcol %d, hPos %d\n", row, col, hPos);
	bufPtr = bufr;
	memset(bufr, 0, sizeof(bufr));
	*bufPtr++ = 0x80;
    *bufPtr++ = hPos & 0x0f;      	// column address low
    *bufPtr++ = 0x80;
    *bufPtr++ = (hPos >> 4)+ 0x10;	// column address high + 0x10
    *bufPtr++ = 0x80;
    *bufPtr++ = (row & 7) + 0xb0; 	// page (row) address + 0xb0
	if(lines < 2)	return;

	bf2Ptr = buf2;
	memset(buf2, 0, sizeof(buf2));
	*bf2Ptr++ = 0x80;
    *bf2Ptr++ = hPos & 0x0f;      	// column address low
    *bf2Ptr++ = 0x80;
    *bf2Ptr++ = (hPos >> 4) + 0x10;	// column address high + 0x10
    *bf2Ptr++ = 0x80;
    *bf2Ptr++ = ((row+1) & 7) + 0xb0;	// page address + 0xb0
}

void OLED_GFX::print5x7(char chr){
	if(chr == '\n'){		// handle new line, clear to end of line and position cursor to start of next line
        bufPtr += 6 * (21-sCol) + 2;
		sCol = 21;
		return;
	}
	uint16_t i, c = chr-32;	// index into font_5x7 array, skip non-printable characters
	if(c > sizeof(font_5x7)/5)
		return;				// unprintable
	for(i=0; i<5; i++)
		*bufPtr++ = font_5x7[c*5+i];
	*bufPtr++ = 0;			// interchacter space
	sCol++;
}

void OLED_GFX::printSmall(int row, int col, char *str){
	if(col > 0)		sCol = (col - 1) % 21;		// keep in the range 0-20 no matter what the user sends
	if(row > 0)		sRow = (row - 1) & 7;		// keep in the range 0-7 no matter what the user sends
	if(*str == 0)	return;						// a print with an empty string is just a set cursor
	while(true){
		setCursor(sRow, sCol, 1);
		*bufPtr++ = 0x40;	// data follows
		while(*str){
			print5x7(*str++);
			if(sCol > 20){
//		printf("1sRow %d, sCol %d, char %c, bufr index %d\n", sRow, sCol, *str, bufPtr-bufr);
				i2cPtr->write(0, address, bufr, bufPtr-bufr);
				sRow++;		// wrap to next line
				sRow &= 7;	// if past last line, wrap to first line
				sCol = 0;
//		printf("2sRow %d, sCol %d, char %c, bufr index %d\n", sRow, sCol, *str, bufPtr-bufr);
				break;
			}
		}
		if(*str)	continue;
		i2cPtr->write(0, address, bufr, bufPtr-bufr);
		break;
	}
}

uint8_t getBit(const uint8_t *bitArray, uint16_t bitNumber){
	uint8_t bite = bitArray[bitNumber>>3];
	uint8_t bit = bite >> (bitNumber & 7);
	return bit & 1;
}

void OLED_GFX::print9x15(char chr){
	if(chr == '\n'){		// handle new line, clear to end of line and position cursor to start of next line
        bufPtr += 9 * (14-mCol) + 2;
        bf2Ptr += 9 * (14-mCol) + 2;
		mCol = 15;
		return;
	}
	uint16_t i, j, k, c = (chr & 0x7f) - 32;

	for(i=0; i<14; i++){	
		k = 107*i*8 + c*9;		// byte 0 offset into the bit array
		for(j=0; j<9; j++){
		 	if(getBit(font_9x15, k++)){
				if(i<8)	bufPtr[j] |= (1<<i);
				else	bf2Ptr[j] |= (1<<(i-8));
			}
		}
	}
	bufPtr+=9;
	bf2Ptr+=9;
	mCol++;
}

void OLED_GFX::printLarge(int row, int col, char *str){
	if(col > 0)		mCol = (col - 1) % 14;		// keep in the range 0-12 no matter what the user sends
	if(row > 0)		mRow = (row - 1) & 7;		// keep in the range 0-7 no matter what the user sends
	if(*str == 0)	return;						// a print with an empty string is just a set cursor function
	while(true){
//		printf("row %d, col %d\n", mRow, mCol);
		setCursor(mRow, mCol, 2);	// addresses  in the output puffers
		*bufPtr++ = 0x40;	// data follows - upper page
		*bf2Ptr++ = 0x40;	// data follows - lower page
		while(*str){		// for every character in the string
			print9x15(*str++);
			if(mCol > 14){	// wrap text to the next line
				i2cPtr->write(0, address, bufr, bufPtr-bufr);
				i2cPtr->write(0, address, buf2, bf2Ptr-buf2);
				mRow = (mRow + 2) & 7;
				mCol = 0;
				break;
			}
		}
		if(*str)	continue;	// go back to the while(true);
		i2cPtr->write(0, address, bufr, bufPtr-bufr);	// the last few characters, <14
		i2cPtr->write(0, address, buf2, bf2Ptr-buf2);
		break;					// exit the  while(true) loop and returnt53  v vv 
	}
}

void OLED_GFX::print     (int row, int col, char *str, int sz){
	if(sz != 0)	printLarge(row, col, str);
	else		printSmall(row, col, str);
}

// functions added to support GFX character display on the ST7789 TFT display
// This allows Thai, English, or mixed characters to be displayed
// It also allows various font styles and sizes to be displayed

void OLED_GFX::bufferChar(uint8_t raw) {
	if( raw < fnt->first || raw >fnt->last ) return;// better might print error char
	uint8_t c = raw - fnt->first;					// set for GFX font bitmap range
	uint16_t h = fnt->glyph[c].height, w = fnt->glyph[c].width;	// character height
	for(uint16_t y=0; y<h && y<64; y++){			// scan the bits in the bitmap table
		for(uint16_t x=0; x<w; x++){				// for the width
			uint16_t bitNum = y*w+x;				// find the bit offset
			uint16_t index = fnt->glyph[c].bitmapOffset + (bitNum>>3);	// byte offset is bit offset >>3
			bool bit = fnt->bitmap[index] & (0x80 >> (bitNum & 7));	// state of bit at (x,y)
			uint16_t shift = fnt->glyph[c].yOffset + y + fnt->yAdvance/5*4;
			if(shift > maxY)	maxY = shift;
			if(shift < minY)	minY = shift;
			if(bit)									// if bit is set in the GFX bitmap, set the bit in the output buffer
				gfx_buffer[bufferLen + x + fnt->glyph[c].xOffset] |= 1ULL<<shift;
		}
	}
	bufferLen += fnt->glyph[c].xAdvance;			// advance the output buffer pointer
}

void OLED_GFX::printGFX(int row, int col, char *message){
	uint8_t *p = (uint8_t *)message;
	if(col > 0)		gCol = (col - 1) % 21;		// keep in the range 0-12 no matter what the user sends
	if(row > 0)		gRow = (row - 1) & 7;		// keep in the range 0-6 no matter what the user sends
	if(*p == 0)		return;						// a print with an empty string is just a set cursor function

	memset(gfx_buffer, 0, sizeof(gfx_buffer));	// clear the output buffer
	bufferPtr = 0; // this is the pointer to where the data to display starts
	bufferLen = 0;	// this is the pointer to where we start filling the buffer.
	minY = 32000;
	maxY = 0;

	// create a bit map from the text message and the font files 

	while(*p  && bufferLen < 128)	// stop if terminal null character or bitmap buffer near capacity
		if(*p == 0xe0 && (p[1] == 0xb8 || p[1] == 0xb9)){	// is it a Thai character?
			fnt = &thFont;	// switch to Thai characters	// switch to Thai GFX struct
			if(p[1] == 0xb9)	bufferChar(p[2] - 0x41);
			if(p[1] == 0xb8)	bufferChar(p[2] - 0x81);
			p += 3;
		}else{
			fnt = &enFont;	// switch to ASCII characters
			bufferChar(*p++);
		}

	// printf to the console for inspecting the bitmap generated by the text
	// help visualizing during development - then printf code can be removed or commented out
	printf("message width %d, minY %d, maxY %d\n", bufferLen, minY, maxY);
	for(int i=0; i<64; i++){								// print the output bitmap for testing
		for(int j=0; j<bufferLen && j<160; j++){			// unreadable if the line wraps, only print up to ? columns
			if(gfx_buffer[j] & (1ULL<<i)) printf("g");
			else printf("--");
		}
		printf("\n");
	}
	int nPages = (maxY + 7) >> 3;						// how many lines to display depends on the GFX font max height
	for(int page=0; page<nPages; page++){
		setCursor(gRow+page, gCol, 1);					// addresses  in the output puffers
		*bufPtr++ = 0x40;								// data follows
		for(int i=0; i<bufferLen && i<128; i++){
			*bufPtr++ = 0xff & (gfx_buffer[i] >> (page<<3)); // move bits from gfx buffer to display
		}
		i2cPtr->write(0, address, bufr, bufPtr-bufr);	// the last few characters, <14
	}
}

char * OLED_GFX::num2str(double val, uint8_t wid, uint8_t dig, uint8_t fmt){
	static char buf[40], *p; // buffer for results
	if( wid > sizeof(buf) - 1)	wid = sizeof(buf) - 1;
	switch(fmt){
		case 0:	// integer
			sprintf(buf, "%*ld", wid, (long)val);
			break;
		case 1: // hex
			sprintf(buf, "%*lX", wid, (long)val);
			break;
		case 2: // hex with leading 0
			sprintf(buf, "%0*lX", wid, (long)val);
			break;
		case 3: // fixed point
			sprintf(buf, "%*.*f", wid, dig, val);
			break;
		case 4: // exponential
			sprintf(buf, "%*.*E", wid, dig, val);
			break;
		default:
			p = buf;
			while(wid--)	*p++ = '?';
			*p = 0;
	}
	return buf;
}
