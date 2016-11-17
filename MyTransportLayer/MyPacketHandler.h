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

		unsigned packets_count = buffer.getLength() / mtu;

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
				left = 0;
				data_lngth = left;
			}
			else {
				data = new unsigned char[mtu];
				memcpy(data, buffer.get() + indicator, mtu);
				indicator += mtu;
				left -= mtu;
				data_lngth = mtu;
			}

			packets[i].setIP_s(source_ip);
			packets[i].setIP_d(destination_ip);
			packets[i].setVrsn(IPv4_PROTOCOL);
			packets[i].setPrtcl(143);
			packets[i].setData(data, data_lngth);
			packets[i].setChcksm();
			i++;
		}

		output.open(output_file_name, std::ios::binary | std::ios::out);
		if (output.is_open()) {

			output.write( (char*)packets, i*(sizeof(MyPacket)-sizeof(unsigned char*))+buffer.getLength() );
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
		left =  data_lngth = buffer.getLength();

		tempdata = new unsigned char[data_lngth];

		data_idx = 0;
		total_headers = 0;

		while (left) {

			//read header
			MyPacket header;

			memcpy(&header, buffer.get() + data_lngth - left, TYPICAL_HEADER_SIZE);
			left -= header.getHeaderLength();
			total_headers++;
			//validate fields
			if (header.getIP_s() == source_ip && header.getIP_d() == destination_ip
				&& header.getPrtcl() == prtcl && header.getVrsn() == vrsn) {

				memcpy(tempdata + data_idx, buffer.get() + data_lngth - left, header.getDataLength());
				data_idx += header.getDataLength();

			}
			left -= header.getDataLength();

			//checksum
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