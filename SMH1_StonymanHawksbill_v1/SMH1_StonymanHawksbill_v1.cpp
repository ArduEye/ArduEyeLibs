/*
SMH1_StonymanHawksbill_v1.c

Basic functions to operate a Stonyman or Hawksbill vision chip using the 
ArduEye Rocket system.

Working revision started November 19, 2011
November 27, 2011: significant overhaul- new names for constants and added
"slow" and regular versions of many functions. Also added options to select
from multiple inputs.
November 28, 2001: renamed. Added SMH1 prefix to all macros and functions.
Added SPI functions. Should be re-written but still unverified.
January 21, 2012: renamed to _v1
January 22, 2012: initial release
February 17, 2012: includes Arduino.h or WProgram.h depending on Arduino IDE version
*/
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
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif
#include <SPI.h>
#include "SMH1_StonymanHawksbill_v1.h"

/*-----------------------------------------------------------------------------
SMH1_ConfigureBiasesSetRaw -- Initializes the biases according to the supply voltage
and turns the chip on by setting Config system register to 0x10. Note that this
puts the chip into raw mode e.g. bypasses the amplifier. This function uses the
digitalWrite ports since speed is really not an issue
STATUS: USED
*/
void SMH1_ConfigureBiasesSetRaw(char vddtype) {
  unsigned char nbias,aobias,vref;
  // determine biases. Only one option for now.
  switch (vddtype) {
    case SMH1_VDD_5V0:
    default:
      nbias = 55;
      aobias = 55;
      vref = 30;
  }
  SMH1_SetPtrValSlow(SMH_SYS_NBIAS,nbias); // set nbias
  SMH1_SetPtrValSlow(SMH_SYS_AOBIAS,aobias); // set aobias
  SMH1_SetPtrValSlow(SMH_SYS_VREF,vref); // set vref
  SMH1_SetPtrValSlow(SMH_SYS_CONFIG,0x10); // turn chip on yay!
}

/*-----------------------------------------------------------------------------
SMH1_ConfigureAnalogInput -- Configures the DDRC register to ensure the appropriate
A# pin is configured as an input. Variable "anain" is between 0 and 3. This is
for chips plugged into slow ports.
STATUS: USED
*/
void SMH1_ConfigureAnalogInput(char anain){
  switch (anain){
    case 0:
      DDRC &= 0xFE;
      break;
    case 1:
      DDRC &= 0xFD;
      break;
    case 2:
      DDRC &= 0xFB;
      break;
    case 3:
      DDRC &= 0xF7;
      break;
  }
}

/*-----------------------------------------------------------------------------
SMH1_SelectChipSPIADC -- Configures the DDRC register and operates the appropriate
pin for when using the SPI ADC. Variable "anain" indicates which chip to use. 
Variable "cs" is 1 or 0 and indicates whether chip is selected.
This is for chips plugged into fast ports.
STATUS: USED
*/
void SMH1_SelectChipSPIADC(char anain, char cs) {
  // make sure A# pin is set to be a digital output
  switch (anain){
    case 0:
      DDRC |= 0x01; // set A0 to output
      if (cs)
        PORTC |= 0x01; // turn on
      else
        PORTC &= 0xFE; // turn off
      break;
    case 1:
      DDRC |= 0x02; // set A1 to output
      if (cs)
        PORTC |= 0x02; // turn on
      else
        PORTC &= 0xFD; // turn off
      break;
    case 2:
      DDRC |= 0x04; // set A2 to output
      if (cs)
        PORTC |= 0x04; // turn on
      else
        PORTC &= 0xFB; // turn off
      break;
    case 3:
      DDRC |= 0x08; // set A3 to output
      if (cs)
        PORTC |= 0x08; // turn on
      else
        PORTC &= 0xF7; // turn off
      break;
  }
}

/*-----------------------------------------------------------------------------
SMH1_SetupIO -- Sets up the five digital pins going from the Arduino to the 
vision chip and sets them to the default digital low position
STATUS: USED
*/
void SMH1_SetupIO(void) {
  // set all digital pins to output and set to zero
  pinMode(SMH1_RESP,OUTPUT); 
  digitalWrite(SMH1_RESP,LOW);
  pinMode(SMH1_INCP,OUTPUT); 
  digitalWrite(SMH1_INCP,LOW);
  pinMode(SMH1_RESV,OUTPUT); 
  digitalWrite(SMH1_RESV,LOW);
  pinMode(SMH1_INCV,OUTPUT); 
  digitalWrite(SMH1_INCV,LOW);
  pinMode(SMH1_INPHI,OUTPUT);
  digitalWrite(SMH1_INPHI,LOW);
}

