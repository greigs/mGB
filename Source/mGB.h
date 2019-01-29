
extern UBYTE saveData[513U];
//extern UBYTE wavData[512U];

extern UBYTE serialBuffer[256];
extern UBYTE serialBufferPosition;
extern UBYTE serialBufferReadPosition;

UBYTE statusByte;
UBYTE addressByte;
UBYTE valueByte;
UBYTE captureDataType;
UBYTE capturedAddress;
UBYTE updateDisplaySynthCounter;

void asmMain();
void asmUpdateSerial();
void asmLoadWav(UBYTE ost);
void asmUpdateMidiBuffer();
void asmUpdatePu1();
void asmUpdatePu2();
void asmUpdateWav();
void asmUpdateNoi();
void asmPlayNotePu1();

void testSynths();

#define DEBUG_MODE 1
#define GLOBAL_POSITION_YOFFSET -1
#define GLOBAL_POSITION_YOFFSET_BYTE 4

#define PU1 0
#define PU2 1
#define WAV 2
#define NOI 3

#define PU1_CURRENT_STATUS 0
#define PU1_CURRENT_NOTE   1
#define PU2_CURRENT_STATUS 2
#define PU2_CURRENT_NOTE   3
#define WAV_CURRENT_STATUS 4
#define WAV_CURRENT_NOTE   5
#define NOI_CURRENT_STATUS 6
#define NOI_CURRENT_NOTE   7

#define PBWHEEL_CENTER     0x80


#define RGB_LIGHT      RGB( 23, 29, 31)
#define RGB_DARK       RGB( 0, 0, 2)
#define RGB_HAWT       RGB( 31, 5, 31)
#define RGB_HAWTBLU    RGB( 0, 15, 31)
#define SPRITE_ARRL 0x79
#define SPRITE_ARRT 0x80
#define SPRITE_ARRT_START 26
#define SPRITE_ARRL_START 27
#define SPRITE_PUFREQ_LOW 1

UWORD freq[72] = {
  44, 	156,  262,  363,  457,  547,  631,  710,  786,  854,  923,  986,
  1046, 1102, 1155, 1205, 1253, 1297, 1339, 1379, 1417, 1452, 1486, 1517,
  1546, 1575, 1602, 1627, 1650, 1673, 1694, 1714, 1732, 1750, 1767, 1783,
  1798, 1812, 1825, 1837, 1849, 1860, 1871, 1881, 1890, 1899, 1907, 1915,
  1923, 1930, 1936, 1943, 1949, 1954, 1959, 1964, 1969, 1974, 1978, 1982,
  1985, 1988, 1992, 1995, 1998, 2001, 2004, 2006, 2009, 2011, 2013, 2015
};

UBYTE noteStatus[8];

UBYTE noiFreq[72] = { 
0x94,0x87,0x86,0x85,0x84,0x77,0x76,0x75,0x74,0x67,0x66,0x65,
0x64,0x57,0x56,0x55,0x54,0x47,0x46,0x45,0x44,0x37,0x36,0x35,
0x34,0x27,0x26,0x25,0x24,0x17,0x16,0x15,0x14,0x07,0x06,0x00,
0x9C,0x8F,0x8E,0x8D,0x8C,0x7F,0x7E,0x7D,0x7C,0x6F,0x6E,0x6D,
0x6C,0x5F,0x5E,0x5D,0x5C,0x4F,0x4E,0x4D,0x4C,0x3F,0x3E,0x3D,
0x3C,0x2F,0x2E,0x2D,0x2C,0x1F,0x1E,0x1D,0x1C,0x0F,0x0E,0x08
};

UBYTE pu1Env;
UBYTE pu2Env;
UBYTE noiEnv;
UBYTE noiMode;

UBYTE pu1Trig = 0x80;
UBYTE pu2Trig = 0x80;

UBYTE pu1Vel;
UBYTE pu2Vel;

BOOLEAN pu1NoteOffTrigger;
BOOLEAN pu2NoteOffTrigger;
BOOLEAN wavNoteOffTrigger;
BOOLEAN noiNoteOffTrigger;

