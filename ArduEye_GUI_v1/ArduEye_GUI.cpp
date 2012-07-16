/*********************************************************************/
/*********************************************************************/
//	ArduEye_GUI_v1.c
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

//supports older version of ARDUINO IDE
#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #else
  #include "WProgram.h"
  #endif

#include "ArduEye_GUI.h"

//class instance to be referenced in sketch
ArduEyeGUIClass ArduEyeGUI;

/*********************************************************************/
//	Constructor
//	Defaults to no GUI, no GUI commands will be sent
/*********************************************************************/

ArduEyeGUIClass::ArduEyeGUIClass(void)
{
  // initialize this instance's variables
  detected=0;	//arduGUI not detected
}

/*********************************************************************/
//	start
//	Enables the interface to start sending data to the GUI
/*********************************************************************/

void ArduEyeGUIClass::start(void)
{
  detected=1;	//GUI detected
}

/*********************************************************************/
//	stop
//	Disables the interface so that no data is sent to the GUI
/*********************************************************************/

void ArduEyeGUIClass::stop(void)
{
  detected=0;	//GUI not detected
}

/*********************************************************************/
//	getCommand
//	checks the serial port for incoming commands and parses them
//	into command and argument using the format "X#", where "X" is a //	single character and "#" is an optional number. The number "#" 
//	may be omitted, or may be one or more digits in size. The 
//	extracted command character and number argument are returned
// 	via pointers.  The special command !0 and !1, which enables and
//	disables the GUI, is intercepted here, but still passed through.
/*********************************************************************/

void ArduEyeGUIClass::getCommand(char *command, int *argument) 
{
  char cmdbuf[11];
  unsigned char i;

  // initialize
  for (i=0; i<11; ++i)
    cmdbuf[i] = 0;
  i = 0;
  // delay to ensure that all stuff is sent through serial port
  delay(100);
  // load cmdbuf
  while (Serial.available() && i<10) {
    cmdbuf[i] = Serial.read();
    i++;
  }
  // clear end of array
  cmdbuf[10]=0;
  // clear rest of buffer
  while (Serial.available())
    ;
  // get command
  *command = cmdbuf[0];
  // get argument
  sscanf(cmdbuf+1,"%d",argument);


// Turns on or off GUI mode. USE ONLY WHEN CONNECTED TO PROCESSING GUI
// Otherwise you'll get a lot of gobblygook on the serial monitor...
   if(*command=='!')
   {
      if(*argument==0) {
        stop();
        Serial.println("Arduino Out! GUI off");
      }
      if(*argument==1) {
        start();
        Serial.println("Arduino Here! GUI on");
      }
   }        
}

/*********************************************************************/
//	sendEscChar
//	write the sent-in byte preceded by the escape character.  
//	This is used for special characters
/*********************************************************************/

void ArduEyeGUIClass::sendEscChar(byte char_out)
{ 
  Serial.write(ESC);		//send escape char
  Serial.write(char_out);	//send special char
}

/*********************************************************************/
//	sendDataByte
//	writes the data byte to the serial port.  If the data byte is
//	the escape character, then it is duplicated to indicate that
//	it is a regular data byte.
/*********************************************************************/

void ArduEyeGUIClass::sendDataByte(byte data_out)
{
  if(data_out!=ESC)
	Serial.write(data_out);		//send data byte
  else
  {
	Serial.write(data_out);		//duplicate escape character
	Serial.write(data_out);
  }

}

/*********************************************************************/
//	sendImage (short version)
//	sends an image to the GUI for display
//
//	ARGUMENTS:
//	rows: number of rows in image
//	cols: number of cols in image
//	pixels: a 1D array of short pixel values in the image
//	size: number of pixels in image (rows*cols)
/*********************************************************************/

void ArduEyeGUIClass::sendImage(byte rows,byte cols,short *pixels,
					  short size)
{

  union	//to get the signed bytes to format properly, use a union
  {	
   short i_out;
   byte b[2];
  }u;
  
  if(detected)	//if GUI is detected, send bytes
  {
  	sendEscChar(START);	//send start packet
  
  	sendDataByte(IMAGE);		//write image header
  	sendDataByte(rows);		//write rows of image
  	sendDataByte(cols);		//write cols of image
  	
	//for some reason, Serial.write(byte_array,num)
	//doesn't work over a certain number of bytes
	//so send bytes one at a time
	for(int i=0;i<size;i++)	
	{
	  u.i_out=pixels[i];		//put two bytes into union
	  sendDataByte(u.b[0]);	//send first byte 
	  sendDataByte(u.b[1]);	//send second byte
 	}
  
  	sendEscChar(STOP);		//send stop packet
  }

}

/*********************************************************************/
//	sendImage (char version)
//	sends an image to the GUI for display
//
//	ARGUMENTS:
//	rows: number of rows in image
//	cols: number of cols in image
//	pixels: a 1D array of char pixel values in the image
//	size: number of pixels in image (rows*cols)
/*********************************************************************/

