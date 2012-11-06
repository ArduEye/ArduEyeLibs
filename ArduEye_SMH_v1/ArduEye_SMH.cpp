/*********************************************************************/
/*********************************************************************/
//	ArduEye_SMH_v1.c
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

#include "ArduEye_SMH.h"
#include <SPI.h>	//SPI required for external ADC

//class instance to be referenced in sketch
ArduEyeSMHClass ArduEyeSMH;

/*********************************************************************/
//	begin
//	Initializes the vision chips for normal operation.  Sets vision
//	chip pins to low outputs, clears chip registers, sets biases and
//	config register.  If no parameters are passed in, default values
//	are used.
/*********************************************************************/

void ArduEyeSMHClass::begin(short vref,short nbias,short aobias,char gain,char selamp)
{
  //set all digital pins to output
  RESP_DDR|=RESP;
  INCP_DDR|=INCP;
  RESV_DDR|=RESV;
  INCV_DDR|=INCV;
  INPHI_DDR|=INPHI;

  //set external ADC SS to high
  ADC_SS_DDR|=ADC_SS;
  ADC_SS_PORT|=ADC_SS;
 
  //set all pins low
  SMH1_SetAllLow;

  //clear all chip register values
  clearValues();

  //set up biases
  setBiases(vref,nbias,aobias);

  short config=gain+(selamp*8)+(16);
  
  //turn chip on with config value
  setPointerValue(SMH_SYS_CONFIG,config);
  
  //if amp is used, set useAmp variable
  if(selamp==1)
   useAmp=1;
  else
   useAmp=0;

}

/*********************************************************************/
//	setPointer
//	Sets the pointer system register to the desired value
/*********************************************************************/

void ArduEyeSMHClass::setPointer(char ptr)
{
  // clear pointer
  SMH1_ResP_Pulse; // macro

  // increment pointer to desired value
  for (short i=0; i!=ptr; ++i) 
    SMH1_IncP_Pulse; // macro
}

/*********************************************************************/
//	setValue
//	Sets the value of the current register
/*********************************************************************/

void ArduEyeSMHClass::setValue(short val) 
{
  // clear pointer
  SMH1_ResV_Pulse; // macro

  // increment pointer
  for (short i=0; i!=val; ++i) 
    SMH1_IncV_Pulse; // macro
}

/*********************************************************************/
//	incValue
//	Sets the pointer system register to the desired value.  Value is
//	not reset so the current value must be taken into account
/*********************************************************************/

void ArduEyeSMHClass::incValue(short val) 
{
  for (short i=0; i<val; ++i) //increment pointer
    SMH1_IncV_Pulse;
}

/*********************************************************************/
//	pulseInphi
//	Operates the amplifier.  Sets inphi pin high, delays to allow
//	value time to settle, and then brings it low.
/*********************************************************************/

void ArduEyeSMHClass::pulseInphi(char delay) 
{
  SMH1_InPhi_SetHigh;
  delayMicroseconds(delay);
  SMH1_InPhi_SetLow;
}

/*********************************************************************/
//	setPointerValue
//	Sets the pointer to a register and sets the value of that        
//	register
/*********************************************************************/

void ArduEyeSMHClass::setPointerValue(char ptr,short val)
{
	setPointer(ptr);	//set pointer to register
      setValue(val);	//set value of that register
}

/*********************************************************************/
//	clearValues
//	Resets the value of all registers to zero
/*********************************************************************/

void ArduEyeSMHClass::clearValues(void)
{
    for (char i=0; i!=8; ++i)
    	setPointerValue(i,0);	//set each register to zero
}

/*********************************************************************/
//	setVREF
//	Sets the VREF register value (0-63)
/*********************************************************************/

void  ArduEyeSMHClass::setVREF(short vref)
{
  setPointerValue(SMH_SYS_VREF,vref);
}

/*********************************************************************/
//	setNBIAS
//	Sets the NBIAS register value (0-63)
/*********************************************************************/

void  ArduEyeSMHClass::setNBIAS(short nbias)
{
  setPointerValue(SMH_SYS_NBIAS,nbias);
}

/*********************************************************************/
//	setAOBIAS
//	Sets the AOBIAS register value (0-63)
/*********************************************************************/

void  ArduEyeSMHClass::setAOBIAS(short aobias)
{
  setPointerValue(SMH_SYS_AOBIAS,aobias);
}

