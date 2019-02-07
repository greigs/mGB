#include "gb/gb.h"
#include "gb/cgb.h"
#include "mGB.h"
//#include "rand.h"
//#include "gb/font.h"
//#include <stdio.h>
//#include <gb/drawing.h>
//#include <gb/font.h>
//#include <gb/console.h>
//#include <stdarg.h>
//#include "printf.c"

void printbyte(UBYTE v1, UBYTE v2, UBYTE v3)
{
	bkg[0] = (v1 >> 4)+1;
	bkg[1] = (0x0F & v1)+1;

	bkg[2] = 0;

	bkg[3] = (v2 >> 4)+1;
	bkg[4] = (0x0F & v2)+1;

	bkg[5] = 0;

	bkg[6] = (v3 >> 4)+1;
	bkg[7] = (0x0F & v3)+1;

	bkg[8] = 0;
	set_bkg_tiles(1,16,10,1,bkg);
}

#include "mGBSynthPitchFunctions.c"
#include "mGBSynthCommonFunctions.c"
#include "mGBDisplayFunctions.c"
#include "mGBMemoryFunctions.c"
#include "mGBUserFunctions.c"

void setSoundDefaults()
{
  NR52_REG = 0x8FU;  //Turn sound on
  NR50_REG = 0x77U;  //Turn on Pulses outs

  setOutputPan(0U, 64U);
  setOutputPan(1U, 64U);
  setOutputPan(2U, 64U);
  setOutputPan(3U, 64U);

  asmLoadWav(wavDataOffset); //tRIANGLE
  NR32_REG = 0x00U;

  NR44_REG = 0x80U;
  NR41_REG = 0x3FU; //sound length
  
}


void noOp(){
 // intentionally does nothing
}

void glideTo(){
	if (valueByte != 0x00U)
	{
		if (newNote == 0x00U){
			newNote = addressByte;
		}
		
		// Save the previous note if there was one
		prevNoteTmp = noteStatus[PU1_CURRENT_NOTE] + 0x24U;

		if (prevNoteTmp == 0x00U){
			prevNoteTmp = 0x24U;
		}
		prevNote[PU1] = prevNoteTmp;

		if (prevNoteTmp > newNote){
			// Why is this needed? It does nothing?
			// Without a call to *something* within this block, the compiler tries to
			// optimise the stack pointer, but causes a giant mess instead (it crashes).
			// This could probably be fixed by messing with the stack some more,
			// but this is an easier hack for now.
			// Yeah, GBDK kinda sucks. At least I'm blaming *it* for the time being.
			noOp();

			tmp1 = prevNoteTmp;
			tmp1 -= 0x24U;
			tmp1Freq = freq[tmp1];
			tmp2 = newNote;
			tmp2 -= 0x24U;
			tmp2Freq = freq[tmp2];
			newStepSize = tmp1Freq;
			newStepSize -= tmp2Freq;
			newStepSize /= 0x6FU;
		}
		if (prevNoteTmp < newNote){
			tmp1 = prevNoteTmp;
			tmp1 -= 0x24U;
			tmp1Freq = freq[tmp1];
			tmp2 = newNote;
			tmp2 -= 0x24U;
			tmp2Freq = freq[tmp2];
			newStepSize = tmp2Freq;
			newStepSize -= tmp1Freq;
			newStepSize /= 0x6FU;
		}
		if (newStepSize == 0x00U) {
			newStepSize = 0x01U;
		}

		portDelay[PU1] = 0;
		portStepSize[PU1] = newStepSize;
		noteStatus[PU1_CURRENT_NOTE] = newNote;	
		addressByte = newNote;
	}
	asmPlayNotePu1();
	
	// same reason as above
	noOp();
	//cursorEnable[0] = 1;	
}

void testSynths()
{	
	newNote += 0x08U;
	glideTo();
	cursorEnable[0] = 1;	
}

void main()
{
	//font_init();
	addressByte = 0x25U;
	newNote = addressByte;
    disable_interrupts();
	  cpu_fast();
		checkMemory();
		displaySetup();
		setSoundDefaults();
  	add_TIM(updateSynths);

		loadDataSet(0x00U);
		loadDataSet(0x01U);
		loadDataSet(0x02U);
		loadDataSet(0x03U);
    enable_interrupts();


  /* Set TMA to divide clock by 0x100 */
  		TMA_REG = 0x00U;
  /* Set clock to 262144 Hertz */
  		TAC_REG = 0x05U;
  /* Handle VBL and TIM interrupts */

  set_interrupts(VBL_IFLAG | TIM_IFLAG | SIO_IFLAG);

	SHOW_BKG;
	SHOW_SPRITES;

	showSplashScreen();
	//delay(500);

	showMainScreen();
    printversion();
	asmMain();
	//testSynths();
}

