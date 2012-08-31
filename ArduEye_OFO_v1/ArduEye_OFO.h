/*********************************************************************/
/*********************************************************************/
//	ArduEye_OFO_v1.h
//	ArduEyeOFO Library provides optical flow algorithms
//	
//	Functions to calculate optical flow and odometry
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
/* 
Change log:
July 10, 2012: initial release

*/

#ifndef ARDUEYE_OFO_H
#define ARDUEYE_OFO_H

//supports older version of ARDUINO IDE
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif


/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
//	ArduEyeOFOClass
/*********************************************************************/
/*********************************************************************/

class ArduEyeOFOClass
{
  // user-accessible "public" interface
  public:

	// Low Pass Filters an OF value with coefficient alpha
      void LPF(short *filtered_OF,short *new_OF,float alpha);

	// Optical Accumulation using thresholding
      short Accumulate(short *new_OF,short *acc_OF,short threshold);

	// A simplified version of Srinivasan's Image Interpolation
	// algorithm for a 1D image
	void IIA_1D(short *curr_img, short *last_img, char numpix, short 			scale, short *out);
	void IIA_1D(char *curr_img, char *last_img, char numpix, short 			scale, short *out);

	// A simplified version of Srinivasan's Image Interpolation
	// algorithm for a 2D image using the standard plus interpolation
	void IIA_Plus_2D(char *curr_img, char *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);
	void IIA_Plus_2D(short *curr_img, short *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);

	// A simplified version of Srinivasan's Image Interpolation 
	// algorithms for a 2D image using the more compact square 	
	// interpolation
	void IIA_Square_2D(char *curr_img, char *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);
	void IIA_Square_2D(short *curr_img, short *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);

	// Lucas Kanade algorithm for optical flow for a 2D image using
	// the standard plus interpolation
	void LK_Plus_2D(char *curr_img, char *last_img, short rows, short 				cols, short scale, short *ofx, short *ofy);
	void LK_Plus_2D(short *curr_img, short *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);

	// Lucas Kanade algorithm for optical flow for a 2D image
	// using the more compact square interpolation
	void LK_Square_2D(char *curr_img, char *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);
	void LK_Square_2D(short *curr_img, short *last_img, short rows, 				short cols, short scale,short *ofx,short *ofy);

};

//class instance
extern ArduEyeOFOClass ArduEyeOFO;

#endif