/*********************************************************************/
//	setBiasesVdd
//	Sets biases based on chip voltage
/*********************************************************************/

void ArduEyeSMHClass::setBiasesVdd(char vddType)
{
  
  // determine biases. Only one option for now.
  switch (vddType) 
  {
    case SMH1_VDD_5V0:	//chip receives 5V
    default:
      setPointerValue(SMH_SYS_NBIAS,SMH_NBIAS_5V0);
	setPointerValue(SMH_SYS_AOBIAS,SMH_AOBIAS_5V0);
	setPointerValue(SMH_SYS_VREF,SMH_VREF_5V0);
    break;
  }
}

/*********************************************************************/
//	setBiases
//	Sets all three biases
/*********************************************************************/

void ArduEyeSMHClass::setBiases(short vref,short nbias,short aobias)
{
   	setPointerValue(SMH_SYS_NBIAS,nbias);
	setPointerValue(SMH_SYS_AOBIAS,aobias);
	setPointerValue(SMH_SYS_VREF,vref);
}

/*********************************************************************/
//	setConfig
//	Sets configuration register
//	cvdda:  (1) connect vdda, always should be connected
//	selamp: (0) bypasses amplifier, (1) connects it
//	gain: amplifier gain 1-7
//	EXAMPLE 1: To configure the chip to bypass the amplifier:
//	setConfig(0,0,1);
//	EXAMPLE 2: To use the amplifier and set the gain to 4:
//	setConfig(4,1,1);
/*********************************************************************/

void ArduEyeSMHClass::setConfig(char gain, char selamp, char cvdda) 
{
   short config=gain+(selamp*8)+(cvdda*16);	//form register value

   if(selamp==1)	//if selamp is 1, set useAmp variable to 1
     useAmp=1;
   else 
     useAmp=0;
  
   // Note that config will have the following form binary form:
   // 000csggg where c=cvdda, s=selamp, ggg=gain (3 bits)
   // Note that there is no overflow detection in the input values.
   setPointerValue(SMH_SYS_CONFIG,config);
}

/*********************************************************************/
//	setAmpGain
//	A friendlier version of setConfig.  If amplifier gain is set to 
//	zero, amplifier is bypassed.  Otherwise the appropriate amplifier
//	gain (range 1-7) is set.
/*********************************************************************/

void ArduEyeSMHClass::setAmpGain(char gain)
{
   short config;

   if((gain>0)&&(gain<8))	//if gain is a proper value, connect amp
   {
    config=gain+8+16;	//gain+(selamp=1)+(cvdda=1)
    useAmp=1;	//using amplifier
   }
   else	//if gain is zero or outside range, bypass amp
   {
    config=16;	//(cvdda=1)
    useAmp=0;	//bypassing amplifier
   }

   setPointerValue(SMH_SYS_CONFIG,config);	//set config register
}

/*********************************************************************/
//	setAnalogInput
//	Sets the analog pin for one vision chip to be an input.
//	This is for the Arduino onboard ADC, not an external ADC
/*********************************************************************/

void ArduEyeSMHClass::setAnalogInput(char analogInput)
{
 switch (analogInput)
 {
    case 0:
      ANALOG_DDR&=~ANALOG0;	//set chip 0 as analog input
      break;
    case 1:
      ANALOG_DDR&=~ANALOG1;
      break;
    case 2:
      ANALOG_DDR&=~ANALOG2;
      break;
    case 3:
      ANALOG_DDR&=~ANALOG3;
      break;
  }
}

/*********************************************************************/
//	setADCInput
//	Sets the analog pin to be a digital output and select a chip
//	to connect to the external ADC.  The state can be used to
//	deselect a particular chip as well.
/*********************************************************************/

void ArduEyeSMHClass::setADCInput(char ADCInput,char state)
{
  
  // make sure A# pin is set to be a digital output
  switch (ADCInput){
    case 0:
      ANALOG_DDR |=ANALOG0; // set A0 to output
      if (state)
        ANALOG_PORT |= ANALOG0; // turn on
      else
        ANALOG_PORT &= ~ANALOG0; // turn off
      break;
    case 1:
      ANALOG_DDR |= ANALOG1; // set A1 to output
      if (state)
        ANALOG_PORT |= ANALOG1; // turn on
      else
        ANALOG_PORT &= ~ANALOG1; // turn off
      break;
    case 2:
      ANALOG_DDR  |= ANALOG2; // set A2 to output
      if (state)
        ANALOG_PORT |= ANALOG2; // turn on
      else
        ANALOG_PORT &= ~ANALOG2; // turn off
      break;
    case 3:
      ANALOG_DDR |= ANALOG3; // set A3 to output
      if (state)
        ANALOG_PORT |= ANALOG3; // turn on
      else
        ANALOG_PORT &= ~ANALOG3; // turn off
      break;
  }
}

