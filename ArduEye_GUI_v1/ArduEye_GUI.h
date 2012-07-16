/*********************************************************************/
/*********************************************************************/
//	ArduEye_GUI_v1.h
//	ArduEyeGUI Library to interface with the ArduEye processing GUI
//	
//	Functions to send data to the ArduEye processing GUI for display
//	For example, send an image array to the GUI and have it display
//	the image in the GUI window. A special serial command "!1" is
//	sent from the GUI to enable all functions, so that a bunch of 
//	unreadable data isn't sent to the serial monitor.
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

#ifndef ArduEye_GUI_h
#define ArduEye_GUI_h

//supports older version of ARDUINO IDE
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

/*********************************************************************/
//Defines GUI comm handler special characters

#define ESC 27		//escape char
#define START 1		//start packet
#define STOP  2		//stop packet

/*********************************************************************/
//Defines GUI comm handler data sets

#define IMAGE  		2	//short image packet
#define POINTS 		4	//points packet
#define VECTORS 		6	//byte vectors packet
#define IMAGE_CHAR	8	//byte image packet
#define VECTORS_SHORT	10	//short vectors packet

/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
//	ArduEyeGUIClass
/*********************************************************************/
/*********************************************************************/

class ArduEyeGUIClass
{
  // user-accessible "public" interface
  public:

    // constructor has detected variable turned off
    // GUI functions must be enabled by command sent from
    // GUI on start-up.
    ArduEyeGUIClass(void);		//constructor

    // enables all functions, called in response to special
    // command from GUI
    void start(void);		//allow Arduino to send data

    // disables all functions so unreadable data isn't sent to 
    // the serial monitor
    void stop(void);		//don't allow Arduino to send data
 
    // sends the escape character plus the passed in byte, used
    // to send header information
    void sendEscChar(byte);	//send escape plus special char	

    // sends the given byte to the GUI, but if the byte is an
    // escape character, it sends it twice.
    void sendDataByte(byte);	//send data (repeats ESC char)

    // read available data from Serial port and parse into
    // command and argument.  Also intercepts special command
    // from GUI 
    void getCommand(char*,int*);
	
    // sends image for display in GUI.  First two arguments are
    // number of rows and columns.  Third argument is a short or
    // char 1D image array, and final argument is total size of
    // array			
    void sendImage(byte,byte,short*,short);
    void sendImage(byte,byte,char*,short);

    // sends a set of vectors to be displayed in the GUI on top of    
    // any image.  First two arguments are the number of rows and 
    // columns in the VECTOR display (not image).  Third argument
    // is an array of char or short pairs, each one a vector (X,Y)   
    // to be displayed, and final argument is number of vectors.	
    void sendVectors(byte,byte,char*,short);
    void sendVectors(byte,byte,short*,short);
					
    // sends an array of points to be highlighted on the image in
    // the GUI.  First two arguments are number of rows and columns
    // in the display image.  Third argument is a byte array with
    // pairs of (row,col) that should be highlighted, and final 
    // argument is number of pairs.
    void sendPoints(byte,byte,byte*,short);
				
    
  // library-accessible "private" interface
  private:
    int detected;	 //whether the GUI is detected
    
    
};

//class instance
extern ArduEyeGUIClass ArduEyeGUI;

#endif