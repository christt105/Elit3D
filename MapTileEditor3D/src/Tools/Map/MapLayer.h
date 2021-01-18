#pragma once

#include <unordered_map>

#include "Tools/Math/int2.h"
#include "Tools/OpenGL/Buffer.h"

#include "Core/Globals.h"

#include "Properties.h"

class r1Mesh;
struct TypeVar;
class Object;

class OpenGLBuffers {
public:
    OpenGLBuffers();
    ~OpenGLBuffers();

    void InitData();

    unsigned int VAO = 0u;

    Buffer<float> vertices;
    Buffer<unsigned int> indices;
    Buffer<float> texture;
};

class Layer {
    friend class r1Map;
    friend class m1MapEditor;
    friend class p1Layers;
    friend class p1Resources; //TODO HM
public:
    enum class DataTypeExport {
        NONE = -1,

        CSV,
        CSV_NO_NEWLINE,
        BASE64_NO_COMPRESSION,
        BASE64_ZLIB,

        MAX
    };

    enum class Type {
        NONE = -1,

        TILE,
        OBJECT,
        TERRAIN,

        MAX
    };

    Layer(Layer::Type t);
    ~Layer();

    float height = 0.f;

    unsigned int id_tex = 0u;

    void Draw(const int2& size, int tile_width, int tile_height) const;
    void Reset(const int2& size);
    void SelectTex() const;

    static void SelectBuffers();
    static void DrawTile(const int2& size);
    static bool HeightOrder(const Layer* l1, const Layer* l2);

    void OnInspector();

    std::string Parse(int sizeX, int sizeY, DataTypeExport d) const;
    nlohmann::json Parse(int sizeX, int sizeY) const;
    void Unparse(int sizeX, int sizeY, const std::string& data);

    const char* GetName() const;
    void SetName(const char* n);

    Type GetType() const;
    void SetType(Type t);
    static std::string TypeToString(Type t);
    static Layer::Type StringToType(const std::string& s);

    union {
        TILE_DATA_TYPE* tile_data = nullptr; //TODO: Research about set id with short or int
        Object* root;
    };
private:
    static OpenGLBuffers tile;


    Properties properties;

    std::string name = "Layer";
    char buf[30];
    bool visible = true;
    bool locked = false;
    float opacity = 1.f;
    int displacement[2] = { 0,0 };

    Type type = Type::TILE;
};