/*-----------------------------------------------------------------------------
SMH1_SetPtrSlow -- Sets the pointer system register to the desired value ptr. This
is the "slow" version that uses the ArduEye digitalWrite function.
STATUS: USED
*/
void SMH1_SetPtrSlow(char ptr) {
  char i;
  // clear pointer
  digitalWrite(SMH1_RESP,HIGH);
  delayMicroseconds(1);
  digitalWrite(SMH1_RESP,LOW);
  delayMicroseconds(1);
  // increment pointer
  for (i=0; i!=ptr; ++i) {
    digitalWrite(SMH1_INCP,HIGH);
    delayMicroseconds(1);
    digitalWrite(SMH1_INCP,LOW);
    delayMicroseconds(1);
  }
}
  
/*-----------------------------------------------------------------------------
SMH1_SetPtrFast -- Sets the pointer system register to the desired value ptr. This
is the "fast" version using pulse macros.
STATUS: USED
*/
void SMH1_SetPtrFast(char ptr) {
  char i;
  // clear pointer
  SMH1_ResP_Pulse; // macro
  // increment pointer
  for (i=0; i!=ptr; ++i) {
    SMH1_IncP_Pulse; // macro
  }
}

/*-----------------------------------------------------------------------------
SMH1_SetValSlow -- Sets the selected system register to the desired value val. This
is the "slow" version that uses the ArduEye digitalWrite function.
STATUS: USED
*/
void SMH1_SetValSlow(short val) {
  short i;
  // clear value
  digitalWrite(SMH1_RESV,HIGH);
  delayMicroseconds(1);
  digitalWrite(SMH1_RESV,LOW);
  delayMicroseconds(1);
  // increment value
  for (i=0; i!=val; ++i) {
    digitalWrite(SMH1_INCV,HIGH);
    delayMicroseconds(1);
    digitalWrite(SMH1_INCV,LOW);
    delayMicroseconds(1);
  }
}

/*-----------------------------------------------------------------------------
SMH1_SetValFast -- Sets the selected system register to the desired value val. This
is the "fast" version that uses pulse macros.
STATUS: USED
*/
void SMH1_SetValFast(short val) {
  short i;
  // clear pointer
  SMH1_ResV_Pulse; // macro
  // increment pointer
  for (i=0; i!=val; ++i) {
    SMH1_IncV_Pulse; // macro
  }
}

/*-----------------------------------------------------------------------------
SMH1_ResvSlow -- Resets the selected system register to zero. This
is the "slow" version that uses the ArduEye digitalWrite function.
Status: Untested
*/
void SMH1_ResvSlow(void) {
  digitalWrite(SMH1_RESV,HIGH);
  delayMicroseconds(1);
  digitalWrite(SMH1_RESV,LOW);
}

/*-----------------------------------------------------------------------------
SMH1_IncvSlow -- Increments the selected system register by val. This
is the "slow" version that uses the ArduEye digitalWrite function.
STATUS: USED
*/
void SMH1_IncvSlow(short val) {
  short i;
  for (i=0; i<val; ++i) {
    digitalWrite(SMH1_INCV,HIGH);
    digitalWrite(SMH1_INCV,LOW);
  }
}

/*-----------------------------------------------------------------------------
SMH1_IncvFast -- Increments the selected system register by val. This
is the "fast" version that uses the pulse macros.
STATUS: USED
*/
void SMH1_IncvFast(short val) {
  short i;
  for (i=0; i<val; ++i) {
    SMH1_IncV_Pulse;
  }
}

/*-----------------------------------------------------------------------------
SMH1_SetPtrValSlow -- Selects a system register and the sets it to a desired value. This
is the "slow" version that uses the ArduEye digitalWrite function.
STATUS: USED
*/
void SMH1_SetPtrValSlow(char ptr, short val) {
  SMH1_SetPtrSlow(ptr);
  SMH1_SetValSlow(val);
}

/*-----------------------------------------------------------------------------
SMH1_SetPtrValFast -- Selects a system register and the sets it to a desired value. This
is the "fast" version that uses pulse macros.
STATUS: USED
*/
void SMH1_SetPtrValFast(char ptr, short val) {
  SMH1_SetPtrFast(ptr);
  SMH1_SetValFast(val);
}

