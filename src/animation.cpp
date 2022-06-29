#include <stdio.h>
#include <stdint.h>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "imgui/imgui.h"
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "glad/glad.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

typedef int32_t i32;
typedef uint32_t u32;
typedef uint8_t  u8;
typedef float f32;

using namespace std;
using namespace glm;

// Comment is blue? Awesome!
// Though needs to change the tag color like
// TAGS: IMPORTANT, NOTE, URGENT, TECH, REFERENCE
// @Important 

// Util Func Declar
std::vector<std::string> split(std::string s, const std::string& delim);
std::string readAll(const char* filePath);
// Graphics Func
static void renderSphere();
static int compile_shader(const string& file);

void GlfwErrorCallback(int error, const char* message)
{
	cout << error << message << endl;
}


const char* glsl_version = "#version 330";

int main(void) {
    glfwInit();
    // glfw set up
    glfwSetErrorCallback(GlfwErrorCallback);
    
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "FBX Exercise", NULL, NULL);
    if (!window)
    {
        // Window or OpenGL context creation failed
        cout << "Window setup is wrong is wrong" << __LINE__ <<endl;
    }
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    // glad init
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (!status) {
        cout << "something wrong with the loader" << endl;
    }
    // Imgui set up
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Set up theme, more themes can be chosen
    ImGui::StyleColorsDark();
    
    // Setup Platform and renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    // Scene Set up
    
    // Compile Shader
    
    u32 program = compile_shader("res/basic.shader");
    
    glEnable(GL_DEPTH_TEST);
    
    // projection, view, model setup
    mat4 projection = glm::perspective(glm::radians(45.0f), 1920.f / 1080.f, 0.1f, 100.0f);
    
    vec3 eye = vec3(0.0f, 0.0f, -2.0f);
    vec3 target = vec3(0.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    mat4 view = glm::lookAt(eye, target, up);
    mat4 model = scale(mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
    
    
    
    static bool show_demo_window = true;
    
    while (!glfwWindowShouldClose(window)) {
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, (float*) &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE,(float*) &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE,(float*) &model[0][0]);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();;
        
        
        glViewport(0, 0, 1920, 1080);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
        renderSphere();
        ImGui::ShowDemoWindow(&show_demo_window);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    const char* description;
    int code_1 = glfwGetError(&description);
    
    return 0;
}


static u32 compile_individual_shader(const char* file , u32 type) {
    u32 shader = glCreateShader(type);
    glShaderSource(shader, 1, &file, NULL);
    glCompileShader(shader);
    
    int success;
    char log[512];
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        string type_s;
        switch (type) {
            case GL_VERTEX_SHADER:
            type_s = "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER:
            type_s = "GL_FRAGMENT_SHADER";
            default:
            type_s = "UNKNOWN_SHADER";
        }
        
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        cout << "SHADER_COMPILE_FAILED: " << type_s << " Log: " << log << endl;
    }
    return shader;
}

i32 compile_shader(const string& file) {
    vector<string> contents = split(readAll(file.c_str()), "type - delim");
    
    u32 vert = compile_individual_shader(contents[0].c_str(), GL_VERTEX_SHADER);
    u32 frag = compile_individual_shader(contents[1].c_str(), GL_FRAGMENT_SHADER);
    
    u32 program = glCreateProgram();
    
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    
    glLinkProgram(program);
    glValidateProgram(program);
    
    // Log
    int success;
    char log[512];
    
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    if (!success) {
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        cout << "SHADER::FAILED::TOLOAD: " << file << endl; 
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    return program;
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
static void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);
        
        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        
        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;
        
        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                
                // Important(qiming):
                // Here I made it negative since I want to make the row going counterclock wise instead of clock wise
                float zPos = -std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                
                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }
        
        
        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        
        indexCount = static_cast<unsigned int>(indices.size());
        
        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
    
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

std::vector<std::string> split(std::string s, const std::string& delim)
{
    std::vector<std::string> ret;
    size_t pos;
    while ((pos = s.find(delim)) != std::string::npos) {
        ret.emplace_back(s.substr(0, pos));
        s.erase(0, pos + delim.length());
    }
    ret.emplace_back(s);
    return ret;
}

std::string readAll(const char* filePath)
{
    std::ifstream in;
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::string content;
    try
    {
        // open the file
        in.open(filePath);
        // open a stream to receive contents
        std::stringstream shaderStream;
        shaderStream << in.rdbuf();
        // Close file after finishing reading
        in.close();
        content = shaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "FILE::READ_FAILED\n" << " File: " << __FILE__ << " Passed in: " << filePath << std::endl;
    }
    return content;
}