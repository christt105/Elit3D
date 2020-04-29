#pragma once
//#include "Transform.h"

template<typename T>
struct Buffer {
	unsigned int size = 0u;
	unsigned int id = 0u;
	T* data = nullptr;
};

class Object
{
	friend class m1Objects;
public:
	Object();
	~Object();

	void GenerateBuffers();
	void Draw();

private:
	Buffer<float> vertices;
	Buffer<unsigned int> indices;
	unsigned int VAO = 0u;

	//Transform transform;
};