/*-----------------------------------------------------------------------------
SMH1_ClearAllValsSlow -- Clears all system registers. This
is the "slow" version that uses the ArduEye digitalWrite function. A fast
version is generally not needed.
STATUS: USED
*/
void SMH1_ClearAllValsSlow(void) {
  char i;
  for (i=0; i!=8; ++i)
    SMH1_SetPtrValSlow(i,0);
}

/*-----------------------------------------------------------------------------
SMH1_ConfigSlow -- Sets the "CONFIG" system register which includes both turning on 
or off the bias generators and configuring the big amplifier. This is the 
"slow" version that uses the ArduEye digitalWrite function. Generally this 
will be adequate unless there is a need to acquire individual pixels at 
different gains.
VARIABLES
gain = 0,1,...,7: This is the gain of the amplifier, between 0 and 7. Of course a gain of
zero is meaningless and a gain of one may be obtained by bypassing the
amplifier, but these options are available here.
selamp = 0,1: set to 1 to use the amplifier and 0 to bypass it.
cvdda = 1: set to 1 to turn on the bias generators. Generally this is necessary
unless the bias signals are externally provided, which is not really an option.
So generally you should use 1 for this value.
EXAMPLE 1: To configure the chip to bypass the amplifier:
ConfigBigAmpSlow(0,0,1);
EXAMPLE 2: To use the amplifier and set the gain to 4:
ConfigBigAmpSlow(4,1,1);
STATUS: USED
*/
void SMH1_ConfigSlow(char gain, char selamp, char cvdda) {
  char val = gain + (selamp * 0x08) + (cvdda * 0x10);
  short vals=val;
  // Note that val and vals will have the following form binary form:
  // 000csggg where c=cvdda, s=selamp, ggg=gain (3 bits)
  // Note that there is no overflow detection in the input values.
  SMH1_SetPtrValSlow(SMH_SYS_CONFIG,vals);
}

/*-----------------------------------------------------------------------------
SMH1_ConfigFast -- Identical to ConfigSlow above except uses pulse macros and is faster
STATUS: USED
*/
void SMH1_ConfigFast(char gain, char selamp, char cvdda) {
  char val = gain + (selamp * 0x08) + (cvdda * 0x10);
  short vals=val;
  // Note that val and vals will have the following form binary form:
  // 000csggg where c=cvdda, s=selamp, ggg=gain (3 bits)
  // Note that there is no overflow detection in the input values.
  SMH1_SetPtrValFast(SMH_SYS_CONFIG,vals);
}

/*-----------------------------------------------------------------------------
SMH1_PulseInphiSlow -- Pulses Inphi, with a delay between rise and fall to hold
Inphi high for a set amount of time. This is the "slow" version that uses the 
ArduEye digitalWrite function.
STATUS: USED
*/
void SMH1_PulseInphiSlow(char delay) {
  digitalWrite(SMH1_INPHI,HIGH);
  delayMicroseconds(delay);
  digitalWrite(SMH1_INPHI,LOW);
}

/*-----------------------------------------------------------------------------
SMH1_PulseInphiFast -- Pulses Inphi, with a delay between rise and fall to hold
Inphi high for a set amount of time. This is the "fast" version that uses 
pulse macros.
STATUS: USED
*/
void SMH1_PulseInphiFast(char delay) {
  SMH1_InPhi_SetHigh;
  delayMicroseconds(delay);
  SMH1_InPhi_SetLow;
}

/*-----------------------------------------------------------------------------
SMH1_SetBasicBinning -- Configures binning in the focal plane using the VSW and HSW
system registers. The super pixels are aligned with the top left of the image
e.g. "offset downsampling" is not used. This function is for the Stonyman chip 
only. This function uses the Arduino digitalWrite function since it will not be
called often so slow operation is tolerable.
VARIABLES:
hbin: set to 1, 2, 4, or 8 to bin horizontally by that amount
vbin: set to 1, 2, 4, or 8 to bin vertically by that amount
Status: Untested
*/
void SMH1_SetBasicBinning(unsigned char hbin, unsigned char vbin)
{
  switch (hbin) {
    case 2:
      hbin = 0xAA;
      break;
    case 4:
      hbin = 0xEE;
      break;
    case 8:
      hbin = 0xFE;
      break;
    default:
      hbin = 0x00;
  }
  switch (vbin) {
    case 2:
      vbin = 0xAA;
      break;
    case 4:
      vbin = 0xEE;
      break;
    case 8:
      vbin = 0xFE;
      break;
    default:
      vbin = 0x00;
  }
  SMH1_SetPtrValSlow(SMH_SYS_HSW,hbin);
  SMH1_SetPtrValSlow(SMH_SYS_VSW,vbin);
}

