

/* ArduGUI_Example

This is an Arduino sketch that demonstrates sending data packets to the
ArduGUI running on your computer. The hardware platform is a Centeye 
Stonyman chip on an SH breakout board, connected to an Arduino Uno or 
Duemilanove using an ArduEye Rocket board. It is assumed that the SH 
breakout board has a Stonyman chip on board, and a printed pinhole 
approximately in the middle of the chip's focal plane.

Setup involves two steps: First, hold a flashlight or LED high above the
chip, and use the "p" command to find the pinhole (by looking for brightest
pixel point). Second, cover sensor with a sheet of paper and use "c" 
command to calibrate.

The script takes an image from the Stonyman chip and displays it on the
ArduGUI screen, along with the maximum point as a points data packet.

This script requires the use of three libraries, ArduGUI, SMH1_StonymanHawksbill_v1,
and CYE_Images_v1.h, which should be placed in the Arduino libraries folder.

Revision February 16, 2012
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

//
#include <SMH1_StonymanHawksbill_v1.h>
#include <CYE_Images_v1.h>
#include <arduGUI.h>
#include <SPI.h>

//---------------------------------------------------------------
// GLOBAL VARIABLES

// WINDOW_SIZE = Size of a window in pixels (across, high)
#define WINDOWSIZE 16

// NUM_PIXELS = number of pixels in each window
#define NUMPIXELS (WINDOWSIZE*WINDOWSIZE)

// dumpType: determine what is dumped to serial terminal every frame
unsigned char dumpType = 0;

// Row and column of pinhole
unsigned char mPinhole,nPinhole; 

// Image Arrays: 
short A[NUMPIXELS]; // current image
unsigned char C[NUMPIXELS]; // calibration variable part
short Cbase=0;


// charbuf: character buffer for generating outputs
char charbuf[80];

// Command inputs - for receiving commands from user via Serial terminal
char command; // command character
int commandArgument; // argument of command


   
byte lights[10];
short num_lights=0;

char vectors[10];
short num_vectors=0;



arduGUI GUI;

//-----------------------------------------------------------------------
// GetUserCommand
// Users may send simple commands via the Arduino's serial monitor using
// the format "X#", where "X" is a single character and "#" is an optional
// number. The number "#" may be omitted, or may be one or more digits 
// in size. The extracted command character and number argument are returned
// via pointers.
void GetUserCommand(char *command, int *argument) {
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
  //Serial.print('1');Serial.println("message received");
  
  
}

//-----------------------------------------------------------------------
// dumpCommandList - Dumps a list of commands to the serial monitor to
// help the user out. The delay is used to avoid overflowing the serial
// port
void dumpCommandList() {
  Serial.println("a: MATLAB dump"); 
  Serial.println("c: calibrate FPN");
  Serial.println("h#: 1=use hyperacuity, 0=don't"); 
  Serial.println("i: dump stuff for this frame only"); 
  Serial.println("m#: set pinhole row to #"); 
  Serial.println("n#: set pinhole col to #"); 
  Serial.println("o#: set output level = #"); 
  Serial.println("p: find pinhole"); 
  Serial.println("s#: shape (convexity) threshold = #"); 
  Serial.println("t#: intensity threshold = #"); 
  Serial.println("u: debug dump");
  Serial.println("z: MATLAB dump entire chip");  
}



    
   
//=======================================================================
// ARDUINO SETUP AND LOOP FUNCTIONS

void setup() {
  char w;
  SMH1_SetupIO(); // Connect Arduino to Stonyman chip
  SMH1_ClearAllValsSlow(); // Clear Stonyman chip register values
  Serial.begin(115200); 
 
  // Initialize Stonyman chip register values including biases for 5V
  SMH1_SetPtrValSlow(SMH_SYS_VREF,50);
  SMH1_SetPtrValSlow(SMH_SYS_NBIAS,55);
  SMH1_SetPtrValSlow(SMH_SYS_AOBIAS,55);
  SMH1_SetPtrValSlow(SMH_SYS_CONFIG,16); 
  //dumpCommandList();
  pinMode(2,OUTPUT); // for debugging- digital outputs 2 and 3 may be pulsed etc to take time measurements
  pinMode(3,OUTPUT);
  
  // As an estimate, set initial pinhole location to 55,55
  mPinhole=55;nPinhole=55;
}

void loop() {

  short i,j,k,r,c,m,n;
  
  digitalWrite(3,HIGH);
  
  // PROCESS USER COMMANDS, IF ANY
  if (Serial.available()>0) { // Check Serial buffer for input from user
   
    GetUserCommand(&command,&commandArgument); // get user command and argument
    switch (command) {
     
      case 'c': // grab calibration mask
        // Note: this will have to performed if the chip is changed or the pinhole location is changed
       Serial.println("Calibrating...");
        SMH1_GetImageSlow(A,mPinhole-WINDOWSIZE/2,WINDOWSIZE,1,nPinhole-WINDOWSIZE/2,WINDOWSIZE,1,0,0);
        // find minimum
        Cbase = 10000; // e.g. "high"
        for (i=0; i<NUMPIXELS; ++i)
          if (A[i]<Cbase)
            Cbase = A[i];
        // generate calibration mask
        for (i=0; i<NUMPIXELS; ++i)
          C[i] = A[i] - Cbase;
        Serial.println("...done");
        break;
     
      case 'm': // set pinhole row
        mPinhole = commandArgument;
        sprintf(charbuf,"Pinhole row = %d",mPinhole);
        Serial.println(charbuf);     
        break;   
      case 'n': // set pinhole column
        nPinhole = commandArgument;
        sprintf(charbuf,"Pinhole column = %d",nPinhole);
        Serial.println(charbuf); 
        break;
      case 'o': // set output dump type
      if((commandArgument>=0)&&(commandArgument<=2))
      {
        dumpType = commandArgument;
        Serial.print("output dumptype = ");
         Serial.println((int)dumpType); 
      }
      else
      {Serial.println("not the right argument");}
        break;         
      case 'p': // find pinhole- hold flashlight or LED high above sensor before sending this command
         Serial.println("Finding Pinhole...");
        //getUserPing(NULL);
        SMH1_FindMaxSlow(30,50,30,50,0,&mPinhole,&nPinhole);
        sprintf(charbuf,"Pinhole at %d,%d",mPinhole,nPinhole);
        Serial.println(charbuf);
       // delay(2000);
        break;
      
      case '?':dumpCommandList();
      case '!':
      if(commandArgument==0) 
      {
        GUI.stop();
        Serial.println(">>Goodbye, Computer!");
      }
      if(commandArgument==1) 
      {
        GUI.start();
        Serial.println(">>I'm here, Computer!");
      }
      break;
      default:break;
        
    }
  }
  
  if (dumpType==1)
  {
    
    //SMH1_GetImageSlow(A,mPinhole-WINDOWSIZE/2,WINDOWSIZE,1,nPinhole-WINDOWSIZE/2,WINDOWSIZE,1,0,0); // about 44ms with 16x16 window
    SMH1_GetImageFast(A,mPinhole-WINDOWSIZE/2,WINDOWSIZE,1,nPinhole-WINDOWSIZE/2,WINDOWSIZE,1,0,0); // about 30ms with 16x16 window
  
    // Apply calibration mask
    for (i=0; i<NUMPIXELS; ++i) 
      A[i] -= C[i];
  
    lights[0]=1;
    lights[1]=1;
    lights[2]=4;
    lights[3]=4;
    num_lights=2;
  
    vectors[0]=-50;
    vectors[1]=-50;
    num_vectors=1;
  
    GUI.sendImage(WINDOWSIZE,WINDOWSIZE,A,NUMPIXELS);
    GUI.sendPoints(WINDOWSIZE,WINDOWSIZE,lights,num_lights);
    GUI.sendVectors(1,1,vectors,num_vectors);

  }
  
   
  
}



