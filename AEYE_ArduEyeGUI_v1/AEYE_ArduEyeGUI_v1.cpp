/*
arduGUI.c

Basic functions to send data to arduGUI

Change Log:
February 17, 2012: AEYE_ArduEyeGUI_v1 created
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

#include "AEYE_ArduEyeGUI_v1.h"


#define ESC 27		//escape char
#define START 1		//start packet
#define STOP  2		//stop packet

#define IMAGE  	2	//image packet
#define POINTS 	4	//points packet
#define VECTORS 	6	//vectors packet


//Constructor
ArduEyeGUI::ArduEyeGUI(void)
{
  // initialize this instance's variables
  detected=0;	//arduGUI not detected
}

void ArduEyeGUI::start(void)
{
  detected=1;	//arduGUI detected
}

void ArduEyeGUI::stop(void)
{
  detected=0;	//arduGUI not detected
}

void ArduEyeGUI::send_escape_char(char char_out)
{ 
  Serial.write(ESC);		//send escape char
  Serial.write(char_out);	//send special char
}

void ArduEyeGUI::sendImage(byte rows,byte cols,short *pixels,short num_points)
{
  short i;
  
  if(detected)	//if GUI is detected, send bytes
  {
  	send_escape_char(START);	//send start packet
  
  	Serial.write(IMAGE);		//write image header
  	Serial.write(rows);		//write rows of image
  	Serial.write(cols);		//write cols of image
  	
	//for some reason, Serial.write(byte_array,num)
	//doesn't work over a certain number of bytes
	//so send bytes one at a time
	for(i=0;i<num_points;i++)	
	{
 	  Serial.write((byte)((pixels[i]>>8)&0xFF));	//upper 8 bits
	  Serial.write((byte)((pixels[i])&0xFF));		//lower 8 bits
	}
  
  	send_escape_char(STOP);		//send stop packet
  }

}

void ArduEyeGUI::sendPoints(byte rows,byte cols,byte *points,short num_points)
{ 
  if(detected)	//if GUI is detected, send bytes
  {
 	send_escape_char(START);		//send start packet
	
 	Serial.write(POINTS);			//send points header
 	Serial.write(rows);			//send rows of image
 	Serial.write(cols);			//send cols of image
 	Serial.write(points,num_points*2);	//write points arrau
  
  	send_escape_char(STOP);			//send stop packet
  }

}

void ArduEyeGUI::sendVectors(byte rows,byte cols,char *vector,short num_vectors)
{ 

  //NOTE: for now rows and cols aren't being used on the GUI
  //side, so this just supports one vector in the center of
  //the image

  if(detected)	//if GUI is detected, send bytes
  {
  	send_escape_char(START);		//send start packet

  	Serial.write(VECTORS);			//send vector header
  	Serial.write(rows);			//send rows of vectors
  	Serial.write(cols);			//send cols of vectors
  	Serial.write((byte*)vector,num_vectors*2);
  
  	send_escape_char(STOP);			//send stop packet
  }
}