/*-----------------------------------------------------------------------------
SMH1_MatlabDumpEntireChip -- This function dumps the entire contents of a Stonyman or 
Hawksbill chip to the Serial monitor in a form that may be copied into Matlab. The image
is written be stored in matrix Img. This function uses the Arduino's ADC and thus
is for chips plugged into the slow sockets. This function also uses the 
Arduino digitalWrite function since the limiting speed factor is the
Serial downlink.
VARIABLES: 
anain (0,1,2,3): Selects one analog input
useamp (0 or 1): 0 to not use amp, 1 to use amp (pulses Inphi). This value
should be equal to the "selamp" variable used in the ConfigSlow function
above.
whichchip(0 or 1): 0 for Stonyman, 1 for Hawksbill
STATUS: USED
*/
void SMH1_MatlabDumpEntireChip(char anain, char useamp, char whichchip) {
  unsigned char row,col,rows,cols;
  unsigned short val;
  
  SMH1_ConfigureAnalogInput(anain); // Make sure analog pin is configured as input
  
  if (whichchip==1) {
	  rows=cols=136;
  }	else {
	  rows=cols=112;
  }	
  
  Serial.println("Img = [");
  SMH1_SetPtrValSlow(SMH_SYS_ROWSEL,0); // set row = 0
  for (row=0; row<rows; ++row) {
    SMH1_SetPtrValSlow(SMH_SYS_COLSEL,0); // set column = 0
    for (col=0; col<cols; ++col) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useamp) {
        SMH1_PulseInphiSlow(2);
      }
      // get data value
      delayMicroseconds(1);
      val = analogRead(anain);
      // increment column
      SMH1_IncvSlow(1);
      Serial.print(val);
      Serial.print(" ");
    }
    SMH1_SetPtrSlow(SMH_SYS_ROWSEL); // point to row
    SMH1_IncvSlow(1); // increment row
    Serial.println(" ");
  }
  Serial.println("];");
}

/*-----------------------------------------------------------------------------
SMH1_MatlabDumpChipSection -- This function dumps a box section of a Stonyman or Hawksbill 
to the Serial monitor in a form that may be copied into Matlab. The image
is written to be stored in matrix Img. This function uses the Arduino's ADC and thus
is for chips plugged into the slow sockets. This function also uses the 
Arduino digitalWrite function since the limiting speed factor is the
Serial downlink.
VARIABLES: 
rowstart: first row to acquire
numrows: number of rows to acquire
rowskip: skipping between rows (useful if binning is used)
colstart: first column to acquire
numcols: number of columns to acquire
colskip: skipping between columns
anain (0,1,2,3): which analog input to use
useamp (0 or 1): 0 to not use amp, 1 to use amp (pulses Inphi). This value
should be equal to the "selamp" variable used in the ConfigSlow function
above.
EXAMPLES:
MatlabDumpChipSection(16,8,1,24,8,1,0): Grab an 8x8 window of pixels at raw 
resolution starting at row 16, column 24, from chip at input 0
MatlabDumpChipSection(0,14,8,0,14,8,2): Grab entire Stonyman chip when using
8x8 binning. Grab from input 2.
STATUS: USED
*/
void SMH1_MatlabDumpChipSection(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, unsigned char anain, char useamp) {
  short val;
  unsigned char row,col;

  SMH1_ConfigureAnalogInput(anain); // Make sure analog pin is configured as input
  
  Serial.println("Dat = [");
  SMH1_SetPtrValSlow(SMH_SYS_ROWSEL,rowstart);
  for (row=0; row<numrows; row++) {
    SMH1_SetPtrValSlow(SMH_SYS_COLSEL,colstart);
    for (col=0; col<numcols; col++) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useamp) {
        SMH1_PulseInphiSlow(2);
      }
      // get data value
      delayMicroseconds(1);
      val = analogRead(0); // acquire
      SMH1_IncvSlow(colskip);
      Serial.print(val);
      Serial.print(" ");
    }
    SMH1_SetPtrSlow(SMH_SYS_ROWSEL);
    SMH1_IncvSlow(rowskip); // go to next row
    Serial.println(" ");
  }
  Serial.println("];");
}

