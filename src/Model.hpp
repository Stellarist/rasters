#pragma once

#include "tiny_obj_loader.h"
#include "Texture.hpp"

class Model{
private:
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::map<std::string, Texture*>  textures;

    void readModel(const std::string& filepath);
    void readTextures(const std::string& filepath);
    
public:
    Model()=default;
    Model(const std::string& filepath);

    void setTextures(const std::map<std::string, Texture*>& textures);
    void addTextures(const std::string& filepath, TextureType type);

friend class Shader;
};