void ArduEyeGUIClass::sendImage(byte rows,byte cols,char *pixels,
					  short size)
{

  if(detected)	//if GUI is detected, send bytes
  {
  	sendEscChar(START);	//send start packet
  
  	sendDataByte(IMAGE_CHAR);		//write image header
  	sendDataByte(rows);		//write rows of image
  	sendDataByte(cols);		//write cols of image
  	
	//for some reason, Serial.write(byte_array,num)
	//doesn't work over a certain number of bytes
	//so send bytes one at a time
	for(int i=0;i<size;i++)	
	{
	  sendDataByte((byte)pixels[i]);	//send first byte 
 	}
  
  	sendEscChar(STOP);		//send stop packet
  }
}

/*********************************************************************/
//	sendVectors (short version)
//	sends an image to the GUI for display
//
//	ARGUMENTS:
//	rows: number of rows in vector display (NOT IMAGE)
//	cols: number of cols in vector display (NOT IMAGE)
//	vector: an array of vectors in [X1,Y1,X2,Y2,...] format
//	num_vectors: number of vectors (size of vector/2)
//
//	EXAMPLES:
//	short vector[4]={vx1,vy1,vx2,vy2}
//	sendVectors(1,2,vector,2)
//	displays a 1x2 array of two vectors in the display windows
//	which means vx will display on the left and vy on the right
/*********************************************************************/

void ArduEyeGUIClass::sendVectors(byte rows,byte cols,short *vector,short num_vectors)
{ 

  union
  {	//to get the signed bytes to format properly, use a union
   short i_out;
   byte b[2];
  }u;

  if(detected)	//if GUI is detected, send bytes
  {
  	sendEscChar(START);		//send start packet

  	sendDataByte(VECTORS_SHORT);		//send vector header
  	sendDataByte(rows);			//send rows of vectors
  	sendDataByte(cols);			//send cols of vectors

  	for(int i=0;i<num_vectors*2;i+=2)
	{
	  u.i_out=vector[i];		//put two bytes into union
	  sendDataByte(u.b[0]);	//send first byte 
	  sendDataByte(u.b[1]);	//send second byte
	  u.i_out=vector[i+1];		//put two bytes into union
	  sendDataByte(u.b[0]);	//send first byte 
	  sendDataByte(u.b[1]);	//send second byte
	}

  	sendEscChar(STOP);			//send stop packet
  }
}

/*********************************************************************/
//	sendVectors (char version)
//	sends an image to the GUI for display
//
//	ARGUMENTS:
//	rows: number of rows in vector display (NOT IMAGE)
//	cols: number of cols in vector display (NOT IMAGE)
//	vector: an array of vectors in [X1,Y1,X2,Y2,...] format
//	num_vectors: number of vectors (size of vector/2)
//
//	EXAMPLES:
//	char vector[4]={vx1,vy1,vx2,vy2}
//	sendVectors(1,2,vector,2)
//	displays a 1x2 array of two vectors in the display windows
//	which means vx will display on the left and vy on the right
/*********************************************************************/

void ArduEyeGUIClass::sendVectors(byte rows,byte cols,char 							    *vector,short num_vectors)
{ 

  if(detected)	//if GUI is detected, send bytes
  {
  	sendEscChar(START);		//send start packet

  	sendDataByte(VECTORS);		//send vector header
  	sendDataByte(rows);			//send rows of vectors
  	sendDataByte(cols);			//send cols of vectors

  	for(int i=0;i<num_vectors*2;i+=2)
	{
	  sendDataByte((byte)vector[i]);
	  sendDataByte((byte)vector[i+1]);
	}

  	sendEscChar(STOP);			//send stop packet
  }
}

/*********************************************************************/
//	sendPoints 
//	sends an array of points to highlight in the GUI display
//
//	ARGUMENTS:
//	rows: number of rows in image
//	cols: number of cols in image
//	points: an array of points in [r1,c1,r2,c2,...] format
//	num_vectors: number of points (size of points/2)
//
//	EXAMPLES:
//	char points[4]={2,4,10,11}
//	sendpoints(16,16,points,2)
//	on a 16x16 array, highlights the points (2,4) and (10,11)
/*********************************************************************/

void ArduEyeGUIClass::sendPoints(byte rows,byte cols,byte *points,short 				         num_points)
{ 
  if(detected)	//if GUI is detected, send bytes
  {
 	sendEscChar(START);		//send start packet
	
 	sendDataByte(POINTS);			//send points header
 	sendDataByte(rows);			//send rows of image
 	sendDataByte(cols);			//send cols of image
	
	for(int i=0;i<num_points*2;i+=2)
	{
	  sendDataByte(points[i]);		//send point row
	  sendDataByte(points[i+1]);		//send point col
	}
  
  	sendEscChar(STOP);			//send stop packet
  }

}