/*-----------------------------------------------------------------------------
SMH1_MatlabDumpChipSectionSPIADC -- This function dumps a box section of a Stonyman or Hawksbill 
to the Serial monitor in a form that may be copied into Matlab. The image
is written to be stored in matrix Img. This function uses the SPI ADC and thus
is for chips plugged into the slow sockets. This function also uses the 
Arduino digitalWrite function since the limiting speed factor is the
Serial downlink.
VARIABLES: 
rowstart: first row to acquire
numrows: number of rows to acquire
rowskip: skipping between rows (useful if binning is used)
colstart: first column to acquire
numcols: number of columns to acquire
colskip: skipping between columns
anain (0,1,2,3): which analog input to use
useamp (0 or 1): 0 to not use amp, 1 to use amp (pulses Inphi). This value
should be equal to the "selamp" variable used in the ConfigSlow function
above.
ADCType: Which ADC is used
EXAMPLES:
MatlabDumpChipSectionSPIADC(16,8,1,24,8,1,0,SMH1_ADCTYPE_MCP3201): Grab an 8x8 window of pixels at raw 
resolution starting at row 16, column 24, from chip at input 0. Use MCP3201 ADC
MatlabDumpChipSectionSPIADC(0,14,8,0,14,8,2,SMH1_ADCTYPE_MCP3201): Grab entire Stonyman chip when using
8x8 binning. Grab from input 2. Use MCP3201 ADC
STATUS: USED
*/
void SMH1_MatlabDumpChipSectionSPIADC(unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, unsigned char anain, unsigned char useamp, char ADCType) {
  short val;
  unsigned char row,col;

  SMH1_SelectChipSPIADC(anain,1); // enable chip
  PORTB |= 0x04; // make sure SS is high
  
  Serial.println("Dat = [");
  SMH1_SetPtrValSlow(SMH_SYS_ROWSEL,rowstart);
  for (row=0; row<numrows; row++) {
    SMH1_SetPtrValSlow(SMH_SYS_COLSEL,colstart);
    for (col=0; col<numcols; col++) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useamp) {
        SMH1_PulseInphiSlow(2);
      }
      // final delay
      delayMicroseconds(1);
      // This next sequence acquires a pixel from the ADC
      PORTB &= 0xFB; // turn SS low to start conversion
      unsigned char chigh=SPI.transfer(0); // get high byte
      unsigned char clow=SPI.transfer(0); // get low byte
      // extract 10 or 12 bit val from clow and chigh
      switch (ADCType) {
        case SMH1_ADCTYPE_MCP3001: // Micrchip 10 bit
          val = ((short)(chigh&0x1F))<<5;
          val += (clow&0xF8)>>3;
          break;
        case SMH1_ADCTYPE_MCP3201: // Microchip 12 bit
          val = ((short)(chigh&0x1F))<<7;
          val += (clow&0xFE)>>1;
          break;
        default:
          val = 555;
          break;
      }
      PORTB |= 0x04; // turn SS high to stop conversion and readout      
      SMH1_IncvSlow(colskip);
      Serial.print(val);
      Serial.print(" ");
    }
    SMH1_SetPtrSlow(SMH_SYS_ROWSEL);
    SMH1_IncvSlow(rowskip); // go to next row
    Serial.println(" ");
  }
  Serial.println("];");
  SMH1_SelectChipSPIADC(anain,0); // disable chip
  PORTB |= 0x04; // again make sure SS is high
}