/*********************************************************************/
//	setBinning
//	Configures binning in the focal plane using the VSW and HSW
//	system registers. The super pixels are aligned with the top left 
//	of the image, e.g. "offset downsampling" is not used. This 
//	function is for the Stonyman chip only. 
//	VARIABLES:
//	hbin: set to 1, 2, 4, or 8 to bin horizontally by that amount
//	vbin: set to 1, 2, 4, or 8 to bin vertically by that amount
/*********************************************************************/

void ArduEyeSMHClass::setBinning(short hbin,short vbin)
{
   short hsw,vsw;

   switch (hbin) //horizontal binning
   {
    case 2:		//downsample by 2
      hsw = 0xAA;
      break;
    case 4:		//downsample by 4
      hsw = 0xEE;
      break;
    case 8:		//downsample by 8
      hsw = 0xFE;
      break;
    default:	//no binning
      hsw = 0x00;
   }

   switch (vbin) 	//vertical binning
   {
    case 2:		//downsample by 2
      vsw = 0xAA;
      break;
    case 4:		//downsample by 4
      vsw = 0xEE;
      break;
    case 8:		//downsample by 8
      vsw = 0xFE;
      break;
    default:	//no binning
      vsw = 0x00;
    }

  //set switching registers
  setPointerValue(SMH_SYS_HSW,hsw);
  setPointerValue(SMH_SYS_VSW,vsw);
}

/*********************************************************************/
//	calcMask
//	Expose the vision chip to uniform texture (such as a white piece
//	of paper placed over the optics).  Take an image using the 
//	getImage function.  Pass the short "img" array and the "size"
//	number of pixels, along with a unsigned char "mask" array to hold
//	the FPN mask and mask_base for the FPN mask base.  Function will
//	populate the mask array and mask_base variable with the FPN mask,
//	which can then be used with the applMask function. 
/*********************************************************************/

void ArduEyeSMHClass::calcMask(short *img, short size, unsigned char *mask,short *mask_base)
{
 	*mask_base = 10000; // e.g. "high"

      for (int i=0; i<size; ++i)
        if (img[i]<(*mask_base))	//find the min value for mask_base
          *mask_base = img[i];

      // generate calibration mask
      for (int i=0; i<size; ++i)
        mask[i] = img[i] - *mask_base;	//subtract min value for mask
}

/*********************************************************************/
//	applyMask
//	given the "mask" and "mask_base" variables calculated in        
//	calcMask, and a current image, this function will subtract the
//	mask to provide a calibrated image.
/*********************************************************************/

void ArduEyeSMHClass::applyMask(short *img, short size, unsigned char *mask, short mask_base)
{
	 // Subtract calibration mask
  	 for (int i=0; i<size;++i) 
	{
    		img[i] -= mask_base+mask[i];  //subtract FPN mask
    		img[i]=-img[i];          //negate image so it displays properly
 	}
}

/*********************************************************************/
//	getImage
//	This function acquires a box section of a Stonyman or Hawksbill 
//	and saves to image array img.  Note that images are read out in 
//	raster manner (e.g. row wise) and stored as such in a 1D array. 
//	In this case the pointer img points to the output array. 
//
//	VARIABLES: 
//	img (output): pointer to image array, an array of signed shorts
//	rowstart: first row to acquire
//	numrows: number of rows to acquire
//	rowskip: skipping between rows (useful if binning is used)
//	colstart: first column to acquire
//	numcols: number of columns to acquire
//	colskip: skipping between columns
//	ADCType: which ADC to use, defined ADC_TYPES
//	anain (0,1,2,3): which analog input to use
//	
//	EXAMPLES:
//	getImage(img,16,8,1,24,8,1,SMH1_ADCTYPE_ONBOARD,0): 
//	Grab an 8x8 window of pixels at raw resolution starting at row 
//	16, column 24, from chip using onboard ADC at input 0
//	getImage(img,0,14,8,0,14,8,SMH1_ADCTYPE_MCP3201,2): 
//	Grab entire Stonyman chip when using
//	8x8 binning. Grab from input 2.
/*********************************************************************/

