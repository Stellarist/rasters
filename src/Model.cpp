#define TINYOBJLOADER_IMPLEMENTATION

#include "Model.hpp"

#include <cstddef>
#include <iostream>

Model::Model(const std::string& filepath)
{
    readModel(filepath);
    readTextures(filepath);
}

void Model::readModel(const std::string& filepath)
{
    // get file directory and name
    size_t file_pos=filepath.find_last_of('/');
    std::string file_dir=filepath.substr(0,file_pos+1);
    std::string file_name=filepath.substr(file_pos+1);

    // load obj file
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.triangulate=true;
    reader_config.mtl_search_path=file_dir;

    if(!reader.ParseFromFile(file_dir+file_name, reader_config)){
        if(!reader.Error().empty()){
            std::cerr<<"TinyObjReader1: "<<reader.Error()<<std::endl;
        }
        exit(1);
    }
    if(!reader.Warning().empty()){
        std::cerr<<"TinyObjReader2: "<<reader.Error()<<std::endl;
    }

    // read properties
    attrib=reader.GetAttrib();
    shapes=reader.GetShapes();
    materials=reader.GetMaterials();
}

void Model::readTextures(const std::string& filepath)
{
    size_t file_pos=filepath.find_last_of('/');
    std::string file_dir=filepath.substr(0,file_pos+1);

    for(auto& material: materials){
        if(!material.diffuse_texname.empty())
            textures[material.diffuse_texname]=new Texture(file_dir+material.diffuse_texname, TextureType::DIFFUSE);

        if(!material.specular_texname.empty())
            textures[material.specular_texname]=new Texture(file_dir+material.specular_texname, TextureType::SPECULAR);

        if(!material.bump_texname.empty())
            textures[material.bump_texname]=new Texture(file_dir+material.bump_texname, TextureType::BUMP);
    }
}

void Model::setTextures(const std::map<std::string, Texture*>& textures)
{
    this->textures=textures;
}

void Model::addTextures(const std::string& filepath, TextureType type)
{
    size_t file_pos=filepath.find_last_of('/');
    std::string file_name=filepath.substr(file_pos+1);
    textures[file_name]=new Texture(filepath, type);
}