/*-----------------------------------------------------------------------------
SMH1_FindMaxSlow -- Searches over a block section of a Stonyman or Hawksbill chip
to find the brightest pixel. This function is intended to be used for things 
like finding the location of a pinhole in response to a bright light.
This function uses the Arduino's ADC and thus is for chips plugged into the 
slow sockets. This function also uses the Arduino digitalWrite function.
VARIABLES: 
rowstart: first row to search
numrows: number of rows to search
colstart: first column to acquire
numcols: number of columns to acquire
anain (0,1,2,3): which analog input to use
rowwinner: (output) pointer to variable to write row of brightest pixel
colwinner: (output) pointer to variable to write column of brightest pixel
EXAMPLE:
FindMaxSlow(8,104,8,104,0,&rowwinner,&colwinner): Search rows 8...104 and
columns 8...104 for brightest pixel, from chip attached to slow input 0
STATUS: USED
*/
void SMH1_FindMaxSlow(unsigned char rowstart, unsigned char numrows, unsigned char colstart, unsigned char numcols, char anain, unsigned char *rowwinner, unsigned char *colwinner) {
  unsigned short maxval=5000,val;
  unsigned char row,col,bestrow,bestcol;
  
  SMH1_ConfigureAnalogInput(anain); // Make sure analog pin is configured as input
  // go to first row
  SMH1_SetPtrValSlow(SMH_SYS_ROWSEL,rowstart);
  row = rowstart;
  // loop through all rows
  for(; row < rowstart+numrows; ++row) {
    // go to first column
    SMH1_SetPtrValSlow(SMH_SYS_COLSEL,colstart);
    col = colstart;
    // loop through all columns
    for (; col < colstart+numcols; ++col) {
      delayMicroseconds(3); // delay
      val = analogRead(anain); // get pixel
      // if pixel is brighter than previous brightest pixel then update.
      // Note that brighter pixels output a lower value, hence the 
      // "less than" comparison
      if (val<maxval) {
        bestrow=row;
        bestcol=col;
        maxval=val;
      }
      SMH1_IncvSlow(1); // go to next column
    }
    // go to next row
    SMH1_SetPtrSlow(SMH_SYS_ROWSEL);
    SMH1_IncvSlow(1);
  }
  // Optionally we can comment out these next three items
  Serial.print("bestrow = "); Serial.println((short)bestrow);
  Serial.print("bestcol = "); Serial.println((short)bestcol);
  Serial.print("maxval = "); Serial.println((short)maxval);
  // write location of brightest pixel to rowwinner and colwinner
  *rowwinner = bestrow;
  *colwinner = bestcol;
}

/*-----------------------------------------------------------------------------
SMH1_FindMaxSPIADC -- Searches over a block section of a Stonyman or Hawksbill chip
to find the brightest pixel. This function is intended to be used for things 
like finding the location of a pinhole in response to a bright light.
This function uses the SPI ADC and thus is for chips plugged into the 
fast sockets. This function also uses the Arduino digitalWrite function.
VARIABLES: 
rowstart: first row to search
numrows: number of rows to search
colstart: first column to acquire
numcols: number of columns to acquire
anain (0,1,2,3): which analog input to use
rowwinner: (output) pointer to variable to write row of brightest pixel
colwinner: (output) pointer to variable to write column of brightest pixel

EXAMPLE:
FindMaxSPIADC(8,104,8,104,0,SMH1_ADCTYPE_MCP3201,&rowwinner,&colwinner): Search rows 8...104 and
columns 8...104 for brightest pixel, from chip attached to slow input 0. Use MCP3201 ADC
STATUS: USED
*/
void SMH1_FindMaxSPIADC(unsigned char rowstart, unsigned char numrows, unsigned char colstart, unsigned char numcols, char anain, char ADCType, unsigned char *rowwinner, unsigned char *colwinner) {
  unsigned short maxval=5000,val;
  unsigned char row,col,bestrow,bestcol;
  
  SMH1_SelectChipSPIADC(anain,1); // enable chip
  PORTB |= 0x04; // make sure SS is high
  // go to first row
  SMH1_SetPtrValSlow(SMH_SYS_ROWSEL,rowstart);
  row = rowstart;
  // loop through all rows
  for(; row < rowstart+numrows; ++row) {
    // go to first column
    SMH1_SetPtrValSlow(SMH_SYS_COLSEL,colstart);
    col = colstart;
    // loop through all columns
    for (; col < colstart+numcols; ++col) {
      delayMicroseconds(3); // delay
      // This next sequence acquires a pixel from the ADC
      PORTB &= 0xFB; // turn SS low to start conversion
      unsigned char chigh=SPI.transfer(0); // get high byte
      unsigned char clow=SPI.transfer(0); // get low byte
      // extract 10 or 12 bit val from clow and chigh
      switch (ADCType) {
        case SMH1_ADCTYPE_MCP3001: // Micrchip 10 bit
          val = ((short)(chigh&0x1F))<<5;
          val += (clow&0xF8)>>3;
          break;
        case SMH1_ADCTYPE_MCP3201: // Microchip 12 bit
          val = ((short)(chigh&0x1F))<<7;
          val += (clow&0xFE)>>1;
          break;
        default:
          val = 555;
          break;
      }
      PORTB |= 0x04; // turn SS high to stop conversion and readout  
      
      // if pixel is brighter than previous brightest pixel then update.
      // Note that brighter pixels output a lower value, hence the 
      // "less than" comparison
      if (val<maxval) {
        bestrow=row;
        bestcol=col;
        maxval=val;
      }
      SMH1_IncvSlow(1); // go to next column
    }
    // go to next row
    SMH1_SetPtrSlow(SMH_SYS_ROWSEL);
    SMH1_IncvSlow(1);
  }
  // Optionally we can comment out these next three items
  Serial.print("bestrow = "); Serial.println((short)bestrow);
  Serial.print("bestcol = "); Serial.println((short)bestcol);
  Serial.print("maxval = "); Serial.println((short)maxval);
  // write location of brightest pixel to rowwinner and colwinner
  *rowwinner = bestrow;
  *colwinner = bestcol;
  SMH1_SelectChipSPIADC(anain,0); // disable chip
  PORTB |= 0x04; // make sure SS is high
}
  
