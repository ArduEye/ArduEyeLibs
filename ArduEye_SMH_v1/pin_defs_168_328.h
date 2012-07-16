/*********************************************************************/
/*********************************************************************/
//	pin_defs_168_328.h
//	ArduEye Library for the ATMega 168/328
//	
//	Defines pin interface between ATMega 168/328 and 
//	Stonyman/Hawksbill vision chips
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

#ifndef _PIN_DEFS_168_328_H_INCLUDED
#define _PIN_DEFS_168_328_H_INCLUDED

//RESP pin
#define SMH_RESP_PORT PORTB
#define SMH_RESP_DDR DDRB
#define SMH_RESP_PIN 0x01

//INCP pin
#define SMH_INCP_PORT PORTD
#define SMH_INCP_DDR DDRD
#define SMH_INCP_PIN 0x80
	
//RESV pin
#define SMH_RESV_PORT PORTD
#define SMH_RESV_DDR DDRD
#define SMH_RESV_PIN 0x40

//INCV pin
#define SMH_INCV_PORT PORTD
#define SMH_INCV_DDR DDRD
#define SMH_INCV_PIN 0x20

//INPHI pin
#define SMH_INPHI_PORT PORTD
#define SMH_INPHI_DDR DDRD
#define SMH_INPHI_PIN 0x10

//ANALOG OUT pins (4 supported)
#define SMH_ANALOG_PORT PORTC
#define SMH_ANALOG_DDR	DDRC
#define SMH_ANALOG0 0x01
#define SMH_ANALOG1 0x02
#define SMH_ANALOG2 0x04
#define SMH_ANALOG3 0x08

//EXTERNAL ADC SS pin
#define SMH_ADC_SS_PORT PORTB
#define SMH_ADC_SS_DDR	 DDRB
#define SMH_ADC_SS_PIN  0x04

#endif
