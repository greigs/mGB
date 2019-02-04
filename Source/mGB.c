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

void glideTo(){
	if (newNote == 0x00U){
		newNote = addressByte;
	}
	
	// Save the previous note if there was one
	prevNoteTmp = noteStatus[PU1_CURRENT_NOTE] + 0x24U;

	if (prevNoteTmp == 0x00U){
		//printf("pre");
		prevNoteTmp = 0x24U;
	}
	//printf("read%d ",prevNoteTmp);
	prevNote[0] = prevNoteTmp;

	// The new note
	//printf("n%d ",newNote);
	//printf("n%d ",newNote);
	valueByte = 0xFFU;
	//printf("a");
	//printf("%d %d",prevNote[0],newNote);

	//printf("read%d ",prevNoteTmp > newNote);	
    if (prevNoteTmp > newNote){
		//printf("a");
		tmp1 = prevNoteTmp;
		
		tmp1 -= 0x24U;
		delay(0);
		tmp1Freq = freq[tmp1];
		tmp2 = newNote;
		//printf("a%d %d",tmp1,tmp2);
		// delay(5000);
		 tmp2 -= 0x24U;
		 //printf("ay%d ",tmp2);
		 tmp2Freq = freq[tmp2];
		 //newStepSize = 0x01U;
    	 newStepSize = tmp1Freq;
		 newStepSize -= tmp2Freq;
		 newStepSize /= 0x8FU;
        //portStepSize[0] -= tmp2Freq;
		//portStepSize[0] /= 0xF;
	}
	else if (prevNoteTmp < newNote){
		//printf("b");
		tmp1 = prevNoteTmp;
		//printf("bt%d ",tmp1);
		tmp1 -= 0x24U;
		tmp1Freq = freq[tmp1];
		tmp2 = newNote;
		// printf("b%d %d",tmp1,tmp2);
		// delay(5000);
		tmp2 -= 0x24U;
		//printf("by%d ",tmp2);
		tmp2Freq = freq[tmp2];
		//newStepSize = 0x01U;
    	newStepSize = tmp2Freq;
        newStepSize -= tmp1Freq;
		newStepSize /= 0x8FU;
        //portStepSize[0] -= tmp1Freq;
		//portStepSize[0] /= 2;
	}
	if (newStepSize == 0x00U) {
		newStepSize = 0x01U;
		//printf("rs");
	}
	//printf("c");
	portDelay[0] = 0;
	portStepSize[0] = newStepSize;
	noteStatus[PU1_CURRENT_NOTE] = newNote;	
	addressByte = newNote;
	asmPlayNotePu1();
	
	cursorEnable[0] = 1;	
}

void testSynths()
{	
	
	newNote += 0x08U;
	asmGlideToC();
	
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