/*-----------------------------------------------------------------------------
SMH1_GetImageSlow -- This function acquires a box section of a Stonyman or Hawksbill 
and saves to image array img. This function uses the Arduino's ADC and thus
is for chips plugged into the slow sockets. This function also uses the 
Arduino digitalWrite function. Note that images are read out in raster manner
(e.g. row wise) and stored as such in a 1D array. In this case the pointer
img points to the output array. 
VARIABLES: 
img (output): pointer to image array, an array of signed shorts
rowstart: first row to acquire
numrows: number of rows to acquire
rowskip: skipping between rows (useful if binning is used)
colstart: first column to acquire
numcols: number of columns to acquire
colskip: skipping between columns
anain (0,1,2,3): which analog input to use
useamp (0 or 1): 0 to not use amp, 1 to use amp (pulses Inphi). This value
should be equal to the "selamp" variable used in the ConfigSlow function
above.
EXAMPLES:
GetImageSlow(16,8,1,24,8,1,0): Grab an 8x8 window of pixels at raw 
resolution starting at row 16, column 24, from chip at input 0
GetImageSlow(0,14,8,0,14,8,2): Grab entire Stonyman chip when using
8x8 binning. Grab from input 2.
STATUS: USED
*/
void SMH1_GetImageSlow(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char anain, char useamp) {
  short *pimg = img; // pointer to output image array
  short val;
  unsigned char row,col;
  
  SMH1_ConfigureAnalogInput(anain); // Make sure analog pin is configured as input
  // Go to first row
  SMH1_SetPtrValSlow(SMH_SYS_ROWSEL,rowstart);
  // Loop through all rows
  for (row=0; row<numrows; ++row) {
    // Go to first column
    SMH1_SetPtrValSlow(SMH_SYS_COLSEL,colstart);
    // Loop through all columns
    for (col=0; col<numcols; ++col) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useamp) {
        SMH1_PulseInphiSlow(2);
      }
      // get data value
      delayMicroseconds(1);
      val = analogRead(anain); // acquire pixel
      *pimg = val; // store pixel
      pimg++; // advance pointer
      SMH1_IncvSlow(colskip); // go to next column
    }
    SMH1_SetPtrSlow(SMH_SYS_ROWSEL);
    SMH1_IncvSlow(rowskip); // go to next row
  }
}

/*-----------------------------------------------------------------------------
SMH1_GetImageFast -- This function acquires a box section of a Stonyman or Hawksbill 
and saves to image array img. This function uses the Arduino's ADC and thus
is for chips plugged into the slow sockets. This function uses pulse macros
and is thus faster than the "slow" version. Note that images are read out in 
raster manner (e.g. row wise) and stored as such in a 1D array. In this case 
the pointer img points to the output array. 
VARIABLES: 
img (output): pointer to image array, an array of signed shorts
rowstart: first row to acquire
numrows: number of rows to acquire
rowskip: skipping between rows (useful if binning is used)
colstart: first column to acquire
numcols: number of columns to acquire
colskip: skipping between columns
anain (0,1,2,3): which analog input to use
useamp (0 or 1): 0 to not use amp, 1 to use amp (pulses Inphi). This value
should be equal to the "selamp" variable used in the ConfigSlow function
above.
EXAMPLES:
GetImageFast(16,8,1,24,8,1,0): Grab an 8x8 window of pixels at raw 
resolution starting at row 16, column 24, from chip at input 0
GetImageFast(0,14,8,0,14,8,2): Grab entire Stonyman chip when using
8x8 binning. Grab from input 2.
STATUS: USED
*/
void SMH1_GetImageFast(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char anain, char useamp) {
  short *pimg = img; // pointer to output image array
  short val;
  unsigned char row,col;
  
  SMH1_ConfigureAnalogInput(anain); // Make sure analog pin is configured as input
  // Go to first row
  SMH1_SetPtrValFast(SMH_SYS_ROWSEL,rowstart);
  // Loop through all rows
  for (row=0; row<numrows; ++row) {
    // Go to first column
    SMH1_SetPtrValFast(SMH_SYS_COLSEL,colstart);
    // Loop through all columns
    for (col=0; col<numcols; ++col) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useamp) {
        SMH1_PulseInphiFast(2);
      }
      // get data value
      delayMicroseconds(1);
      val = analogRead(anain); // acquire pixel
      *pimg = val; // store pixel
      pimg++; // advance pointer
      SMH1_IncvFast(colskip); // go to next column
    }
    SMH1_SetPtrFast(SMH_SYS_ROWSEL);
    SMH1_IncvFast(rowskip); // go to next row
  }
}

