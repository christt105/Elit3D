#pragma once

#include <unordered_map>

#include "Tools/Math/int2.h"
#include "Tools/OpenGL/Buffer.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"

#include "Core/Globals.h"

#include "Properties.h"

class r1Mesh;
struct TypeVar;
class Object;

class OpenGLBuffers {
public:
    OpenGLBuffers() = default;
    ~OpenGLBuffers();

    void InitData();

    unsigned int VAO = 0u;

    Buffer<float> vertices;
    Buffer<unsigned int> indices;
    Buffer<float> texture;
};

class MapLayer { //TODO: Inheritance 
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

    MapLayer(MapLayer::Type t);
    virtual ~MapLayer() = default;

    virtual void Draw(const int2& size, int tile_width, int tile_height) const = 0;
    virtual void Reset(const int2& size) = 0;
    virtual void Resize(const int2& oldSize, const int2& newSize) {}
    

    static void SelectBuffers();
    static void DrawTile(const int2& size);
    static bool HeightOrder(const MapLayer* l1, const MapLayer* l2);

    void OnInspector();

    virtual std::string Parse(int sizeX, int sizeY, DataTypeExport d) const = 0;
    virtual nlohmann::json Parse(int sizeX, int sizeY) const = 0;
    virtual void Unparse(int sizeX, int sizeY, const std::string& data) = 0;

    virtual nlohmann::json Serialize(const int2& size) const;
    virtual void           Deserialize(const nlohmann::json& json, const int2& size);

    const char* GetName() const;
    void SetName(const char* n);

    Type GetType() const;
    static std::string TypeToString(Type t);
    std::string ToString() const;
    static MapLayer::Type StringToType(const std::string& s);

    static OpenGLBuffers tile;

    Properties properties;

    std::string name = "Layer";
    char buf[30];
    bool visible = true;
    bool locked = false;
    float opacity = 1.f;
    int displacement[2] = { 0,0 };
    float height = 0.f;

    const Type type = Type::TILE;
};
