#pragma once
#include "Resources/Base/Resource.h"
#include "Objects/Components/c1Transform.h"
#include <ExternalTools/pugixml/pugixml.hpp>

class r1Tileset3d :
    public Resource
{
public:
    struct Tile3d {
        Tile3d(const uint64_t& uid, const std::string& str);
        c1Transform transform = c1Transform(nullptr);
        uint64_t uidObject = 0ULL;
        std::string name = "tile";
    };
public:
    r1Tileset3d(const uint64_t& uid);
    ~r1Tileset3d();

    void GenerateFiles() override;
    void Load() override;
    void LoadVars() override;

    void Parse(pugi::xml_node& node) const;
    void Parse(nlohmann::json& node) const;

    std::vector<Tile3d*> tiles;
};