/*-----------------------------------------------------------------------------
SMH1_GetImageSPIADC -- This function acquires a box section of a Stonyman or Hawksbill 
and saves to image array img. This function uses the external ADC and thus is
for chips plugged into the fast sockets. Variable chipsel determines which 
chip is selected. This function also uses pulse macros for fastest speed. 
Note that images are read out in raster manner (e.g. row wise) and stored as 
such in a 1D array. In this case the pointer img points to the output array. 
VARIABLES: 
img (output): pointer to image array, an array of signed shorts
rowstart: first row to acquire
numrows: number of rows to acquire
rowskip: skipping between rows (useful if binning is used)
colstart: first column to acquire
numcols: number of columns to acquire
colskip: skipping between columns
anain (0,1,2,3): which analog input to use
useamp (0 or 1): 0 to not use amp, 1 to use amp (pulses Inphi). This value
should be equal to the "selamp" variable used in the ConfigSlow function
above.
ADCType: which ADC to use
EXAMPLES:
GetImageSPIADC(16,8,1,24,8,1,0,SMH1_ADCTYPE_MCP3201): Grab an 8x8 window of pixels at raw 
resolution starting at row 16, column 24, from chip at input 0
GetImageSPIADC(0,14,8,0,14,8,2,SMH1_ADCTYPE_MCP3201): Grab entire Stonyman chip when using
8x8 binning. Grab from input 2.
STATUS: USED
*/
void SMH1_GetImageSPIADC(short *img, unsigned char rowstart, unsigned char numrows, unsigned char rowskip, unsigned char colstart, unsigned char numcols, unsigned char colskip, char anain, char useamp, char ADCType) {
  short *pimg = img; // pointer to output image array
  short val;
  unsigned char row,col;
  
  SMH1_SelectChipSPIADC(anain,1); // enable chip
  PORTB |= 0x04; // make sure SS is high
  // Go to first row
  SMH1_SetPtrValFast(SMH_SYS_ROWSEL,rowstart);
  // Loop through all rows
  for (row=0; row<numrows; ++row) {
    // Go to first column
    SMH1_SetPtrValFast(SMH_SYS_COLSEL,colstart);
    // Loop through all columns
    for (col=0; col<numcols; ++col) {
      // settling delay
      delayMicroseconds(1);
      // pulse amplifier if needed
	    if (useamp) {
        SMH1_PulseInphiFast(2);
      }
      // get data value
      delayMicroseconds(1);
      // This next sequence acquires a pixel from the ADC
      PORTB &= 0xFB; // turn SS low to start conversion
      unsigned char chigh=SPI.transfer(0); // get high byte
      unsigned char clow=SPI.transfer(0); // get low byte
      // extract 10 or 12 bit val from clow and chigh
      switch (ADCType) {
        case SMH1_ADCTYPE_MCP3001: // Micrchip 10 bit
          val = ((short)(chigh&0x1F))<<5;
          val += (clow&0xF8)>>3;
          break;
        case SMH1_ADCTYPE_MCP3201: // Microchip 12 bit
          val = ((short)(chigh&0x1F))<<7;
          val += (clow&0xFE)>>1;
          break;
        default:
          val = 555;
          break;
      }
      PORTB |= 0x04; // turn SS high to stop conversion and readout
      *pimg = val; // store pixel
      pimg++; // advance pointer
      SMH1_IncvFast(colskip); // go to next column
    }
    SMH1_SetPtrFast(SMH_SYS_ROWSEL);
    SMH1_IncvFast(rowskip); // go to next row
  }
  SMH1_SelectChipSPIADC(anain,0); // disable chip
  PORTB |= 0x04; // Again make sure SS is high
}