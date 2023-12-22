#include "Window.hpp"

#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "global.hpp"
#include "Pipeline.hpp"

Window::Window()
{
    // initialize glfw
    glfwInit();
    window=glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Canvas", nullptr, nullptr);
    if(window==nullptr){
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMaximizeWindow(window);
    glfwGetWindowSize(window, &this->width, &this->height);
    glfwMakeContextCurrent(window);

    // initialize glad
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glViewport(0, 0, width, height);

    // set callback
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(this->window, frameBufferSizeCallback);

    createGlShader(PROJECT_PATH "/shaders/main.vs", PROJECT_PATH "/shaders/main.fs");
}

Window::~Window()
{
    // delte components
    delete shader;
    delete rasterizer;
    delete model;
    delete camera;

    deleteGlShader();

    // destroy glfw window
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::setInitConfig()
{
    model=new Model(PROJECT_PATH "/assets/models/Nanosuit/Nanosuit.obj");
    // model=new Model(PROJECT_PATH "/assets/models/Diablo/diablo3_pose.obj");
    // model->addTextures(PROJECT_PATH "/assets/models/Diablo/diablo3_pose_diffuse.tga", TextureType::DIFFUSE);

    direct_t view_pos(960.f, 540.f, 3.0f);
    camera=new Camera(view_pos);
    rasterizer=new Rasterizer(width, height);
    shader=new Shader();

    Pipeline::bind(camera);
    Pipeline::bind(rasterizer);
    Pipeline::bind(shader);
    Pipeline::bind(model);
}

void Window::setRenderConfig()
{
    // set model matrix
    matrix_t mat=matrix_t::Identity();
    mat=Geometry::translate(mat, direct_t(960.f, 875.f, 0.f));
    mat=Geometry::scale(mat, direct_t(50.f, -50.f, 50.f));
    mat=Geometry::rotate(mat, glfwGetTime(), direct_t(0.f, 1.f, 0.f));
    shader->setModel(mat);

    // set view and projection matrix
    // shader->setView(camera->getView());
    // shader->setProjection(camera->getProjection());
}

void Window::run()
{
    initialize();
    setInitConfig();

    while(!glfwWindowShouldClose(window)){
        double start=glfwGetTime();

        Pipeline::clear({1.f, 1.f, 1.f});
        Pipeline::render();

        setRenderConfig();
        processInput();

        glUseProgram(window_shader);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, rasterizer->getFramebufferData());
        glBindVertexArray(this->vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();

        double end=glfwGetTime();
        std::cerr<<1.f/(end-start)<<"fps"<<std::endl;
    }

    release();
}

void Window::initialize()
{
    float vertices[]={
        -1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    };

    // set vao and vbo
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // set texture
    glGenTextures(1, &window_texture);
    glBindTexture(GL_TEXTURE_2D, window_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(window_shader);
    glUniform1i(glGetUniformLocation(window_shader, "texture0"), 0);
}

void Window::release()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Window::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Window::frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void Window::createGlShader(const char* vertex_path, const char* fragment_path)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertex_source;
    std::string fragment_source;
    std::ifstream vertex_file;
    std::ifstream fragment_file;
    vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try{
        vertex_file.open(vertex_path);
        fragment_file.open(fragment_path);
        std::stringstream vertex_stream, fragment_stream;
        vertex_stream<<vertex_file.rdbuf();
        fragment_stream<<fragment_file.rdbuf();
        vertex_file.close();
        fragment_file.close();
        vertex_source=vertex_stream.str();
        fragment_source=fragment_stream.str();
    }catch(std::ifstream::failure e){
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vertex_code=vertex_source.c_str();
    const char* fragment_code=fragment_source.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    vertex=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code, nullptr);    
    glCompileShader(vertex);
    checkGlShader(vertex, "VERTEX");

    fragment=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code, nullptr);
    glCompileShader(fragment);
    checkGlShader(fragment, "FRAGMENT");

    window_shader=glCreateProgram();
    glAttachShader(window_shader, vertex);
    glAttachShader(window_shader, fragment);
    glLinkProgram(window_shader);
    checkGlShader(window_shader, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Window::checkGlShader(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if(type != "PROGRAM"){
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog 
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }else{
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog 
                << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Window::deleteGlShader()
{
    glDeleteProgram(window_shader);
}
