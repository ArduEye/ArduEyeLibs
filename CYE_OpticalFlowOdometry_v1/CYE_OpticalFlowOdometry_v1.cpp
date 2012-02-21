
/*
CYE_OpticalFlowOdometry_v1.c

Basic functions to compute optical flow and (eventually) compute odometry

Working revision started November 19, 2011
November 29, 2011: Renamed with functions renamed
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
#include "CYE_OpticalFlowOdometry_v1.h"

/*------------------------------------------------------------------------
CYE_OFO_ShortIIA_1D -- This is a one dimensional version of the image interpolation
algorithm (IIA) as described by Prof. Mandyam Srinivasan. f1 and f2 are
input line images. numpix is the number of pixels in the image. scale is
a scaling factor, mostly for the benefit of the fixed-poing arithmetic
performed here. out is the resulting output optical flow.
VARIABLES:
f1,f2: first and second images
numpix: number of pixels in line image
scale: value of one pixel of motion (for scaling output)
out: pointer to integer value for output.
STATUS: UNTESTED
*/
void CYE_OFO_ShortIIA_1D(short *f1, short *f2, char numpix, short scale, int *out) {
  short *pleft,*pright,*pone,*ptwo;
  long top,bottom;
  char i;
  
  // Set up pointers
  pleft = f1;
  pone = f1+1;
  pright = f1+2;
  ptwo = f2+1;
  
  top=bottom=0;
  
  for(i=0; i<numpix-2; ++i) {
    int deltat = *ptwo - *pone; // temporal gradient i.e. pixel change over time
    int deltax = *pright - *pleft; // spatial gradient i.e. pixel change over space
    top += deltat * deltax;
    bottom += deltax * deltax;
    // Increment pointers
    pleft++;
    pone++;
    pright++;
    ptwo++;
  }
  
  // Compute final output. Note use of "scale" here to multiply 2*top to a
  // larger number so that it may be meaningfully divided using fixed point
  // arithmetic
  *out = 2*top*scale/bottom;
}

/*------------------------------------------------------------------------
CYE_OFO_CharIIA_2D -- This function computes optical flow between two images X1 and
X2 using a simplified version of Mandyam Srinivasan's image interpolation
algorithm. This algorithm assumes that displacements are generally on 
the order of one pixel or less. The x and y displacements are sent
out as floats. (See note at bottom of this function.) The #def value
of_scale represents one pixel worth of displacement- if the displacement
between X1 and X2 is one pixel in one direction, the corresponding of
value (ofx or ofy) should be on the order of 100.
This function is hard coded for a 12x12 image. It can be adapted to
work with different array sizes by changing all instances of "12" to 
the appropriate value below where noted.
Credit- Thanks to "A.J." on Embedded Eye for optimizing this function.
Apologies for ruining it with the "float" assignments at the end.
VARIABLES:
X1,X2: first and second images
(NEED TO ADD ROWS AND COLUMNS!)
ofx,ofy: pointers to floats which hold output optical flows
STATUS: UNTESTED
*/
void CYE_OFO_CharIIA_2D(char *X1, char *X2, float *ofx, float *ofy)
{
  int32_t  A11=0, A12=0, A22=0, b1=0, b2=0;
  int16_t  F2F1, F4F3, FCF0;
        
  // set up pointers
  char *f0 = X1 + 12 + 1; // "12"
  char *f1 = X1 + 12 + 2; // "12"
  char *f2 = X1 + 12; // "12"
  char *f3 = X1 + 2*12 + 1; // "12"
  char *f4 = X1 + 1;
  char *fz = X2 + 12 + 1; // "12"

  // loop through
  for (char r=1; r!=12-1; ++r) { // "12"
    for (char c=1; c!=12-1; ++c) { // "12"
      // compute differentials, then increment pointers (post increment)
      F2F1 = (*(f2++) - *(f1++));
      F4F3 = (*(f4++) - *(f3++));
      FCF0 = (*(fz++) - *(f0++));

      // update summations
      A11 += (F2F1 * F2F1);
      A12 += (F4F3 * F2F1);
      A22 += (F4F3 * F4F3);
      b1  += (FCF0 * F2F1);                   
      b2  += (FCF0 * F4F3);                                   
    }
    f0+=2;
    fz+=2;
    f1+=2;
    f2+=2;
    f3+=2;
    f4+=2;
  }
       
  int64_t detA = ( (int64_t)(A11)*A22 - (int64_t)(A12)*A12 );
  int64_t XS = ( (int64_t)(b1)*A22 - (int64_t)(b2)*A12 ) * of_scale / detA;
  int64_t YS = ( (int64_t)(b2)*A11 - (int64_t)(b1)*A12 ) * of_scale / detA;
        
  // Note from Geof- The code below assigning OFX and OFY are from the
  // original implementations of the code. For some reason I cannot get
  // this algorithm to work unless I use the "float" assignment below.
  // My knowledge of casteing in C is limited, so it is probably a very
  // simple error. Anyway- apologizes for sticking with the float
  // assignement for now- it is wasteful of CPU cycles. I'll send a
  // free ArduEye to the first person who corrects this issue.
  //OFX=(signed char)(XS);
  //OFY=(signed char)(YS);
  *ofx = XS;
  *ofy = YS;
}

