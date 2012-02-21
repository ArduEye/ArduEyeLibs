/*
===============================================================================
Copyright (c) 2012 Centeye, Inc. 
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
    
    Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY CENTEYE, INC. ``AS IS'' AND ANY EXPRESS OR 
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
EVENT SHALL CENTEYE, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are 
those of the authors and should not be interpreted as representing official 
policies, either expressed or implied, of Centeye, Inc.
===============================================================================
*/
/* 
Change log:
January 22, 2012: initial release
February 17, 2012: includes Arduino.h or WProgram.h depending on Arduino IDE version
*/
#ifndef SMH1_StonymanHawksbill_v1_h
#define SMH1_StonymanHawksbill_v1_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif
#include <SPI.h>

// Basic digital interface defines- These are the Arduino "digital" pins
// associated with the five digital inputs to a Stonyman/Hawksbill chip
#define SMH1_RESP 8
#define SMH1_INCP 7
#define SMH1_RESV 6
#define SMH1_INCV 5
#define SMH1_INPHI 4

// Basic analog interface defines- These are the four Arduino "analog" pins
// associated with the four possible slow plugins on the ArduEye Uno Rocket board
#define SMH1_ANALOG0 0
#define SMH1_ANALOG1 1
#define SMH1_ANALOG2 2
#define SMH1_ANALOG3 3

// These four macros pulse the RESP,INCP,RESV,INCV inputs to the ArduEye. This
// works faster than using the standard Arduino digitalWrite() function.
#define SMH1_ResP_Pulse {PORTB|=0x01;PORTB&=0xFE;}
	
#define SMH1_IncP_Pulse {PORTD|=0x80;PORTD&=0x7F;}

#define SMH1_ResV_Pulse {PORTD|=0x40;PORTD&=0xBF;}
	
#define SMH1_IncV_Pulse {PORTD|=0x20;PORTD&=0xDF;}	

// These two macros set InPhi high and low respectively. Generally a short delay
// is inserted in between the calling of these two functions.
#define SMH1_InPhi_SetHigh {PORTD|=0x10;}
	
#define SMH1_InPhi_SetLow {PORTD&=0xEF;}	

// Finally a macro to set all five digital signals to low
#define SMH1_SetAllLow {PORTB&=0xFE;PORTD&=0x7F;PORTD&=0xBF;PORTD&=0xDF;PORTD&=0xEF;}  
	
// Stonyman / Hawksbill system register values. Set the pointer register to the
// appropriate value below to access the corresponding system register.
#define SMH_SYS_COLSEL 0
#define SMH_SYS_ROWSEL 1
#define SMH_SYS_VSW 2
#define SMH_SYS_HSW 3
#define SMH_SYS_CONFIG 5
#define SMH_SYS_VREF 4
#define SMH_SYS_NBIAS 6
#define SMH_SYS_AOBIAS 7

// ADC types
// MCP3201, Microchip, 12bits, 100ksps
#define SMH1_ADCTYPE_MCP3201 1
// MCP3001, Microchip, 10bits, 200ksps
#define SMH1_ADCTYPE_MCP3001 2

// Supply voltage types
// Notation: AVB is A.B volts. e.g. 5V0 is 5V, 3V3 is 3.3V, etc.
#define SMH1_VDD_5V0  1

//=============================================================
// FUNCTION PROTOTYPES
//=============================================================
void SMH1_ConfigureBiasesSetRaw(char vddtype);
void SMH1_ConfigureAnalogInput(char anain);
void SMH1_SelectChipSPIADC(char anain, char cs);
void SMH1_SetupIO(void);
void SMH1_SetPtrSlow(char ptr);
void SMH1_SetPtrFast(char ptr);
void SMH1_SetValSlow(short val);
void SMH1_SetValFast(short val);
void SMH1_ResvSlow(void);
void SMH1_IncvSlow(short val);
void SMH1_IncvFast(short val);
void SMH1_SetPtrValSlow(char ptr, short val);
void SMH1_SetPtrValFast(char ptr, short val);
void SMH1_ClearAllValsSlow(void);
void SMH1_ConfigSlow(char gain, char selamp, char cvdda);
void SMH1_ConfigFast(char gain, char selamp, char cvdda);
void SMH1_PulseInphiSlow(char delay);
void SMH1_PulseInphiFast(char delay);
void SMH1_SetBasicBinning(unsigned char hbin, unsigned char vbin);
void SMH1_MatlabDumpEntireChip(char anain, char useamp, char whichchip);
void SMH1_MatlabDumpChipSection(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, unsigned char anain, char useamp);
void SMH1_MatlabDumpChipSectionSPIADC(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, unsigned char anain, unsigned char useamp, char ADCType);
void SMH1_FindMaxSlow(unsigned char rowstart, unsigned char numrows, unsigned char colstart, unsigned char numcols, char anain, unsigned char *rowwinner, unsigned char *colwinner);
void SMH1_FindMaxSPIADC(unsigned char rowstart, unsigned char numrows, unsigned char colstart, unsigned char numcols, char anain, char ADCType, unsigned char *rowwinner, unsigned char *colwinner);
void SMH1_GetImageSlow(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char anain, char useamp);
void SMH1_GetImageFast(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char anain, char useamp);
void SMH1_GetImageSPIADC(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char anain, char useamp, char ADCType);

#endif