void ArduEyeSMHClass::getImage(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char ADCType,char anain) 
{
  short *pimg = img; // pointer to output image array
  short val;
  unsigned char chigh,clow;
  unsigned char row,col;
  
  if(ADCType==SMH1_ADCTYPE_ONBOARD)	//if using onboard ADC
     setAnalogInput(anain);		//set analog input to Arduino
  else if(ADCType==SMH1_ADCTYPE_MCP3201_2)
  { 
     setAnalogInput(anain);
     ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }
  else	//if using external ADC
  {
    setADCInput(anain,1); // enable chip
    ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }

  // Go to first row
  setPointerValue(SMH_SYS_ROWSEL,rowstart);
 
  // Loop through all rows
  for (row=0; row<numrows; ++row) {
    
    // Go to first column
    setPointerValue(SMH_SYS_COLSEL,colstart);
    
    // Loop through all columns
    for (col=0; col<numcols; ++col) {
      
      // settling delay
      delayMicroseconds(1);

      // pulse amplifier if needed
	if (useAmp) 
        pulseInphi(2);
      
      // get data value
      delayMicroseconds(1);
      
      // get pixel value from ADC
      switch (ADCType) 
      {
        case SMH1_ADCTYPE_ONBOARD:	//onboard Arduino ADC
           val = analogRead(anain); // acquire pixel
	    break;
        case SMH1_ADCTYPE_MCP3001:  // Micrchip 10 bit
           ADC_SS_PORT &= ~ADC_SS;  // turn SS low to start conversion
           chigh=SPI.transfer(0);   // get high byte
           clow=SPI.transfer(0);    // get low byte
           val = ((short)(chigh&0x1F))<<5;
           val += (clow&0xF8)>>3;
           ADC_SS_PORT |= ADC_SS;   // SS high to stop
          break;
        case SMH1_ADCTYPE_MCP3201:  // Microchip 12 bit
        case SMH1_ADCTYPE_MCP3201_2:
	     ADC_SS_PORT &= ~ADC_SS;  // turn SS low to start conversion
           chigh=SPI.transfer(0);   // get high byte
           clow=SPI.transfer(0);    // get low byte
           val = ((short)(chigh&0x1F))<<7;
           val += (clow&0xFE)>>1;
	     ADC_SS_PORT |= ADC_SS;   // SS high to stop
          break;
        default:
           val = 555;
          break;
      }
      
      *pimg = val; // store pixel
      pimg++; // advance pointer
      incValue(colskip); // go to next column
    }
    setPointer(SMH_SYS_ROWSEL);
    incValue(rowskip); // go to next row
  }

  if((ADCType!=SMH1_ADCTYPE_ONBOARD)&&(ADCType!=SMH1_ADCTYPE_MCP3201_2))
   setADCInput(anain,0); // disable chip

}

/*********************************************************************/
//	getImageRowSum
//	This function acquires a box section of a Stonyman or Hawksbill 
//	and saves to image array img.  However, each row of the image
//	is summed and returned as a single value.
//	Note that images are read out in 
//	raster manner (e.g. row wise) and stored as such in a 1D array. 
//	In this case the pointer img points to the output array. 
//
//	VARIABLES: 
//	img (output): pointer to image array, an array of signed shorts
//	rowstart: first row to acquire
//	numrows: number of rows to acquire
//	rowskip: skipping between rows (useful if binning is used)
//	colstart: first column to acquire
//	numcols: number of columns to acquire
//	colskip: skipping between columns
//	ADCType: which ADC to use, defined ADC_TYPES
//	anain (0,1,2,3): which analog input to use
//	
//	EXAMPLES:
//	getImage(img,16,8,1,24,8,1,SMH1_ADCTYPE_ONBOARD,0): 
//	Grab an 8x8 window of pixels at raw resolution starting at row 
//	16, column 24, from chip using onboard ADC at input 0
//	getImage(img,0,14,8,0,14,8,SMH1_ADCTYPE_MCP3201,2): 
//	Grab entire Stonyman chip when using
//	8x8 binning. Grab from input 2.
/*********************************************************************/