/*------------------------------------------------------------------------
CYE_OFO_ShortIIA_2D -- Exactly like CYE_OFO_CharIIA_2D except that input
images are shorts.
STATUS: UNTESTED
*/
void CYE_OFO_ShortIIA_2D(short *X1, short *X2, short numrows, short numcols, float *ofx, float *ofy)
{
  int32_t  A11=0, A12=0, A22=0, b1=0, b2=0;
  int16_t  F2F1, F4F3, FCF0;
        
  // set up pointers
  short *f0 = X1 + 8 + 1; // "12"
  short *f1 = X1 + 8 + 2; // "12"
  short *f2 = X1 + 8; // "12"
  short *f3 = X1 + 2*8 + 1; // "12"
  short *f4 = X1 + 1;
  short *fz = X2 + 8 + 1; // "12"

  // loop through
  for (char r=1; r!=8-1; ++r) { // "12"
    for (char c=1; c!=8-1; ++c) { // "12"
      // compute differentials, then increment pointers (post increment)
      F2F1 = (*(f2++) - *(f1++));
      F4F3 = (*(f4++) - *(f3++));
      FCF0 = (*(fz++) - *(f0++));

      // update summations
      A11 += (F2F1 * F2F1);
      A12 += (F4F3 * F2F1);
      A22 += (F4F3 * F4F3);
      b1  += (FCF0 * F2F1);                   
      b2  += (FCF0 * F4F3);                                   
    }
    f0+=2;
    fz+=2;
    f1+=2;
    f2+=2;
    f3+=2;
    f4+=2;
  }
       
  int64_t detA = ( (int64_t)(A11)*A22 - (int64_t)(A12)*A12 );
  int64_t XS = ( (int64_t)(b1)*A22 - (int64_t)(b2)*A12 ) * of_scale / detA;
  int64_t YS = ( (int64_t)(b2)*A11 - (int64_t)(b1)*A12 ) * of_scale / detA;
        
  // Note from Geof- The code below assigning OFX and OFY are from the
  // original implementations of the code. For some reason I cannot get
  // this algorithm to work unless I use the "float" assignment below.
  // My knowledge of casteing in C is limited, so it is probably a very
  // simple error. Anyway- apologizes for sticking with the float
  // assignement for now- it is wasteful of CPU cycles. I'll send a
  // free ArduEye to the first person who corrects this issue.
  //OFX=(signed char)(XS);
  //OFY=(signed char)(YS);
  *ofx = XS;
  *ofy = YS;
}