UINT8 polyVoiceSelect;
UBYTE polyNoteState[3];

BOOLEAN pu1Sus;
BOOLEAN pu2Sus;
BOOLEAN noiSus;
BOOLEAN wavSus;

UBYTE outputSwitch[4] = {0x01,0x02,0x40,0x80};
UBYTE outputSwitchMute[4] = {0x01,0x02,0x40,0x80};
UBYTE outputSwitchValue[4] = {3,3,3,3};


UBYTE counterWavStart;
UBYTE wavShapeLast;
UBYTE wavDataOffset;
UBYTE pbWheelIn[4] = {0x80,0x80,0x80,0x80};
UBYTE pbWheelInLast[4] = {0x80,0x80,0x80,0x80};
BOOLEAN pbWheelActive[4] = {0x00,0x00,0x00,0x00};
UBYTE pbRange[4] = {2,2,2,12};
UBYTE pbNoteRange[8] = {0,0,0,0,0,0,0,0};
UWORD currentFreq;
UINT8 i;
UINT8 x;
UBYTE j;
UBYTE l;
UINT8 lastPadRead;
BOOLEAN joyState[8] = {0,0,0,0,0,0,0,0};
UBYTE wavSweepSpeed;
UWORD wavCurrentFreq;
UWORD currentFreqData[4];
const UBYTE wavData[256] = {
	0x01,0x23,0x45,0x67,
	0x89,0xAB,0xCD,0xEF,
	0xFE,0xDC,0xBA,0x98,
	0x76,0x54,0x32,0x10,

	0x22,0x55,0x77,0xAA,
	0xBB,0xDD,0xEE,0xFF,
	0xEE,0xDD,0xBB,0xAA,
	0x77,0x66,0x44,0x00,

	0x01,0x23,0x45,0x67,
	0x89,0xAB,0xCD,0xEF,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	
	0xFF,0xEE,0xDD,0xCC,
	0xBB,0xAA,0x99,0x88,
	0x77,0x66,0x55,0x44,
	0x33,0x22,0x11,0x00,
	
	0xFF,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0xFF,0xFF,0x00,0x00,
	
	0xFF,0xFF,0xFF,0xFF,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,
	0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,
	
    0x16,0x13,0xAA,0xB3, 
    0x25,0x81,0xE8,0x2A, 
    0x1B,0xEB,0xF8,0x85, 
    0xE1,0x28,0xFF,0xA4, 
    
    0x34,0x09,0x91,0xA7, 
    0x63,0xB8,0x99,0xA1, 
    0x3F,0xE4,0xD0,0x61, 
    0x66,0x73,0x59,0x7C, 
    
    0x86,0x04,0x2F,0xAC, 
    0x85,0x17,0xD6,0xA1, 
    0x03,0xCF,0x27,0xE4, 
    0xF8,0x27,0x89,0x2C, 
    
    0x30,0x1B,0xD4,0x93, 
    0xD3,0x6E,0x35,0x13, 
    0x53,0x05,0x75,0xB9, 
    0x79,0xCF,0x36,0x00, 

    0xD4,0x2C,0xC6,0x4E, 
    0x2C,0x12,0xE2,0x15, 
    0x8B,0xAF,0x3D,0xEF, 
    0x6E,0xF1,0xBF,0xD9, 
    
    0x43,0x17,0x2B,0xF3, 
    0x12,0xC2,0xCB,0x8C, 
    0x3B,0x43,0xF2,0xDF, 
    0x5D,0xF9,0xEF,0x31,

    0x6D,0x46,0xF6,0x7A, 
    0xEE,0x17,0x35,0xF4, 
    0xDA,0xFE,0x7C,0x28, 
    0xB8,0x55,0x12,0x57, 
    
    0xFF,0x82,0xBB,0x85, 
    0xEF,0xD4,0x7C,0xA1, 
    0x05,0xB4,0xFF,0xC1, 
    0x95,0x27,0x30,0x03
};

INT8 pu1Oct;
INT8 pu2Oct;
INT8 wavOct;
INT8 noiOct;

