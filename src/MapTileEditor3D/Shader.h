#pragma once
class Shader
{
private:
	enum Type {
		PROGRAM, VERTEX, FRAGMENT
	};
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void Use();

protected:
	unsigned int id;

private:
	void CheckCompileErrors(unsigned int shader, Type type);
};

