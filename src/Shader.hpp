#pragma once

#include "global.hpp"
#include "Model.hpp"

struct ShaderInfo{
    direct_t   view_pos;
    color_t    color;
    normal_t   normal;
    texcoord_t texcoord;

    vec3f_t ambient;
    vec3f_t diffuse;
    vec3f_t specular;

    std::vector<Texture*> textures;
};

class Shader{
private:
    Model    origin_model;
    Model    current_model;
    direct_t view_pos;
    matrix_t model_mat;
    matrix_t view_mat;
    matrix_t projection_mat;

public:
    Shader();

    void setViewPos(const direct_t& view_pos);
    void setModel(const matrix_t& model_mat);
    void setView(const matrix_t& view_mat);
    void setProjection(const matrix_t& projection_mat);

    void use();
    void flush();
    void transform();
    void render();

    static color_t phongShader(const ShaderInfo& shader);
    static color_t textureShader(const ShaderInfo& shader);

friend class Pipeline;
};
