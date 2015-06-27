/*
ETHER_28J60.cpp - Ethernet library
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

/******************************************************************************
 * Includes
 ******************************************************************************/

#include "EtherShieldWrapper.h"
#include "Ethernet28J60.h"
//#include "WProgram.h" // Arduino 0.23
#include "Arduino.h" // Arduino 1.0 -- 2011 12 15 # Ben Schueler

/******************************************************************************
 * Definitions
 ******************************************************************************/
#define BUFFER_SIZE 500
#define STR_BUFFER_SIZE 32
static uint8_t buf[BUFFER_SIZE+1];
static char strbuf[STR_BUFFER_SIZE+1];

/******************************************************************************
 * Constructors
 ******************************************************************************/

/******************************************************************************
 * User API
 ******************************************************************************/

void Ethernet28J60::setup(const uint8_t macAddress[6], const uint8_t ipAddress[4], uint16_t port)
{
	memcpy(_macAddress, macAddress, 6);
	memcpy(_ipAddress, ipAddress, 6);
	_port = port;
	_plen = 0;

	_esw=EtherShieldWrapper();

	_esw.ESW_enc28j60Init(macAddress);
	_esw.ESW_enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
	delay(10);
	_esw.ESW_enc28j60PhyWrite(PHLCON,0x880);
	delay(500);
	_esw.ESW_enc28j60PhyWrite(PHLCON,0x990);
	delay(500);
	_esw.ESW_enc28j60PhyWrite(PHLCON,0x880);
	delay(500);
	_esw.ESW_enc28j60PhyWrite(PHLCON,0x990);
	delay(500);
	_esw.ESW_enc28j60PhyWrite(PHLCON,0x476);
	delay(100);
	_esw.ESW_init_ip_arp_udp_tcp(_macAddress, _ipAddress, _port);
}


char* Ethernet28J60::serviceRequest()
{
	uint16_t dat_p;
	//	int8_t cmd;
	_plen = _esw.ESW_enc28j60PacketReceive(BUFFER_SIZE, buf);

	/*plen will ne unequal to zero if there is a valid packet (without crc error) */
	if(_plen!=0)
	{
		// arp is broadcast if unknown but a host may also verify the mac address by sending it to a unicast address.
		if (_esw.ESW_eth_type_is_arp_and_my_ip(buf, _plen))
		{
			_esw.ESW_make_arp_answer_from_request(buf);
			return 0;
		}
		// check if ip packets are for us:
		if (_esw.ESW_eth_type_is_ip_and_my_ip(buf, _plen) == 0)
		{
			return 0;
		}
		if ((buf[IP_PROTO_P]==IP_PROTO_ICMP_V) && (buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V))
		{
			_esw.ESW_make_echo_reply_from_request(buf, _plen);
			return 0;
		}
		// tcp port www start, compare only the lower byte
		if ((buf[IP_PROTO_P]==IP_PROTO_TCP_V) && (buf[TCP_DST_PORT_H_P]==0) && (buf[TCP_DST_PORT_L_P] == _port))
		{
			if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
			{
				_esw.ESW_make_tcp_synack_from_syn(buf); // make_tcp_synack_from_syn does already send the syn,ack
				return 0;
			}
			if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
			{
				_esw.ESW_init_len_info(buf); // init some data structures
				dat_p=_esw.ESW_get_tcp_data_pointer();
				if (dat_p==0)
				{ // we can possibly have no data, just ack:
					if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
					{
						_esw.ESW_make_tcp_ack_from_any(buf);
					}
					return 0;
				}
				if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0)
				{
					// head, post and other methods for possible status codes see:
					// http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
					_plen=_esw.ESW_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
					_plen=_esw.ESW_fill_tcp_data_p(buf,_plen,PSTR("<h1>A</h1>"));
					respond();
				}
				if (strncmp("/",(char *)&(buf[dat_p+4]),1)==0) // was "/ " and 2
				{
					// Copy the request action before we overwrite it with the response
					int i = 0;
					while ((buf[dat_p+5+i] != ' ') && (i < STR_BUFFER_SIZE))
					{
						strbuf[i] = buf[dat_p+5+i];
						i++;
					}
					strbuf[i] = '\0';
					_plen=_esw.ESW_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
					return (char*)strbuf;
				}
			}
		}
	}
	return 0;
}


int Ethernet28J60::print(const char* text)
{
	int j = 0;
	while (text[j])
	{
		buf[TCP_CHECKSUM_L_P+3+_plen]=text[j++];
		_plen++;
	}
	return j;
}

int Ethernet28J60::print(const String text)
{
	int j = 0;
	while (text[j])
	{
		buf[TCP_CHECKSUM_L_P+3+_plen]=text[j++];
		_plen++;
	}
	return j;
}

int Ethernet28J60::print(int number)
{
	char tempString[9];
	itoa(number, tempString, 10);
	return print(tempString);
}

int Ethernet28J60::print(double number, uint8_t digits)
{
	//Based on Serial class, adapted by David Alain

	int n = 0;

	if (isnan(number)) return print("nan");
	if (isinf(number)) return print("inf");
	if (number > 4294967040.0) return print ("ovf");  // constant determined empirically
	if (number <-4294967040.0) return print ("ovf");  // constant determined empirically

	// Handle negative numbers
	if (number < 0.0)
	{
		n += print('-');
		number = -number;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	double rounding = 0.5;
	for (uint8_t i=0; i<digits; ++i)
		rounding /= 10.0;

	number += rounding;

	// Extract the integer part of the number and print it
	unsigned long int_part = (unsigned long)number;
	double remainder = number - (double)int_part;
	n += print(int_part);

	// Print the decimal point, but only if there are digits beyond
	if (digits > 0) {
		n += print(".");
	}

	// Extract digits from the remainder one at a time
	while (digits-- > 0)
	{
		remainder *= 10.0;
		int toPrint = int(remainder);
		n += print(toPrint);
		remainder -= toPrint;
	}

	return n;
}

void Ethernet28J60::respond()
{
	_esw.ESW_make_tcp_ack_from_any(buf); // send ack for http get
	_esw.ESW_make_tcp_ack_with_data(buf,_plen); // send data
}
