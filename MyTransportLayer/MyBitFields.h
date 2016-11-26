#ifndef _MYBITFIELDS_
#define _MYBITFIELDS_

class MyBitFields {


public:
	struct _3bitfield {
		unsigned char value : 3;
	};

	struct _4bitfield {
		unsigned char value : 4;
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

	static unsigned createMask(unsigned a, unsigned b){

		unsigned r = 0;

		for (unsigned i = a; i <= b; i++)
			r |= 1 << i;

		return r;
	}

	static unsigned myByteSwap16(unsigned val) {

		unsigned retval((val >> 8) | (val << 8));
				return retval;
	}
	static unsigned myByteSwap32(unsigned val) {

		unsigned retval(((val >> 24) & 0xff) |  // move byte 3 to byte 0
			((val << 8) & 0xff0000) | // move byte 1 to byte 2
			((val >> 8) & 0xff00) |  // move byte 2 to byte 1
			((val << 24) & 0xff000000)); // byte 0 to byte 3
		return retval;
	}
};

#endif