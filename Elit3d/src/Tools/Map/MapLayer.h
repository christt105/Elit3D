#pragma once

#include <unordered_map>

#include "Tools/Math/int2.h"
#include "Tools/OpenGL/Buffer.h"
#include "Tools/Command.h"
#include "ExternalTools/MathGeoLib/include/Math/float3.h"

#include "Core/Globals.h"

#include "Properties.h"

class r1Mesh;
struct TypeVar;
class Object;
class aiNode;
class aiMesh;

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

class MapLayer {
    friend class r1Map;
    friend class m1MapEditor;
    friend class p1Layers;

    class EditLayerCommand : public Command { //TODO: Check if it is the best place to put this
    public:
        EditLayerCommand(MapLayer* layer);

        void Undo() override;
        void Redo() override;
    private:
        std::string data;
        MapLayer* lay = nullptr;
    };

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

        MAX
    };

    MapLayer(MapLayer::Type t, r1Map* m);
    virtual ~MapLayer() = default;

    virtual void Draw(const int2& size, int tile_width, int tile_height) const = 0;
    virtual void Reset(const int2& size) = 0;
    virtual void Resize(const int2& oldSize, const int2& newSize);
    

    static void SelectBuffers();
    static void DrawTile(const int2& size);
    static bool HeightOrder(const MapLayer* l1, const MapLayer* l2);

    void OnInspector();

    virtual void Parse(pugi::xml_node& node, MapLayer::DataTypeExport type, bool exporting) const;
    virtual void Parse(nlohmann::json& node, MapLayer::DataTypeExport type, bool exporting) const;
    virtual aiNode* Parse(std::vector<aiMesh*>& meshes) const { return nullptr; }
    
    virtual void Unparse(const pugi::xml_node& node);
    virtual void Unparse(const nlohmann::json& node);
    virtual void SetDataAfterUnparse() {};

    std::string SerializeData(MapLayer::DataTypeExport t) const;
    void        DeserializeData(const std::string& strdata, MapLayer::DataTypeExport t) const;

    const char* GetName() const;
    void SetName(const char* n);
    void SetSelected();

    Type GetType() const;
    static std::string TypeToString(Type t);
    std::string ToString() const;
    static MapLayer::Type StringToType(const std::string& s);

    static std::string DataTypeToString(DataTypeExport t);
    static MapLayer::DataTypeExport StringToDataType(const std::string& s);

    static OpenGLBuffers tile;

    Properties properties;

    std::string name = "Layer";
    char buf[30];
    bool visible = true;
    bool locked = false;
    float opacity = 1.f;
    int displacement[2] = { 0,0 };
    float height = 0.f;

    r1Map* map = nullptr;

    TILE_DATA_TYPE* data = nullptr;
    
    const Type type = Type::TILE;
};
