/*********************************************************************/
/*********************************************************************/
//	ArduEye_SMH_v1.h
//	ArduEye Library for the Stonyman/Hawksbill Centeye Vision Chips
//	
//	Basic functions to operate Stonyman/Hawksbill with ArduEye boards
//	Supports all Arduino boards that use the ATMega 8/168/328/2560
//	NOTE: ATMega 2560 SPI for external ADC is not supported.
//
//	Working revision started July 9, 2012
//
/*********************************************************************/
/*********************************************************************/

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

#ifndef _ARDUEYE_SMH_H_INCLUDED
#define _ARDUEYE_SMH_H_INCLUDED

//supports older version of ARDUINO IDE
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

/*********************************************************************/
//loads pin definitions based on the board type
//set in the Arduino IDE
//currently supports ATmega2560/8/168/328

#if defined(__AVR_ATmega2560__)
   	#  include "pin_defs_2560.h"	//2560 defs
#elif defined (__AVR_ATmega8__)||(__AVR_ATmega168__)|(__AVR_ATmega168P__)||(__AVR_ATmega328P__)
	#  include "pin_defs_168_328.h"	//8/168/328 defs
#else 
	#  error "Code only supports ATmega 2560 and ATmega 8/168/328"
#endif

/*********************************************************************/
//copy pin defs from loaded pin_def headers

#define RESP_PORT  SMH_RESP_PORT
#define RESP_DDR	 SMH_RESP_DDR
#define RESP	 SMH_RESP_PIN

#define INCP_PORT	 SMH_INCP_PORT
#define INCP_DDR   SMH_INCP_DDR
#define INCP	 SMH_INCP_PIN
	
#define RESV_PORT	 SMH_RESV_PORT
#define RESV_DDR	 SMH_RESV_DDR
#define RESV	 SMH_RESV_PIN

#define INCV_PORT	 SMH_INCV_PORT
#define INCV_DDR	 SMH_INCV_DDR
#define INCV	 SMH_INCV_PIN

#define INPHI_PORT SMH_INPHI_PORT
#define INPHI_DDR  SMH_INPHI_DDR
#define INPHI	 SMH_INPHI_PIN

#define ADC_SS_PORT SMH_ADC_SS_PORT
#define ADC_SS_DDR  SMH_ADC_SS_DDR
#define ADC_SS      SMH_ADC_SS_PIN

#define ANALOG_PORT SMH_ANALOG_PORT
#define ANALOG_DDR  SMH_ANALOG_DDR
#define ANALOG0	  SMH_ANALOG0
#define ANALOG1	  SMH_ANALOG1
#define ANALOG2	  SMH_ANALOG2
#define ANALOG3	  SMH_ANALOG3

/*********************************************************************/
//MACROS to pulse chip lines

#define SMH1_ResP_Pulse {RESP_PORT|=RESP;RESP_PORT&=~RESP;}
	
#define SMH1_IncP_Pulse {INCP_PORT|=INCP;INCP_PORT&=~INCP;}

#define SMH1_ResV_Pulse {RESV_PORT|=RESV;RESV_PORT&=~RESV;}
	
#define SMH1_IncV_Pulse {INCV_PORT|=INCV;INCV_PORT&=~INCV;}

/*********************************************************************/
//MACROS for inphi (delay is inserted between high and low)

#define SMH1_InPhi_SetHigh {INPHI_PORT|=INPHI;}
	
#define SMH1_InPhi_SetLow {INPHI_PORT&=~INPHI;}	

/*********************************************************************/
//MACRO to set all pins low

#define SMH1_SetAllLow {RESP_PORT&=~RESP;INCP_PORT&=~INCP;RESV_PORT&=~RESV;INCV_PORT&=~INCV;INPHI_PORT&=~INPHI;} 

/*********************************************************************/
//SMH System Registers

#define SMH_SYS_COLSEL 0	//select column
#define SMH_SYS_ROWSEL 1	//select row
#define SMH_SYS_VSW 2		//vertical switching
#define SMH_SYS_HSW 3		//horizontal switching
#define SMH_SYS_CONFIG 5	//configuration register
#define SMH_SYS_VREF 4		//voltage reference
#define SMH_SYS_NBIAS 6		//nbias
#define SMH_SYS_AOBIAS 7	//analog out bias

