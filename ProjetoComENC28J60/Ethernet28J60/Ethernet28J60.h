/*
  Ethernet28J60.h - Ethernet library
  Copyright (c) 2010 Simon Monk.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Ethernet28J60_h
#define Ethernet28J60_h

#include <inttypes.h>
#include <WString.h>
#include "EtherShieldWrapper.h"

class Ethernet28J60
{
public:
	void setup(const uint8_t macAddress[6], const uint8_t ipAddress[4], uint16_t port);
	char* serviceRequest();		// returns a char* containing the requestString
	//        or NULL if no request to service
	int print(const char* text); 	// append the text to the response
	int print(const String text);  	// append the text to the response
	int print(int value);  	// append the number to the response
	int print(double number, uint8_t digits);  	// append the number to the response
	void respond(); 		// write the final response
private:
	uint8_t _macAddress[6];
	uint8_t _ipAddress[4];
	uint16_t _port;

	EtherShieldWrapper _esw;
	uint16_t _plen;

};

#endif /* Ethernet28J60_h */

