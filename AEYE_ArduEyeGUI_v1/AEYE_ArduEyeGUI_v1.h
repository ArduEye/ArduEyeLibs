/*
arduGUI.h

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

#ifndef AEYE_ArduEyeGUI_v1_h
#define AEYE_ArduEyeGUI_v1_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

// library interface description
class ArduEyeGUI
{
  // user-accessible "public" interface
  public:
    ArduEyeGUI(void);		//constructor
    void start(void);		//allow Arduino to send data
    void stop(void);		//don't allow Arduino to send data
    void sendImage(byte,byte,short*,short);
					//send image with (arg1) rows and
					//(arg2) cols as array of short ints
 					//(arg3) of length short (arg4)
    void sendPoints(byte,byte,byte*,short);
					//send points on an image of (arg1) rows
					//and (arg2) cols as a byte array (arg3)
					//with short pairs (arg4)
    void sendVectors(byte,byte,char*,short);
					//send vectors of (arg1) rows
					//and (arg2) cols as a char array (arg3)
					//with short pairs (arg4)

  // library-accessible "private" interface
  private:
    int detected;				//whether the GUI is detected
    void send_escape_char(char);	//send escape char plus special 						//char	
    
};

#endif