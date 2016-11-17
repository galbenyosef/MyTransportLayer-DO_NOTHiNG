#ifndef _MYPACKET_
#define _MYPACKET_

#include "myBitFields.h"

typedef MyBitFields::_3bitfield _3bits;
typedef MyBitFields::_4bitfield _4bits;
typedef MyBitFields::_8bitfield _8bits;
typedef MyBitFields::_13bitfield _13bits;
typedef MyBitFields::_16bitfield _16bits;
typedef MyBitFields::_32bitfield _32bits;

const unsigned TYPICAL_HEADER_SIZE = 20;
const unsigned TTL_VALUE = 254;
const unsigned DATA_BYTES = 256;
const unsigned IPv4_PROTOCOL = 4;

class MyPacket {

	/*
	IPv4DatagramFormat
	13-Key Fields
	_______________________________________________________
	|version|header length|type of service|datagram length|
	|_____________________________________________________|
	|    identifier   |      flags       | fragmentation  |
	|_____________________________________________________|
	|   Time-To-Live  |     protocol     |    checksum    |
	|_____________________________________________________|
	|					ip source						  |
	|_____________________________________________________|
	|					ip destination					  |
	|_____________________________________________________|
	|													  |
	|                      DATA							  |
	|_____________________________________________________|

	Header 20 Bytes
	Data 256 Bytes

	*/

	_4bits version;
	_4bits h_length;
	_8bits tos;
	_16bits length;

	_16bits identifier;
	_3bits flags;
	_13bits fragmentation;

	_8bits ttl;
	_8bits protocol;
	_16bits checksum;

	_32bits ip_src;

	_32bits ip_dst;

	unsigned char* data;

public:

	MyPacket() {

		h_length.value = sizeof(MyPacket) - sizeof(unsigned char*);
		ttl.value = TTL_VALUE;
		version.value = 0;
		length.value = identifier.value = checksum.value =
			fragmentation.value = tos.value = flags.value = 0;

	}

	~MyPacket() {
		delete[] data;
	}

	void setIP_s(unsigned ipsrc) {ip_src.value = ipsrc;}
	void setIP_d(unsigned ipdst) {ip_dst.value = ipdst;};
	void setVrsn(unsigned vrsn) { version.value = vrsn; };
	void setPrtcl(unsigned prtcl) { protocol.value = prtcl; };
	void setData(unsigned char* dta, unsigned dta_len) { data = dta,length.value = h_length.value + dta_len; };
	void setChcksm();

	unsigned getIP_s() const { return ip_src.value; };
	unsigned getIP_d() const { return ip_dst.value; };
	unsigned getVrsn() const { return version.value; };
	unsigned getPrtcl() const { return protocol.value; };
	unsigned getHeaderLength() const { return h_length.value; }
	unsigned getDataLength() const { return length.value; };
	unsigned char* getData() const { return data; };
	unsigned getChcksm() const { return checksum.value; }
};

#endif