#ifndef _MYPACKET_
#define _MYPACKET_
/*
#include "myBitFields.h"
typedef MyBitFields::_3bitfield _3bits;
typedef MyBitFields::_4bitfield _4bits;
typedef MyBitFields::_8bitfield _8bits;
typedef MyBitFields::_13bitfield _13bits;
typedef MyBitFields::_16bitfield _16bits;
typedef MyBitFields::_32bitfield _32bits;
*/

const unsigned DATA_BYTES = 700; 
const unsigned IPv4 = 4;
const unsigned DO_NOTHiNG = 143;
const unsigned TTL_VALUE = 254;
const unsigned TYPICAL_HEADER_SIZE = 20;
const unsigned TYPICAL_HEADER_SIZE_AS_32_BIT_WORDS = 5; //Internet Header Length is the number of 32 - bit words in the header

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

	*/
	unsigned version; //4bit
	unsigned h_length; //4bit
	unsigned tos; //8bit
	unsigned length; //16bit
	unsigned identifier; //16bit
	unsigned flags; //3bit
	unsigned fragmentation; //13bit
	unsigned ttl; //8bit
	unsigned protocol; //8bit
	unsigned checksum; //16bit
	unsigned ip_src; //32bit
	unsigned ip_dst; //32bit
	unsigned char* data; // <= 256Bytes

public:

	MyPacket() {

		//h_length = TYPICAL_HEADER_SIZE_AS_32_BIT_WORDS;
		//ttl = TTL_VALUE;

		h_length = ttl = version = length = identifier =
			checksum = fragmentation = tos = flags = 0;

	}

	~MyPacket() {

		delete[] data;

	}

	void setFields(unsigned vrsn,unsigned h_lngt,unsigned prtcl,unsigned ipsrc,unsigned ipdst, unsigned char* dta, unsigned dta_lngt) {

		version = vrsn;
		h_length = h_lngt;
		protocol = prtcl;
		ip_src = ipsrc;
		ip_dst = ipdst;
		data = dta;
		length = h_length * 32 / 8 + dta_lngt;

	}

	static unsigned createMask(unsigned a) {

		unsigned r = 0;

		r = (1 << a) - 1;

		return r;
	}
	//void setChcksm() { checksum = getIP_s() + getIP_d() + getVrsn() + getPrtcl() + getHeaderLength() + getLength(); }

	unsigned char* toWrite() { //Any != 8 bits fields (1byte) has to be manipulated

		unsigned get3Bits = createMask(3);
		unsigned get4Bits = createMask(4);
		unsigned get5Bits = createMask(5);
		unsigned get8Bits = createMask(8);
		unsigned get13Bits = createMask(13);

		unsigned char _vrsn_N_hlen = ((version & get4Bits) << 4 | h_length & get4Bits);
		unsigned char _tos = tos & get8Bits;

		unsigned char _len_lsb = length & get8Bits;
		unsigned char _len_msb = (length >> 8) & get8Bits;

		unsigned char _identifier_lsb = identifier & get8Bits;
		unsigned char _identifier_msb = (identifier >> 8) & get8Bits;

		unsigned char _ttl = ttl & get8Bits;

		unsigned char _protocol = protocol & get8Bits;

		unsigned char _ips_lsb1 = ip_src & get8Bits;
		unsigned char _ips_lsb2 = (ip_src >> 8) & get8Bits;
		unsigned char _ips_msb2 = (ip_src >> 16) & get8Bits;
		unsigned char _ips_msb1 = (ip_src >> 24) & get8Bits;

		unsigned char _ipd_lsb1 = ip_dst & get8Bits;
		unsigned char _ipd_lsb2 = (ip_dst >> 8) & get8Bits;
		unsigned char _ipd_msb2 = (ip_dst >> 16) & get8Bits;
		unsigned char _ipd_msb1 = (ip_dst >> 24) & get8Bits;

		//unsigned short _flags_n_frags = (fragmentation * get13Bits) << 3 | flags * get3Bits;

		unsigned char _flags = flags * get3Bits;

		unsigned short _frags = fragmentation * get13Bits;

		unsigned char _flags_N_frags[2] = { (unsigned char)(_flags << 5 | (_frags >> 8) * get5Bits) , (unsigned char)(_frags & get8Bits) };

		unsigned char* retVal = new unsigned char[getHeaderLength()];

		retVal[0] = _vrsn_N_hlen;
		retVal[1] = _tos;
		retVal[2] = _len_lsb;
		retVal[3] = _len_msb;
		retVal[4] = _identifier_lsb;
		retVal[5] = _identifier_msb;
		retVal[6] = _flags_N_frags[0];
		retVal[7] = _flags_N_frags[1];
		retVal[8] = _ttl;
		retVal[9] = _protocol;

		//retVal[10] = _chksm_lsb;
		//retVal[11] = _chksm_msb;

		retVal[12] = _ips_lsb1;
		retVal[13] = _ips_lsb2;
		retVal[14] = _ips_msb2;
		retVal[15] = _ips_msb1;

		retVal[16] = _ipd_lsb1;
		retVal[17] = _ipd_lsb2;
		retVal[18] = _ipd_msb2;
		retVal[19] = _ipd_msb1;

		//unsigned TheChecksum = 

		return retVal;

	}



	unsigned getHeaderLength() const { return h_length * 32 / 8; }
	unsigned getLength() const { return length; }
	unsigned char* getData() const { return data; }
	unsigned getChcksm() const { return checksum; }



	friend std::ostream& operator<<(std::ostream&, const MyPacket&);
};

std::ostream& operator<<(std::ostream& out, const MyPacket& p)
{
	out << "(vrsn " << p.version << " ,prtcl " << p.protocol
			<< " ,h_lng " << p.h_length << " ,ips " << p.ip_src
				<< " ,ipd " << p.ip_dst << " ,length " << p.length
					<< " ,crc " << p.checksum << ")\n";
	return out;
}


#endif