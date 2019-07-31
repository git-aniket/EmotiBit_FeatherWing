#pragma once

#include "BufferFloat.h"

class DoubleBufferFloat {
private:
	BufferFloat* _buffer1;
	BufferFloat* _buffer2;
	BufferFloat* _inputBuffer;
	BufferFloat* _outputBuffer;
	bool _isPushing = false;
	bool _isGetting = false;
public:



	DoubleBufferFloat(size_t capacity = 64);
	//DoubleBufferFloat(const DoubleBufferFloat &doubleBuffer);
	//DoubleBufferFloat operator=(const DoubleBufferFloat &doubleBuffer);
	~DoubleBufferFloat();
	uint8_t push_back(float f, uint32_t * timestamp = nullptr);
	size_t getData(float ** data, uint32_t * timestamp = nullptr);
	
	/* readData() is used to read the input buffer without causing the buffers to clear and switch
	* This is safe to do in void loop() as long as there isn't a getData() call between readData() and the usage of the read only pointers
	* If getData() is called between readData() and the read only pointer usage, the pointer could be looking at the output buffer, or nullptr
	* it can be loaded as follows:
	* 	const float * data;
	* 	const uint32_t* timestamp;
	*   readData(&data,&timestamp);
	*/
	size_t readData(const float ** data, const uint32_t** timestamp);
	//void setAutoResize(bool b);
	size_t outSize();
	size_t outCapacity();
	void resize(size_t capacity);
};