void ArduEyeSMHClass::getImageRowSum(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char ADCType,char anain) 
{
  short *pimg = img; // pointer to output image array
  short val,total=0;
  unsigned char chigh,clow;
  unsigned char row,col;
  
  if(ADCType==SMH1_ADCTYPE_ONBOARD)	//if using onboard ADC
     setAnalogInput(anain);		//set analog input to Arduino
  else if(ADCType==SMH1_ADCTYPE_MCP3201_2)
  { 
     setAnalogInput(anain);
     ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }
  else	//if using external ADC
  {
    setADCInput(anain,1); // enable chip
    ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }

  // Go to first row
  setPointerValue(SMH_SYS_ROWSEL,rowstart);
 
  // Loop through all rows
  for (row=0; row<numrows; ++row) {
    
    // Go to first column
    setPointerValue(SMH_SYS_COLSEL,colstart);
  
    total=0;
    
    // Loop through all columns
    for (col=0; col<numcols; ++col) {
      
      // settling delay
      delayMicroseconds(1);

      // pulse amplifier if needed
	if (useAmp) 
        pulseInphi(2);
      
      // get data value
      delayMicroseconds(1);
      
      // get pixel value from ADC
      switch (ADCType) 
      {
        case SMH1_ADCTYPE_ONBOARD:	//onboard Arduino ADC
           val = analogRead(anain); // acquire pixel
	    break;
        case SMH1_ADCTYPE_MCP3001:  // Micrchip 10 bit
           ADC_SS_PORT &= ~ADC_SS;  // turn SS low to start conversion
           chigh=SPI.transfer(0);   // get high byte
           clow=SPI.transfer(0);    // get low byte
           val = ((short)(chigh&0x1F))<<5;
           val += (clow&0xF8)>>3;
           ADC_SS_PORT |= ADC_SS;   // SS high to stop
          break;
        case SMH1_ADCTYPE_MCP3201:  // Microchip 12 bit
        case SMH1_ADCTYPE_MCP3201_2:
	     ADC_SS_PORT &= ~ADC_SS;  // turn SS low to start conversion
           chigh=SPI.transfer(0);   // get high byte
           clow=SPI.transfer(0);    // get low byte
           val = ((short)(chigh&0x1F))<<7;
           val += (clow&0xFE)>>1;
	     ADC_SS_PORT |= ADC_SS;   // SS high to stop
          break;
        default:
           val = 555;
          break;
      }
      
      total+=val;	//sum values along row
      incValue(colskip); // go to next column
    }
	
    *pimg = total>>4; // store pixel divide to avoid overflow
    pimg++; // advance pointer

    setPointer(SMH_SYS_ROWSEL);
    incValue(rowskip); // go to next row
  }

  if((ADCType!=SMH1_ADCTYPE_ONBOARD)&&(ADCType!=SMH1_ADCTYPE_MCP3201_2))
   setADCInput(anain,0); // disable chip

}

/*********************************************************************/
//	getImageColSum
//	This function acquires a box section of a Stonyman or Hawksbill 
//	and saves to image array img.  However, each col of the image
//	is summed and returned as a single value.
//	Note that images are read out in 
//	raster manner (e.g. row wise) and stored as such in a 1D array. 
//	In this case the pointer img points to the output array. 
//
//	VARIABLES: 
//	img (output): pointer to image array, an array of signed shorts
//	rowstart: first row to acquire
//	numrows: number of rows to acquire
//	rowskip: skipping between rows (useful if binning is used)
//	colstart: first column to acquire
//	numcols: number of columns to acquire
//	colskip: skipping between columns
//	ADCType: which ADC to use, defined ADC_TYPES
//	anain (0,1,2,3): which analog input to use
//	
//	EXAMPLES:
//	getImage(img,16,8,1,24,8,1,SMH1_ADCTYPE_ONBOARD,0): 
//	Grab an 8x8 window of pixels at raw resolution starting at row 
//	16, column 24, from chip using onboard ADC at input 0
//	getImage(img,0,14,8,0,14,8,SMH1_ADCTYPE_MCP3201,2): 
//	Grab entire Stonyman chip when using
//	8x8 binning. Grab from input 2.
/*********************************************************************/

