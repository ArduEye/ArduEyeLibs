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

#ifndef CYE_Images_v1_h
#define CYE_Images_v1_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif


void CYE_ImgShortCopy(short *A, short *B, unsigned short numpix);
void CYE_ImgShortDumpAsciiSerial(short *img, short numrows, short numcolumns, short mini, short maxi);
void CYE_ImgShortDumpMatlabSerial(short *img, unsigned char numrows, unsigned char numcols);
void CYE_ImgShortFindMinMax(short *img, unsigned char numrows, unsigned char numcols, short *mini, short *maxi);
void CYE_ImgShortFindMax(short *img, unsigned char numrows, unsigned char numcols, unsigned char polarity, unsigned char *winrow, unsigned char *wincol);
short CYE_ImgShortMin(short *A, unsigned short numpix);
short CYE_ImgShortMax(short *A, unsigned short numpix);
void CYE_ImgShortDiff(short *A, short *B, short *D, unsigned short numpix);
void CYE_ImgShortHPF(short *I, short *L, short *H, short numpix, char shiftalpha);
void CYE_ImgShortAddCharFPN(short *A, unsigned char *F, unsigned short numpix, unsigned char mult);
void CYE_ImgCharMakeFPN(unsigned char *F, unsigned short numpix, unsigned char modval);
void CYE_SubwinShort2D(short *I, short *S, char Irows, char Icols, char startrow, char numrows, char startcol, char numcols);
void CYE_SubwinShort2Dto1D(short *I, short *S, char Irows, char Icols, char subrow, char subcol, char Snumpix, char Spixlength, char orientation);
	
#endif