/*********************************************************************/
//default values

// Supply voltage types
// Notation: AVB is A.B volts. e.g. 5V0 is 5V, 3V3 is 3.3V, etc.
#define SMH1_VDD_5V0  1

#define SMH_VREF_5V0 30		//vref for 5 volts
#define SMH_NBIAS_5V0 55	//nbias for 5 volts
#define SMH_AOBIAS_5V0 55	//aobias for 5 volts

#define SMH_GAIN_DEFAULT 0	//no amp gain
#define SMH_SELAMP_DEFAULT 0	//amp bypassed

/*********************************************************************/
// ADC types

// ARDUINO ONBOARD ADC
#define SMH1_ADCTYPE_ONBOARD 0
// MCP3201, Microchip, 12bits, 100ksps
#define SMH1_ADCTYPE_MCP3201 1
// MCP3201, Microchip, 12bits, 100ksps, for ArduEye Bug v1.0
#define SMH1_ADCTYPE_MCP3201_2 2
// MCP3001, Microchip, 10bits, 200ksps
#define SMH1_ADCTYPE_MCP3001 3

/*********************************************************************/


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
//	ArduEyeSMHClass
/*********************************************************************/
/*********************************************************************/

class ArduEyeSMHClass 
{

private:

  //indicates whether amplifier is in use	
  char useAmp;

public:

/*********************************************************************/
// Initialize the vision chip for image readout
  
  void begin(short vref=SMH_VREF_5V0,short nbias=SMH_NBIAS_5V0,short aobias=SMH_AOBIAS_5V0,char gain=SMH_GAIN_DEFAULT,char selamp=SMH_SELAMP_DEFAULT); 

/*********************************************************************/
// Chip Register and Value Manipulation

  //set pointer on chip
  void setPointer(char ptr);

  //set value of current pointer
  void setValue(short val);

  //increment value of current pointer
  void incValue(short val);

  //pulse INPHI to operate amplifier
  void pulseInphi(char delay); 

  //clear all register values
  void clearValues(void);

  //set pointer to register and set value for that register
  void setPointerValue(char ptr,short val);

  //set configuration register on chip
  void setConfig(char gain, char selamp,char cvdda=1);

  //select amp and set amp gain
  void setAmpGain(char gain);

  //set analog input to Arduino for onboard ADC
  void setAnalogInput(char analogInput);

  //set external ADC input
  void setADCInput(char ADCInput,char state);

  //set hsw and vsw registers to bin on-chip
  void setBinning(short hbin,short vbin);

/*********************************************************************/
// Bias functions

  //set individual bias values
  void setVREF(short vref);
  void setNBIAS(short nbias);
  void setAOBIAS(short aobias);
 
  //set biases based on Vdd
  void setBiasesVdd(char vddType);

  //set all bias values
  void setBiases(short vref,short nbias,short aobias);


/*********************************************************************/
// Image Functions

  //given an image, returns a fixed-pattern noise mask and mask_base
  void calcMask(short *img, short size, unsigned char *mask, short *mask_base);

  //applies pre-calculated FPN mask to an image
  void applyMask(short *img, short size, unsigned char *mask, short mask_base);

  //gets an image from the vision chip
  void getImage(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned 	char numcols, unsigned char colskip, char ADCType,char anain);

  //gets a image from the vision chip, sums each row and returns one pixel for the row
  void getImageRowSum(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned 	char numcols, unsigned char colskip, char ADCType,char anain);
 
  //gets a image from the vision chip, sums each col and returns one pixel for the col
  void getImageColSum(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned 	char numcols, unsigned char colskip, char ADCType,char anain);

  //takes an image and returns the maximum value row and col
  void findMax(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char ADCType,char anain,unsigned char *max_row, unsigned char *max_col);

  //prints the entire vision chip over serial as a Matlab array
  void chipToMatlab(char whichchip,char ADCType,char anain);

  //prints a section of the vision chip over serial as a Matlab array
  void sectionToMatlab(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char ADCType, unsigned char 	anain);   

};

//external definition of ArduEyeSMH class instance
extern ArduEyeSMHClass ArduEyeSMH;

#endif
