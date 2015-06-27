/*
  EHTERSHIELD_H library for Arduino etherShield
  Copyright (c) 2008 Xing Yu.  All right reserved.

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

#ifndef ETHERSHIELD_H
#define ETHERSHIELD_H

#include <WString.h>
#include <inttypes.h>
#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"


class EtherShieldWrapper
{
public:

	EtherShieldWrapper();
	uint16_t ESW_fill_tcp_data_p(uint8_t *buf,uint16_t pos, const prog_char * progmem_s);
	uint16_t ESW_fill_tcp_data(uint8_t *buf,uint16_t pos, const char *s);
	void ESW_enc28j60Init(const uint8_t* macaddr);
	void ESW_enc28j60clkout(uint8_t clk);
	void ESW_enc28j60PhyWrite(uint8_t address, uint16_t data);
	uint16_t ESW_enc28j60PacketReceive(uint16_t len, uint8_t* packet);
	void ESW_init_ip_arp_udp_tcp(uint8_t *mymac,uint8_t *myip,uint8_t wwwp);
	uint8_t ESW_eth_type_is_arp_and_my_ip(uint8_t *buf,uint16_t len);
	void ESW_make_arp_answer_from_request(uint8_t *buf);
	uint8_t ESW_eth_type_is_ip_and_my_ip(uint8_t *buf,uint16_t len);
	void ESW_make_echo_reply_from_request(uint8_t *buf,uint16_t len);
	void ESW_make_tcp_synack_from_syn(uint8_t *buf);
	void ESW_init_len_info(uint8_t *buf);
	uint16_t ESW_get_tcp_data_pointer(void);
	void ESW_make_tcp_ack_from_any(uint8_t *buf);
	void ESW_make_tcp_ack_with_data(uint8_t *buf,uint16_t dlen);

	// new web client functions
	void ESW_make_arp_request(uint8_t *buf, uint8_t *server_ip);
	uint8_t ESW_arp_packet_is_myreply_arp ( uint8_t *buf );
	void ESW_tcp_client_send_packet(uint8_t *buf,uint16_t dest_port, uint16_t src_port, uint8_t flags, uint8_t max_segment_size,
			uint8_t clear_seqck, uint16_t next_ack_num, uint16_t dlength, uint8_t *dest_mac, uint8_t *dest_ip);
	uint16_t ESW_tcp_get_dlength( uint8_t *buf );

};	

#endif

