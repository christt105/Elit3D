#pragma once

template<typename T>
struct Buffer {
	unsigned int size = 0u;
	unsigned int id = 0u;
	T* data = nullptr;
};