void ArduEyeSMHClass::getImageColSum(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char ADCType,char anain) 
{
  short *pimg = img; // pointer to output image array
  short val,total=0;
  unsigned char chigh,clow;
  unsigned char row,col;
  
  if(ADCType==SMH1_ADCTYPE_ONBOARD)	//if using onboard ADC
     setAnalogInput(anain);		//set analog input to Arduino
  else if(ADCType==SMH1_ADCTYPE_MCP3201_2)
  { 
     setAnalogInput(anain);
     ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }
  else	//if using external ADC
  {
    setADCInput(anain,1); // enable chip
    ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }

  // Go to first col
  setPointerValue(SMH_SYS_COLSEL,colstart);
 
  // Loop through all cols
  for (col=0; col<numcols; ++col) {
    
    // Go to first row
    setPointerValue(SMH_SYS_ROWSEL,rowstart);
  
    total=0;
    
    // Loop through all rows
    for (row=0; row<numrows; ++row) {
      
      // settling delay
      delayMicroseconds(1);

      // pulse amplifier if needed
	if (useAmp) 
        pulseInphi(2);
      
      // get data value
      delayMicroseconds(1);
      
      // get pixel value from ADC
      switch (ADCType) 
      {
        case SMH1_ADCTYPE_ONBOARD:	//onboard Arduino ADC
           val = analogRead(anain); // acquire pixel
	    break;
        case SMH1_ADCTYPE_MCP3001:  // Micrchip 10 bit
           ADC_SS_PORT &= ~ADC_SS;  // turn SS low to start conversion
           chigh=SPI.transfer(0);   // get high byte
           clow=SPI.transfer(0);    // get low byte
           val = ((short)(chigh&0x1F))<<5;
           val += (clow&0xF8)>>3;
           ADC_SS_PORT |= ADC_SS;   // SS high to stop
          break;
        case SMH1_ADCTYPE_MCP3201:  // Microchip 12 bit
        case SMH1_ADCTYPE_MCP3201_2:
	     ADC_SS_PORT &= ~ADC_SS;  // turn SS low to start conversion
           chigh=SPI.transfer(0);   // get high byte
           clow=SPI.transfer(0);    // get low byte
           val = ((short)(chigh&0x1F))<<7;
           val += (clow&0xFE)>>1;
	     ADC_SS_PORT |= ADC_SS;   // SS high to stop
          break;
        default:
           val = 555;
          break;
      }
      
      total+=val;	//sum value along column
      incValue(rowskip); // go to next row
    }
	
    *pimg = total>>4; // store pixel
    pimg++; // advance pointer

    setPointer(SMH_SYS_COLSEL);
    incValue(colskip); // go to next col
  }

  if((ADCType!=SMH1_ADCTYPE_ONBOARD)&&(ADCType!=SMH1_ADCTYPE_MCP3201_2))
   setADCInput(anain,0); // disable chip

}


/*********************************************************************/
//	findMax
//	Searches over a block section of a Stonyman or Hawksbill chip
//	to find the brightest pixel. This function is intended to be used 
//	for things like finding the location of a pinhole in response to 
//	a bright light.
//
//	VARIABLES: 
//	rowstart: first row to search
//	numrows: number of rows to search
//	rowskip: skipping between rows (useful if binning is used)
//	colstart: first column to search
//	numcols: number of columns to search
//	colskip: skipping between columns
//	ADCType: which ADC to use, defined ADC_TYPES
//	anain (0,1,2,3): which analog input to use
//	rowwinner: (output) pointer to variable to write row of brightest 
//	pixel
//	colwinner: (output) pointer to variable to write column of 
//	brightest pixel
//
//	EXAMPLE:
//	FindMaxSlow(8,104,1,8,104,1,SMH1_ADCTYPE_ONBOARD,0,&rowwinner,
//	&colwinner): 
//	Search rows 8...104 and columns 8...104 for brightest pixel, with 
//	onboard ADC, chip 0
/*********************************************************************/

