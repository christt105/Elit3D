#pragma once

#include "ExternalTools/MathGeoLib/include/Math/float3.h"

#ifdef _DEBUG
#define HANDLE_ERROR() oglh::_HandleError(__FUNCTION__)
#else
#define HANDLE_ERROR()
#endif

class oglh
{
	friend class Viewport;
	friend class m1Render3D;
public:
	enum Filter {
		Nearest,
		Linear
	};

	enum Wrap {
		ClampToEdge,
		ClampToBorder,
		Repeat,
		MirroredRepeat
	};

	//Textures
	/**
	 * Generates a Texture Buffer
	 * @param[out] id Buffer id container
	*/
	static void GenTexture(unsigned int& id);
	/**
	 * Deletes a Texture Buffer
	 * @param[out] id Buffer id container
	*/
	static void DeleteTexture(unsigned int& id);
	/**
	 * Active OpenGL texture
	 * @param val Index of the texture want to active
	*/
	static void ActiveTexture(int val);
	/**
	 * Bind Texture Buffer
	 * @param id Index of the texture buffer that want to bind
	*/
	static void BindTexture(unsigned int id);
	/**
	 * Generates a Texture Buffer and set the values
	 * 
	 * @param[out] id Buffer id container
	 * @param wrap Texture Wrap parameter.
	 * @param filter Texture Filter parameter.
	 * @param size_x Width of the image
	 * @param size_y height of the image
	 * @param data Pointer to the data. Array of unsigned bytes [R][G][B]
	*/
	static void GenTextureData(unsigned int& id, Wrap wrap, Filter filter, unsigned int size_x, unsigned int size_y, const unsigned char* data);
	/**
	 * Set a Texture parameters given a buffer
	 *
	 * @param id Buffer id container
	 * @param wrap Texture Wrap parameter.
	 * @param filter Texture Filter parameter.
	*/
	static void SetTextureProperties(unsigned int id, Wrap wrap, Filter filter);
	/**
	 * Redefines a contiguous subregion of an existing two-dimensional texture image.
	 *
	 * @param x X start region edit postition.
	 * @param y Y start region edit postition.
	 * @param width Width of the region.
	 * @param height Height of the region.
	 * @param pixels Pointer to the data that you want to redefine.
	*/
	static void TexSubImage2D(int x, int y, int width, int height, const unsigned char* pixels);
	/**
	 * Unbind binded texture buffer.
	*/
	static void UnBindTexture();
	//Objects
	/**
	 * Bind VAO, vertex and element buffers
	 *
	 * @param vao Id of the VAO buffer.
	 * @param vertex Id of the Vertez array buffer.
	 * @param elements Id of the element array buffer.
	*/
	static void BindBuffers(unsigned int vao, unsigned int vertex, unsigned int elements);
	/**
	 * Bind buffers
	*/
	static void BindBuffer(unsigned int id);
	/**
	 * Unbind VAO, vertex and elements buffers
	*/
	static void UnBindBuffers();
	/**
	 * Draw selected array buffer
	 *
	 * @param n Number of the elements to draw
	*/
	static void DrawArrays(int n); //TODO: Allow other geometry
	/**
	 * Draw elements
	 *
	 * @param size Number of the elements to draw
	*/
	static void DrawElements(int size); //TODO: Allow other geometry
	static void OldDrawLines(const float3& begin, const float3& end); //TODO: deprecate old direct mode
	/**
	 * Generates a Vertex Array Object
	 * @param[out] id Buffer id container
	*/
	static void GenVAO(unsigned int& id);
	/**
	 * Deletes a Vertex Array Object, Vertex Buffer and Element Buffer
	 * @param[out] vao VAO Buffer id container
	 * @param[out] vertex Vertex Buffer id container
	 * @param[out] elements Elements Buffer id container
	*/
	static void DeleteVAO(unsigned int& vao, unsigned int& vertex, unsigned int& elements);
	/**
	 * Generates a Vertex Array Buffer
	 * @param[out] id Buffer id container
	 * @param size Number of the elements. e.g. A buffer of a 3D triangle size will be 3 because it has 3 points.
	 * @param type_size Size of the element. typeof(type)
	 * @param element_size Size of the element. e.g. A buffer of a 3D triangle size will be 3 because it is composed of 3 floats(x, y, z).
	 * @param data Pointer to the data.
	*/
	static void GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data);
	/**
	 * Generates a Vertex Array Buffer
	 * @param[out] id Buffer id container
	 * @param size Number of the elements. e.g. A buffer of a 3D triangle size will be 3 because it has 3 points.
	 * @param type_size Size of the element. typeof(type)
	 * @param element_size Size of the element. e.g. A buffer of a 3D triangle size will be 3 because it is composed of 3 floats(x, y, z).
	 * @param data Pointer to the data.
	 * @param attrib_index Set Attribute Pointer id.
	 * @param attrib_size Attribute Pointer size.
	*/
	static void GenArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data, unsigned int attrib_index, unsigned int attrib_size);
	/**
	 * Generates a Vertex Array Buffer
	 * @param id Buffer id container
	 * @param size Number of the elements. e.g. A buffer of a 3D triangle size will be 3 because it has 3 points.
	 * @param type_size Size of the element. typeof(type)
	 * @param element_size Size of the element. e.g. A buffer of a 3D triangle size will be 3 because it is composed of 3 floats(x, y, z).
	 * @param data Pointer to the data.
	 * @param attrib_index Set Attribute Pointer id.
	 * @param attrib_size Attribute Pointer size.
	*/
	static void SetArrayBuffer(unsigned int& id, unsigned int size, unsigned int type_size, unsigned int element_size, const float* data, unsigned int attrib_index, unsigned int attrib_size);
	/**
	 * Generates a Vertex Array Buffer
	 * @param[out] id Buffer id container
	 * @param size Number of the elements.
	 * @param data Pointer to the data.
	*/
	static void GenElementBuffer(unsigned int& id, unsigned int size, const unsigned int* data);
	/**
	 * Deletes a Buffer
	 * @param[out] id Buffer id container
	*/
	static void DeleteBuffer(unsigned int& id);
	/**
	 * 
	*/
	static void BufferSubDataArray(unsigned int size, const void* data);
	//Misc
	/**
	 * Sets Polygon Mode
	*/
	static void PolygonMode(bool line);

	static void CheckFramebufferStatus();

	static void EnableCullFace(bool active);

	static void DepthEnable(bool active);
	static std::string GetVersion();
	static std::string GetVendor();
	static std::string GetModel();
private:
	static void _HandleError(const char* func);
	static int WrapEnumToGLEnum(Wrap w);
	static int FilterEnumToGLEnum(Filter w);
};