UINT16 counterWav;

BOOLEAN cueWavSweep;

const unsigned char data_barrow[16] =
{
  0x00,0x00,0xE7,0xE7,0x7E,0x7E,0x3C,0x3C,
  0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00
};

const unsigned char data_larrow[] =
{
  0xC0,0xC0,0xE0,0xE0,0x70,0x70,0xE0,0xE0,
  0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00
};

const unsigned char data_font[] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0xFC,0xFC,0xC6,0xC6,0xE6,0xE6,0xD6,0xD6,
  0xCE,0xCE,0xC6,0xC6,0x7E,0x7E,0x00,0x00,
  0x38,0x38,0x18,0x18,0x18,0x18,0x18,0x18,
  0x18,0x18,0x18,0x18,0x08,0x08,0x00,0x00,
  0xFC,0xFC,0x06,0x06,0x06,0x06,0xFE,0xFE,
  0xC0,0xC0,0xC0,0xC0,0x7C,0x7C,0x00,0x00,
  0xFC,0xFC,0x06,0x06,0x06,0x06,0x3E,0x3E,
  0x06,0x06,0x06,0x06,0xFC,0xFC,0x00,0x00,
  0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7E,0x7E,
  0x06,0x06,0x06,0x06,0x02,0x02,0x00,0x00,
  0xFE,0xFE,0xC0,0xC0,0xC0,0xC0,0xFE,0xFE,
  0x06,0x06,0x06,0x06,0xFE,0xFE,0x00,0x00,
  0x7E,0x7E,0xC0,0xC0,0xFE,0xFE,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0xFC,0xFC,0x00,0x00,
  0xFE,0xFE,0x06,0x06,0x0E,0x0E,0x1C,0x1C,
  0x18,0x18,0x18,0x18,0x08,0x08,0x00,0x00,
  0xFC,0xFC,0xC6,0xC6,0xC6,0xC6,0xFE,0xFE,
  0xC6,0xC6,0xC6,0xC6,0x7E,0x7E,0x00,0x00,
  0xFC,0xFC,0xC6,0xC6,0xC6,0xC6,0xFE,0xFE,
  0x06,0x06,0x06,0x06,0x0C,0x0C,0x00,0x00,
  0x7E,0x7E,0xC6,0xC6,0xFE,0xFE,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x82,0x82,0x00,0x00,
  0xFC,0xFC,0xC6,0xC6,0xFC,0xFC,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0xFC,0xFC,0x00,0x00,
  0x7E,0x7E,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
  0xC0,0xC0,0xC0,0xC0,0x7E,0x7E,0x00,0x00,
  0xF8,0xF8,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0xF8,0xF8,0x00,0x00,
  0x7E,0x7E,0xC0,0xC0,0xC0,0xC0,0xF8,0xF8,
  0xC0,0xC0,0xC0,0xC0,0x7E,0x7E,0x00,0x00,
  0x7E,0x7E,0xC0,0xC0,0xC0,0xC0,0xF8,0xF8,
  0xC0,0xC0,0xC0,0xC0,0x40,0x40,0x00,0x00,
  0x7C,0x7C,0xC0,0xC0,0xCE,0xCE,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x7C,0x7C,0x00,0x00,
  0xC4,0xC4,0xC6,0xC6,0xFE,0xFE,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x46,0x46,0x00,0x00,
  0xFE,0xFE,0x30,0x30,0x30,0x30,0x30,0x30,
  0x30,0x30,0x30,0x30,0xFE,0xFE,0x00,0x00,
  0x06,0x06,0x06,0x06,0x06,0x06,0x06,0x06,
  0x06,0x06,0x06,0x06,0xFC,0xFC,0x00,0x00,
  0xC6,0xC6,0xCC,0xCC,0xF8,0xF8,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x46,0x46,0x00,0x00,
  0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,0xC0,
  0xC0,0xC0,0xC0,0xC0,0x7E,0x7E,0x00,0x00,
  0xC6,0xC6,0xEE,0xEE,0xD6,0xD6,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x82,0x82,0x00,0x00,
  0xFC,0xFC,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x82,0x82,0x00,0x00,
  0x7C,0x7C,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0x7C,0x7C,0x00,0x00,
  0x7E,0x7E,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xFC,0xFC,0xC0,0xC0,0x40,0x40,0x00,0x00,
  0x7E,0x7E,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0x7E,0x7E,0x06,0x06,0x06,0x06,0x00,0x00,
  0xFE,0xFE,0xC6,0xC6,0xC6,0xC6,0xF8,0xF8,
  0xC6,0xC6,0xC6,0xC6,0x46,0x46,0x00,0x00,
  0x7E,0x7E,0xC0,0xC0,0xC0,0xC0,0x7C,0x7C,
  0x06,0x06,0x06,0x06,0xFC,0xFC,0x00,0x00,
  0xFE,0xFE,0x18,0x18,0x18,0x18,0x18,0x18,
  0x18,0x18,0x18,0x18,0x08,0x08,0x00,0x00,
  0x46,0x46,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xC6,0xC6,0xC6,0xC6,0xFC,0xFC,0x00,0x00,
  0x82,0x82,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xC6,0xC6,0x6C,0x6C,0x10,0x10,0x00,0x00,
  0x82,0x82,0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,
  0xD6,0xD6,0xEE,0xEE,0x44,0x44,0x00,0x00,
  0xC6,0xC6,0xEE,0xEE,0x6C,0x6C,0x38,0x38,
  0x6C,0x6C,0xEE,0xEE,0xC6,0xC6,0x00,0x00,
  0xC6,0xC6,0xC6,0xC6,0xC6,0xC6,0x7E,0x7E,
  0x0C,0x0C,0x18,0x18,0x30,0x30,0x00,0x00,
  0xFE,0xFE,0x0E,0x0E,0x1C,0x1C,0x38,0x38,
  0x70,0x70,0xE0,0xE0,0xFE,0xFE,0x00,0x00,
  0x82,0x82,0x44,0x44,0x28,0x28,0x10,0x10,
  0x28,0x28,0x44,0x44,0x82,0x82,0x00,0x00,
  0x00,0x00,0x38,0x38,0x7C,0x7C,0x7C,0x7C,
  0x7C,0x7C,0x38,0x38,0x00,0x00,0x00,0x00,
  0x08,0x08,0x14,0x14,0x22,0x22,0x41,0x41,
  0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
  0x18,0x18,0x24,0x24,0x24,0x24,0x42,0x42,
  0x81,0x81,0x00,0x00,0x00,0x00,0x00,0x00,
  0x0C,0x0C,0x14,0x14,0x24,0x24,0x44,0x44,
  0x87,0x87,0x00,0x00,0x00,0x00,0x00,0x00,
  0x03,0x03,0x0D,0x0D,0x31,0x31,0xC1,0xC1,
  0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,
  0x80,0x80,0xFF,0xFF,0x00,0x00,0x00,0x00,
  0x00,0x00,0xC0,0xC0,0x40,0x40,0x40,0x40,
  0x40,0x40,0x7F,0x7F,0x00,0x00,0x00,0x00,
  0x00,0x00,0xE0,0xE0,0x20,0x20,0x20,0x20,
  0x20,0x20,0x3F,0x3F,0x00,0x00,0x00,0x00,
  0x00,0x00,0xF0,0xF0,0x10,0x10,0x10,0x10,
  0x10,0x10,0x1F,0x1F,0x00,0x00,0x00,0x00,
  0x00,0x00,0xFC,0xFC,0x04,0x04,0x04,0x04,
  0x04,0x04,0x07,0x07,0x00,0x00,0x00,0x00,
  0x00,0x00,0x90,0x90,0x0D,0x0D,0x42,0x42,
  0xA9,0xA9,0x04,0x04,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,
  0x80,0x80,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0x80,0x80,0x80,0x80,0x80,0x80,
  0xC0,0xC0,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0x80,0x80,0x80,0x80,0xC0,0xC0,
  0xE0,0xE0,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0x80,0x80,0xC0,0xC0,0xE0,0xE0,
  0xF0,0xF0,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0xC0,0xC0,0xF0,0xF0,0xF8,0xF8,
  0xFC,0xFC,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0xF0,0xF0,0xF8,0xF8,0xFC,0xFC,
  0xFE,0xFE,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0xFC,0xFC,0xFE,0xFE,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0x7F,0x7F,0x00,0x00,
  0x00,0x00,0x3F,0x3F,0x7F,0x7F,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x1F,0x1F,0x3F,0x3F,0x7F,0x7F,
  0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x0F,0x0F,0x3F,0x3F,0x7F,0x7F,
  0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x07,0x07,0x1F,0x1F,0x3F,0x3F,
  0x7F,0x7F,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x03,0x03,0x0F,0x0F,0x3F,0x3F,
  0x7F,0x7F,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x01,0x01,0x03,0x03,0x0F,0x0F,
  0x3F,0x3F,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x01,0x01,0x01,0x01,0x07,0x07,
  0x1F,0x1F,0xFF,0xFF,0xFE,0xFE,0x00,0x00,
  0x00,0x00,0x00,0x00,0x10,0x10,0x38,0x38,
  0x6C,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x6C,0x6C,0x38,0x38,
  0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x70,0x00,0x18,0x00,0x18,0x00,0x18,
  0x00,0x18,0x00,0x18,0x00,0x08,0x00,0x00,
  0x00,0xFC,0x00,0x06,0x00,0x06,0x00,0xFE,
  0x00,0xC0,0x00,0xC0,0x00,0x7E,0x00,0x00,
  0x00,0xFC,0x00,0x06,0x00,0x06,0x00,0x3E,
  0x00,0x06,0x00,0x06,0x00,0xFC,0x00,0x00,
  0x00,0xC6,0x00,0xC6,0x00,0xC6,0x00,0x7E,
  0x00,0x06,0x00,0x06,0x00,0x02,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x03,0x03,0x04,0x04,0x05,0x05,
  0xF3,0xF3,0xAB,0xAB,0xAB,0xAB,0x89,0x89,
  0x04,0x04,0x03,0x03,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xFF,0xFF,0x00,0x00,0xEF,0xEF,
  0x0C,0x0C,0x7F,0x7F,0x0C,0x0C,0xFF,0xFF,
  0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0xC0,0xC0,0x20,0x20,0x90,0x90,
  0xD1,0xD1,0xD1,0xD1,0xD0,0xD0,0x90,0x90,
  0x20,0x20,0xC0,0xC0,0x00,0x00,0x00,0x00,
  0xB4,0xB4,0xF7,0xF7,0xE4,0xE4,0x44,0x44,
  0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,
  0x76,0x76,0x55,0x55,0x67,0x67,0x55,0x55,
  0x11,0x11,0x00,0x00,0x00,0x00,0x00,0x00,
  0x75,0x75,0x45,0x45,0x77,0x77,0x15,0x15,
  0x31,0x31,0x00,0x00,0x00,0x00,0x00,0x00,
  0x77,0x77,0x05,0x05,0x77,0x77,0x05,0x05,
  0x77,0x77,0x00,0x00,0x00,0x00,0x00,0x00,
  0x70,0x70,0x50,0x50,0x50,0x50,0x50,0x50,
  0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x18,0x18,0x18,0x18,0x00,0x00,
  0x00,0x00,0x70,0x70,0xF8,0xF8,0xF8,0xF8,
  0xF8,0xF8,0x70,0x70,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x50,0x50,0x20,0x20,
  0x50,0x50,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xF0,0x00,0xF0,0x00,0xF0,0x00,0xF0,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x0F,0x00,0x0F,0x00,0x0F,0x00,0x0F,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xF0,0x00,0xF0,0x00,0xF0,0x00,0xF0,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x0F,0x00,0x0F,0x00,0x0F,0x00,0x0F,
  0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,
  0x00,0x00,0x00,0x00,0x00,0xD8,0x00,0x70,
  0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00
};