void ArduEyeSMHClass::findMax(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols,unsigned char colskip, char ADCType,char anain,unsigned char *max_row, unsigned char *max_col)
{
  unsigned short maxval=5000,minval=0,val;
  unsigned char row,col,bestrow,bestcol;
  unsigned char chigh,clow;

  
  if(ADCType==SMH1_ADCTYPE_ONBOARD)	//if using onboard ADC
     setAnalogInput(anain);	//set Arduino analog input
  else
  {
    setADCInput(anain,1); // enable chip
    ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }

  // Go to first row
  setPointerValue(SMH_SYS_ROWSEL,rowstart);

  // Loop through all rows
  for (row=0; row<numrows; ++row) {

    // Go to first column
    setPointerValue(SMH_SYS_COLSEL,colstart);

    // Loop through all columns
    for (col=0; col<numcols; ++col) {

      // settling delay
      delayMicroseconds(1);

      // pulse amplifier if needed
	if (useAmp) 
        pulseInphi(2);
      
      // get data value
      delayMicroseconds(1);
      
      // get pixel from ADC
      switch (ADCType) {
        case SMH1_ADCTYPE_ONBOARD:	//onboard Arduino ADC
           val = analogRead(anain); // acquire pixel
	    break;
        case SMH1_ADCTYPE_MCP3001: // Micrchip 10 bit
           ADC_SS_PORT &= ~ADC_SS; // turn SS low to start conversion
           chigh=SPI.transfer(0); // get high byte
           clow=SPI.transfer(0); // get low byte
           val = ((short)(chigh&0x1F))<<5;
           val += (clow&0xF8)>>3;
           ADC_SS_PORT |= ADC_SS; // SS high to stop
          break;
        case SMH1_ADCTYPE_MCP3201: // Microchip 12 bit
	     ADC_SS_PORT &= ~ADC_SS; // turn SS low to start conversion
           chigh=SPI.transfer(0); // get high byte
           clow=SPI.transfer(0); // get low byte
           val = ((short)(chigh&0x1F))<<7;
           val += (clow&0xFE)>>1;
	     ADC_SS_PORT |= ADC_SS; // SS high to stop
          break;
        default:
           val = 555;
          break;
      }

	if(useAmp)	//amplifier is inverted
	{
		if (val>minval) 	//find max val (bright)
		{
       	 bestrow=row;
       	 bestcol=col;
      	 minval=val;
      	}
	}
	else		//unamplified
	{
      	if (val<maxval) 	//find min val (bright)
		{
        	 bestrow=row;
        	 bestcol=col;
        	 maxval=val;
      	}
	}

      incValue(colskip); // go to next column
    }
    setPointer(SMH_SYS_ROWSEL);
    incValue(rowskip); // go to next row
  }

  if(ADCType!=SMH1_ADCTYPE_ONBOARD)
   setADCInput(anain,0); // disable chip

  // Optionally we can comment out these next three items
  //Serial.print("bestrow = "); Serial.println((short)bestrow);
  //Serial.print("bestcol = "); Serial.println((short)bestcol);
  //Serial.print("maxval = "); Serial.println((short)maxval);

  *max_row = bestrow;
  *max_col = bestcol;
}

/*********************************************************************/
//	chipToMatlab
//	This function dumps the entire contents of a Stonyman or 
//	Hawksbill chip to the Serial monitor in a form that may be copied 
//	into Matlab. The image is written be stored in matrix Img. 
//
//	VARIABLES: 
//	whichchip(0 or 1): 0 for Stonyman, 1 for Hawksbill
//	ADCType: which ADC to use, defined ADC_TYPES
//	anain (0,1,2,3): Selects one analog input
/*********************************************************************/

void ArduEyeSMHClass::chipToMatlab(char whichchip,char ADCType, char anain) 
{
  unsigned char row,col,rows,cols;
  unsigned short val;
  unsigned char chigh,clow;

  
  if(ADCType==SMH1_ADCTYPE_ONBOARD)	//if onboard ADC
     setAnalogInput(anain);		//set analog input
  else
  {
    setADCInput(anain,1); // enable chip
    ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }


  if (whichchip==1) {
	  rows=cols=136;	//hawksbill
  }	else {
	  rows=cols=112;	//stonyman
  }	
  
  Serial.println("Img = [");
  setPointerValue(SMH_SYS_ROWSEL,0); // set row = 0
  for (row=0; row<rows; ++row) {
    setPointerValue(SMH_SYS_COLSEL,0); // set column = 0
    for (col=0; col<cols; ++col) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useAmp) 
            pulseInphi(2);
      
      // get data value
      delayMicroseconds(1);
      
      // get pixel from ADC
      switch (ADCType) {
        case SMH1_ADCTYPE_ONBOARD:
           val = analogRead(anain); // acquire pixel
	    break;
        case SMH1_ADCTYPE_MCP3001: // Micrchip 10 bit
           ADC_SS_PORT &= ~ADC_SS; // turn SS low to start conversion
           chigh=SPI.transfer(0); // get high byte
           clow=SPI.transfer(0); // get low byte
           val = ((short)(chigh&0x1F))<<5;
           val += (clow&0xF8)>>3;
           ADC_SS_PORT |= ADC_SS; // SS high to stop
          break;
        case SMH1_ADCTYPE_MCP3201: // Microchip 12 bit
	     ADC_SS_PORT &= ~ADC_SS; // turn SS low to start conversion
           chigh=SPI.transfer(0); // get high byte
           clow=SPI.transfer(0); // get low byte
           val = ((short)(chigh&0x1F))<<7;
           val += (clow&0xFE)>>1;
	     ADC_SS_PORT |= ADC_SS; // SS high to stop
          break;
        default:
           val = 555;
          break;
      }

      // increment column
      incValue(1);
      Serial.print(val);
      Serial.print(" ");
    }
    setPointer(SMH_SYS_ROWSEL); // point to row
    incValue(1); // increment row
    Serial.println(" ");
  }
  Serial.println("];");
  if(ADCType!=SMH1_ADCTYPE_ONBOARD)
    setADCInput(anain,0); // disable chip

}

