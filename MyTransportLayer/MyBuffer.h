#ifndef _MYBUFFER_
#define _MYBUFFER_

class MyBuffer {

	unsigned length = 0;
	unsigned char* buffer = nullptr;

public:

	MyBuffer() { }
	~MyBuffer() { delete[] buffer; }

	void setBuffer(unsigned len) {
		if (!buffer) { 
			buffer = new unsigned char[len]();
			length = len;
		}
	}

	unsigned char* get() const { return buffer; }
	unsigned getLength() const { return length; }
};

#endif