const unsigned char versionnumber[16] =
{
	32,81,
	2,81,
	4,81,
	5,
	0,0,0,
	30,81,18,81,23,81 //T.H.M. (Theremin Hero Mod)
};

const UBYTE helpdata[10][18] = 
{
		{25,13,30,11,32,15,0,0,0,0,  0,0,0,0,0,0,0,0},
		{33,11,32,0,0,0,0,0,0,0,     0,0,0,0,13,13,1,2},
		{15,24,32,15,22,25,26,15,0,0,0,0,0,0,13,13,1,3},
		{29,33,15,15,26,0,0,0,0,0,   0,0,0,0,13,13,1,4},
		{26,12,0,28,11,24,17,15,0,0, 0,0,0,0,13,13,1,5},
		{29,31,29,30,11,19,24,0,0,0, 0,0,0,0,13,13,7,5},
		{26,11,24,0,0,0,0,0,0,0,     0,0,0,0,13,13,2,1},
		{26,28,15,29,15,30,0,0,0,0,  0,0,0,0,13,13,1,6},
		{33,11,32,0,25,16,16,29,15,30,   0,0,0,0,13,13,1,3},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const UBYTE helpmap[4][9] = 
{
	{0,1,2,3,4,5,6,9,7},
	{0,1,2,9,4,5,6,9,7},
	{0,1,8,3,4,5,6,9,7},
	{0,9,2,9,9,5,6,9,7}
};

UBYTE bkg[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

const UBYTE logo[] =
{
 70,72,74, 0, 0, 0, 0, 0,
 71,73,75,76,77,78,79,80
};

const UWORD bgpalette[] = 
{
	RGB_BLACK , RGB_BLACK, RGB_HAWT, RGB_WHITE
};

const UWORD spritepalette[] = 
{
	RGB_WHITE, RGB_LIGHTGRAY, RGB_LIGHTGRAY, RGB_BLACK,
	RGB_HAWT, RGB_HAWT, RGB_HAWT, RGB_HAWT
};

BOOLEAN recallMode;


const UBYTE SCREEN_XO =8;
const UBYTE SCREEN_YO =16 - GLOBAL_POSITION_YOFFSET_BYTE;
const UBYTE SCREEN_XSPACE =32;
const UBYTE SCREEN_YSPACE =8;
const UBYTE SCREEN_PSTARTX =24;
const UBYTE SCREEN_PSTARTY =41 - GLOBAL_POSITION_YOFFSET_BYTE;

BOOLEAN cursorEnable[4] = {1,0,0,0};
UBYTE cursorColumn;
UBYTE cursorRow[4];
UBYTE cursorBigStartX[4];
UBYTE cursorBigStartY[4];
UBYTE cursorStartX[4];
UBYTE cursorStartY[4];
UBYTE cursorColumnLast;
UBYTE cursorRowMain;
BOOLEAN shiftSelect;

BOOLEAN systemIdle = 1;

UBYTE vibratoPosition[4] = {0,0,0,0};
UBYTE vibratoSpeed[4] = {15,15,15,15};
UBYTE vibratoDepth[4] = {0,0,0,0};
BOOLEAN vibratoSlope[4] = {0,0,0,0};


UWORD portPosition[4] = {0,0,0,0};
UWORD portLength[4] = {0xFF,0xFF,0xFF,0xFF};
BOOLEAN portEnabled[4] = {1,0,0,0};

UBYTE dataSetSnap[28];
UBYTE dataSet[28] = 
{
	0x02, // 0   Transpose 0 - 6	
	0x00, // 1   Shape 0 - 3
	0x06, // 2   Envelope 0 - F
	0x00, // 3   Sweep 0 - F
	0x02, // 4   PB Range 0 - 12
	0x00, // 5   Sustain 0 - 1
	0x03, // 6   Output 0 - 3
	
	0x02, // 7   Transpose 0 - 6
	0x00, // 8   Shape 0 - 3
	0x06, // 9   Envelope 0 - F
	0x02, // 10  PB Range 0 - 12
	0x00, // 11  Sustain 0 - 1
	0x03, // 12  Output 0 - 3
	
	0x02, // 13  Transpose 0 - 6
	0x00, // 14  Shape 0 - F
	0x00, // 15  Offset 0 - F
	0x00, // 16  Sweep 0 - F
	0x02, // 17  PB Range 0 - 12
	0x00, // 18  Sustain 0 - 1
	0x03, // 19  Output 0 - 3
	
	0x02, // 20  Transpose 0 - 6
	0x06, // 21  Env 0 - F
	0x00, // 22  Sustain 0 - 1
	0x03, // 23  Output 0 - 3
	
	0x00, // 24  Save 0
	0x00, // 25  Save 1
	0x00, // 26  Save 2
	0x00, // 27  Save 3
};

UBYTE currentScreen;

BOOLEAN parameterLock[24];

const UBYTE synthAddress[4][2] =
{
	{0,7},
	{7,13},
	{13,20},
	{20,24}
};

const UBYTE tableCursorLookup[4][9] =
{
	{ 0,  1, 2,  3,  4, 5, 6,255,24},
	{ 7,  8, 9,255, 10,11,12,255,25},
	{13, 14,15, 16, 17,18,19,255,26},
	{20,255,21,255,255,22,23,255,27}
};

const UBYTE tableData[28][3] =
{
	{3,5 + GLOBAL_POSITION_YOFFSET,6},
	{3,6 + GLOBAL_POSITION_YOFFSET,4},
	{3,7 + GLOBAL_POSITION_YOFFSET,16},
	{3,8 + GLOBAL_POSITION_YOFFSET,255},
	{3,9 + GLOBAL_POSITION_YOFFSET,49},
	{3,10 + GLOBAL_POSITION_YOFFSET,2},
	{3,11 + GLOBAL_POSITION_YOFFSET,4},

	{7,5 + GLOBAL_POSITION_YOFFSET,6},
	{7,6 + GLOBAL_POSITION_YOFFSET,4},
	{7,7 + GLOBAL_POSITION_YOFFSET,16},
	{7,9 + GLOBAL_POSITION_YOFFSET,49},
	{7,10 + GLOBAL_POSITION_YOFFSET,2},
	{7,11 + GLOBAL_POSITION_YOFFSET,4},
	
	{11,5 + GLOBAL_POSITION_YOFFSET,6},
	{11,6 + GLOBAL_POSITION_YOFFSET,16},
	{11,7 + GLOBAL_POSITION_YOFFSET,32},
	{11,8 + GLOBAL_POSITION_YOFFSET,16},
	{11,9 + GLOBAL_POSITION_YOFFSET,49},
	{11,10 + GLOBAL_POSITION_YOFFSET,2},
	{11,11 + GLOBAL_POSITION_YOFFSET,4},

	{15,5 + GLOBAL_POSITION_YOFFSET,6},
	{15,7 + GLOBAL_POSITION_YOFFSET,16},
	{15,10 + GLOBAL_POSITION_YOFFSET,2},
	{15,11 + GLOBAL_POSITION_YOFFSET,4},
	
	{3,13 + GLOBAL_POSITION_YOFFSET,16},
	{7,13 + GLOBAL_POSITION_YOFFSET,16},
	{11,13 + GLOBAL_POSITION_YOFFSET,16},
	{15,13 + GLOBAL_POSITION_YOFFSET,16},
};

const UBYTE octmap[6][2] =
{
	{3,65},
	{2,65},
	{83,83},
	{2,64},
	{3,64},
	{4,64}
};

const UBYTE panmap[4][2] =
{
	{37,37},
	{37,28},
	{22,37},
	{22,28},
};

const UBYTE susmap[2][2] =
{
	{83,0},
	{82,0},
};

const UBYTE markerMapTiles[4][4] = 
{
	{0x03,0x02,0x00,0x5B},
	{0x07,0x02,0x00,0x5B},
	{0x0B,0x02,0x00,0x5B},
	{0x0F,0x02,0x00,0x5B}
};