/*********************************************************************/
//	sectionToMatlab
//	This function dumps a box section of a Stonyman or Hawksbill 
//	to the Serial monitor in a form that may be copied into Matlab. 
//	The image is written to be stored in matrix Img. 
//
//	VARIABLES: 
//	rowstart: first row to acquire
//	numrows: number of rows to acquire
//	rowskip: skipping between rows (useful if binning is used)
//	colstart: first column to acquire
//	numcols: number of columns to acquire
//	colskip: skipping between columns
//	ADCType: which ADC to use, defined ADC_TYPES
//	anain (0,1,2,3): which analog input to use
//
//	EXAMPLES:
//	sectionToMatlab(16,8,1,24,8,1,SMH1_ADCTYPE_ONBOARD,0): 
//	Grab an 8x8 window of pixels at raw resolution starting at row 
//	16, column 24, from onboard ADC at chip 0
//	sectionToMatlab(0,14,8,0,14,8,SMH1_ADCTYPE_ONBOARD,2): 
//	Grab entire Stonyman chip when using 8x8 binning. Grab from input 
//	2.
/*********************************************************************/

void ArduEyeSMHClass::sectionToMatlab(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char ADCType, unsigned char anain) 
{
  short val;
  unsigned char row,col;
  unsigned char clow,chigh;

  if(ADCType==SMH1_ADCTYPE_ONBOARD)	//if onboard ADC
     setAnalogInput(anain);
  else
  {
    setADCInput(anain,1); // enable chip
    ADC_SS_PORT |= ADC_SS; // make sure SS is high
  }

  Serial.println("Img = [");
  setPointerValue(SMH_SYS_ROWSEL,rowstart);

  for (row=0; row<numrows; row++) {

    setPointerValue(SMH_SYS_COLSEL,colstart);

    for (col=0; col<numcols; col++) {
      // settling delay
      delayMicroseconds(1);

      // pulse amplifier if needed
      if (useAmp) 
        pulseInphi(2);
      
	delayMicroseconds(1);

      // get pixel from ADC
      switch (ADCType) {
        case SMH1_ADCTYPE_ONBOARD:
           val = analogRead(anain); // acquire pixel
	    break;
        case SMH1_ADCTYPE_MCP3001: // Micrchip 10 bit
           ADC_SS_PORT &= ~ADC_SS; // turn SS low to start conversion
           chigh=SPI.transfer(0); // get high byte
           clow=SPI.transfer(0); // get low byte
           val = ((short)(chigh&0x1F))<<5;
           val += (clow&0xF8)>>3;
           ADC_SS_PORT |= ADC_SS; // SS high to stop
          break;
        case SMH1_ADCTYPE_MCP3201: // Microchip 12 bit
	     ADC_SS_PORT &= ~ADC_SS; // turn SS low to start conversion
           chigh=SPI.transfer(0); // get high byte
           clow=SPI.transfer(0); // get low byte
           val = ((short)(chigh&0x1F))<<7;
           val += (clow&0xFE)>>1;
	     ADC_SS_PORT |= ADC_SS; // SS high to stop
          break;
        default:
           val = 555;
          break;
      }

      incValue(colskip);
      Serial.print(val);
      Serial.print(" ");
    }
    setPointer(SMH_SYS_ROWSEL);
    incValue(rowskip); // go to next row
    Serial.println(" ");
  }
  Serial.println("];");

  if(ADCType!=SMH1_ADCTYPE_ONBOARD)
    setADCInput(anain,0); // disable chip

}
