#ifndef _MYPCKTHNDLR_
#define _MYPCKTHNDLR_

#include "MyBuffer.h"
#include "MyPacket.h"

class MyPacketHandler {


public:

	MyPacketHandler() {

	}

	~MyPacketHandler() { }

	void fragmentize(std::string input_file_name, std::string output_file_name, unsigned source_ip, unsigned destination_ip, unsigned mtu) {

		MyPacket* packets;
		MyBuffer buffer;

		std::ifstream input(input_file_name, std::ios::binary | std::ios::in);
		std::ofstream output;

		if (input.is_open()) {

			input.seekg(0, input.end);
			unsigned buf_s = (unsigned)input.tellg();
			input.seekg(0, input.beg);


			buffer.setBuffer(buf_s);

			input.read((char*)buffer.get(), buf_s);
			input.close();
		}

		unsigned packets_count = (unsigned) ( (float) buffer.getLength() / mtu + 0.5 );

		packets = new MyPacket[packets_count];

		unsigned i = 0;

		unsigned indicator, left;

		indicator = 0;
		left = buffer.getLength();

		while (left) {

			unsigned char* data;
			unsigned data_lngth;

			if (left < mtu) {
				data = new unsigned char[left];
				memcpy(data, buffer.get() + indicator, left);
				data_lngth = left;
				left = 0;
			}
			else {
				data = new unsigned char[mtu];
				memcpy(data, buffer.get() + indicator, mtu);
				indicator += mtu;
				data_lngth = mtu;
				left -= mtu;
			}

			packets[i].setFields(IPv4, TYPICAL_HEADER_SIZE_AS_32_BIT_WORDS, DO_NOTHiNG, source_ip, destination_ip, data, data_lngth);
			//packets[i].setChcksm();
			std::cout << packets[i];
			i++;
		}

		output.open(output_file_name, std::ios::binary | std::ios::out);

		if (output.is_open()) {

			for (unsigned i = 0; i < packets_count; i++) {

				unsigned char* out_header = packets[i].toWrite();
				unsigned out_header_size = packets[i].getHeaderLength();
				unsigned char* out_data = packets[i].getData();
				unsigned out_data_length = packets[i].getLength() - out_header_size;

				output.write((char*)out_header, out_header_size);
				output.write((char*)out_data, out_data_length);

				delete[] out_header;
			}

			output.close();

		}
	}

	void defragmentize(std::string input_file_name, std::string output_file_name, unsigned source_ip, unsigned destination_ip, unsigned prtcl, unsigned vrsn) {

		MyBuffer buffer;

		std::ifstream input(input_file_name, std::ios::binary | std::ios::in);
		std::ofstream output;

		if (input.is_open()) {

			input.seekg(0, input.end);
			unsigned buf_s = (unsigned)input.tellg();
			input.seekg(0, input.beg);


			buffer.setBuffer(buf_s);

			input.read((char*)buffer.get(), buf_s);
			input.close();
		}

		unsigned char* data, *tempdata;
		unsigned data_lngth, data_idx, left, total_headers, fixed_size;
		left = data_lngth = buffer.getLength();

		tempdata = new unsigned char[data_lngth];

		data_idx = 0;
		total_headers = 0;

		while (left) {

			//read header
			unsigned theHeaderSize = (*(buffer.get() + data_lngth - left) & MyPacket::createMask(4)) * 32 / 8; //header size as 32bit Words
			unsigned char* theHeader = new unsigned char[theHeaderSize];
			memcpy(theHeader, buffer.get() + data_lngth - left, theHeaderSize);
			left -= theHeaderSize;
			total_headers++;
			//validate fields
			unsigned _vrsn = theHeader[0] >> 4 & MyPacket::createMask(4);
			unsigned _len_msb = theHeader[3];
			unsigned _len_lsb = theHeader[2];
			unsigned _length = (_len_msb & MyPacket::createMask(8)) << 8 | _len_lsb & MyPacket::createMask(8);
			unsigned _prtcl = theHeader[9];

			unsigned _chksm_msb = theHeader[11];
			unsigned _chksm_lsb = theHeader[10];
			unsigned _checksum = _chksm_msb & MyPacket::createMask(8) << 8 | _chksm_lsb & MyPacket::createMask(8);

			unsigned _ips_msb1 = theHeader[15];
			unsigned _ips_msb2 = theHeader[14];
			unsigned _ips_lsb2 = theHeader[13];
			unsigned _ips_lsb1 = theHeader[12];
			unsigned _ipsource = (((_ips_msb1 & MyPacket::createMask(8)) << 8 | _ips_msb2 & MyPacket::createMask(8)) << 8 |
									_ips_lsb2 & MyPacket::createMask(8)) << 8 | _ips_lsb1 & MyPacket::createMask(8);

			unsigned _ipd_msb1 = theHeader[19];
			unsigned _ipd_msb2 = theHeader[18];
			unsigned _ipd_lsb2 = theHeader[17];
			unsigned _ipd_lsb1 = theHeader[16];
			unsigned _ip_dst = (((_ipd_msb1 & MyPacket::createMask(8)) << 8 | _ipd_msb2 & MyPacket::createMask(8)) << 8 |
				_ipd_lsb2 & MyPacket::createMask(8)) << 8 | _ipd_lsb1 & MyPacket::createMask(8);

			std::cout << "version " << _vrsn << "h_size " << theHeaderSize << "length " << _length << 
				"prtcl " << _prtcl << "checksum " << _checksum << "ipsrc " << _ipsource << "ipdst " << _ip_dst << std::endl;
			if (_vrsn == vrsn && _prtcl == prtcl && _ipsource == source_ip && _ip_dst == destination_ip) {

				memcpy(tempdata + data_idx, buffer.get() + data_lngth - left, _length - theHeaderSize);
				data_idx += _length - theHeaderSize;

			}
			left -= _length - theHeaderSize;
		}

		fixed_size = data_lngth - total_headers;
		data = new unsigned char[fixed_size];
		memcpy(data, tempdata, fixed_size);
		
		delete[] tempdata;

		output.open(output_file_name, std::ios::binary | std::ios::out);
		if (output.is_open()) {

			output.write((char*)data, fixed_size);
			output.close();
		}
	}
};

#endif