#ifndef SHIFT_REG_H
#define SHIFT_REG_H

#include <vector>

template<typename T, int N>
class ShiftReg
{
	T buffer[N];
	int idx = 0;
public:
	void shift(T val) {
		idx--;
		if (idx < 0) idx += N;
		buffer[idx] = val;
	};

	T read(int i) {
		i += idx;
		if (i >= N) i -= N;
		return buffer[i];
	};
};

template<typename T>
class ShiftRegB
{
	size_t N;
	std::vector<T> buffer;
	int idx = 0;
public:
	ShiftRegB(size_t N) : N(N), buffer(N) {}

	void shift(T val) {
		idx--;
		if (idx < 0) idx += N;
		buffer[idx] = val;
	};

	T read(int i) {
		i += idx;
		if (i >= N) i -= N;
		return buffer[i];
	};
};

#endif