#ifndef _MYBITFIELDS_
#define _MYBITFIELDS_

class MyBitFields {


public:
	struct _3bitfield {
		unsigned char value : 3;
	};

	struct _4bitfield {
		unsigned  value : 4;
	};

	struct _8bitfield {
		unsigned char value;
	};

	struct _13bitfield {
		unsigned short value : 13;
	};

	struct _16bitfield {
		unsigned short value;
	};

	struct _32bitfield {
		unsigned value;
	};

	static auto myByteSwap(unsigned bits, unsigned val) {
		switch (bits) {

		case 16:
			auto retval((val >> 8) | (val << 8));
			return retval;
		case 32:
			auto newval((val >> 24) & 0xff) | // move byte 3 to byte 0
				((val << 8) & 0xff0000) | // move byte 1 to byte 2
				((val >> 8) & 0xff00) | // move byte 2 to byte 1
				((val << 24) & 0xff000000); // byte 0 to byte 3
			return retval;
		}

	}


};

#endif