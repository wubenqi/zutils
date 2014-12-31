// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//

#ifndef PCAP_UTIL_NET_HEADERS_H_
#define PCAP_UTIL_NET_HEADERS_H_

#ifdef WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#endif

//内存对齐,使内存紧凑
#pragma pack(1)

/* 4 bytes IP address */
typedef struct IPAddress {
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
} IPAddress;

// Set the packing to a 1 byte boundary
//Ethernet Header
typedef struct EthernetHeader {
	u_char dest[6];
	u_char source[6];
	u_short type;
} EthernetHeader;


//Ip header (v4)
typedef struct IPHeader {
    u_char ip_header_len:4; // 4-bit header length (in 32-bit words) normally=5 (Means 20 Bytes may be 24 also)
    u_char ip_version :4; // 4-bit IPv4 version
    u_char ip_tos; // IP type of service
    u_short ip_total_length; // Total length
    u_short ip_id; // Unique identifier
    u_char ip_frag_offset :5; // Fragment offset field
    u_char ip_more_fragment :1;
    u_char ip_dont_fragment :1;
    u_char ip_reserved_zero :1;
    u_char ip_frag_offset1; //fragment offset
    u_char ip_ttl; // Time to live
    u_char ip_protocol; // Protocol(TCP,UDP etc)
    u_short ip_checksum; // IP checksum
    IPAddress ip_srcaddr; // Source address
    IPAddress ip_destaddr; // Source address
} IPHeader;

//UDP header
typedef struct UDPHeader{
	u_short source_port; // Source port no.
	u_short dest_port; // Dest. port no.
	u_short udp_length; // Udp packet length
	u_short udp_checksum; // Udp checksum (optional)
} UDPHeader;

// TCP header
typedef struct TCPHeader {
    u_short source_port; // source port
    u_short dest_port; // destination port
	u_int sequence; // sequence number - 32 bits
	u_int acknowledge; // acknowledgement number - 32 bits
	
	u_char ns :1; //Nonce Sum Flag Added in RFC 3540.
	u_char reserved_part1:3; //according to rfc
	u_char data_offset:4; /*The number of 32-bit words in the TCP header.
									This indicates where the data begins.
									The length of the TCP header is always a multiple
									of 32 bits.*/
	
	u_char fin :1; //Finish Flag
	u_char syn :1; //Synchronise Flag
	u_char rst :1; //Reset Flag
	u_char psh :1; //Push Flag
	u_char ack :1; //Acknowledgement Flag
	u_char urg :1; //Urgent Flag
	u_char ecn :1; //ECN-Echo Flag
	u_char cwr :1; //Congestion Window Reduced Flag

	////////////////////////////////
	u_short window; // window
	u_short checksum; // checksum
	u_short urgent_pointer; // urgent pointer
} TCPHeader;
 
typedef struct ICMPHeader {
    u_char type; // ICMP Error type
    u_char code; // Type sub code
    u_short checksum;
    u_short id;
    u_short seq;
} ICMPHeader;

//内存对齐,使内存紧凑
#pragma pack()

#endif // NET_PCAP_UTIL_NET_HEADERS_H_

