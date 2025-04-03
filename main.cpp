// Autor: Nedeljko Tesanovic
// Opis: Fongovo osvjetljenje

#define _CRT_SECURE_NO_WARNINGS
 
#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "shader.hpp"
#include "model.hpp"
//#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath); //Ucitavanje teksture, izdvojeno u funkciju

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.0f, 1.0f, 1.0f);

int main(void)
{

   
    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    unsigned int wWidth = SCR_WIDTH;
    unsigned int wHeight = SCR_HEIGHT;
    const char wTitle[] = "Na kraj sela zuta kuca";
    window = glfwCreateWindow(wWidth, wHeight, wTitle, NULL, NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    
    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ PROMJENLJIVE I BAFERI +++++++++++++++++++++++++++++++++++++++++++++++++
    float sun[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    float house[] = {
        // First floor
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  // bottom-left
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  // top-right
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  // top-right
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  // top-left
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

        // Left face
        -0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Right face
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

        // Top face
        -0.5f, 0.0f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.0f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.0f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        // Second floor
        // Back face
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,  // bottom-left
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  // top-right
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,  // top-right
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,  // top-left
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

        // Back face front
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f, 0.0f,  // bottom-left
         0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f, 1.0f,  // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    1.0f, 1.0f,  // top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f, 1.0f,  // top-left
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, 1.0f,    0.0f, 0.0f,

        // Left face
        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   0.0f, 0.0f,  // bottom-left
        -0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,   1.0f, 0.0f,  // bottom-right
        -0.5f,  0.0f, -0.5f, 1.0f,  0.0f,  0.0f,   1.0f, 1.0f,  // top-right
        -0.5f,  0.0f, -0.5f, 1.0f,  0.0f,  0.0f,   1.0f, 1.0f,  // top-right
        -0.5f,  0.0f,  0.5f, 1.0f,  0.0f,  0.0f,   0.0f, 1.0f,  // top-left
        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

        // Right face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,  // bottom-left
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,  // bottom-right
         0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,  // top-right
         0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,  // top-right
         0.5f,  0.0f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,  // top-left
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,

        // Bottom face
        -0.5f,  0.0f, -0.5f,  0.0f,  -1.0f,  0.0f,   0.0f, 0.0f,  // bottom-left
        0.5f,   0.0f, -0.5f,  0.0f,  -1.0f,  0.0f,  1.0f, 0.0f,  // bottom-right
        0.5f,   0.0f,  0.5f,  0.0f,  -1.0f,  0.0f,  1.0f, 1.0f,  // top-right
        0.5f,   0.0f,  0.5f,  0.0f,  -1.0f,  0.0f,  1.0f, 1.0f,  // top-right
        -0.5f,  0.0f,  0.5f,  0.0f,  -1.0f,  0.0f,  0.0f, 1.0f,  // top-left
        -0.5f,  0.0f, -0.5f,  0.0f,  -1.0f,  0.0f,  0.0f, 0.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    0.0f, 0.0f,  // bottom-left
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,  // bottom-right
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,  // top-right
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,  // top-right
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    0.0f, 1.0f,  // top-left
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    0.0f, 0.0f,

    };

    float roof[] = {
    0.0f,  0.9f,  0.0f,  0.0f,  0.0f,  1.0f,
    0.6f, 0.5f,  0.6f,  0.0f,  0.0f,  1.0f,
    -0.6f, 0.5f,  0.6f,  0.0f,  0.0f,  1.0f,

    0.0f,  0.9f,  0.0f, -1.0f,  0.0f,  0.0f,
    -0.6f, 0.5f,  0.6f, -1.0f,  0.0f,  0.0f,
    -0.6f, 0.5f, -0.6f, -1.0f,  0.0f,  0.0f,

    0.0f,  0.9f,  0.0f,  0.0f,  0.0f,  -1.0f,
    -0.6f, 0.5f, -0.6f,  0.0f,  0.0f,  -1.0f,
    0.6f, 0.5f, -0.6f,   0.0f,  0.0f,  -1.0f,

    0.0f,  0.9f,  0.0f,  1.0f,  0.0f,  0.0f,
    0.6f, 0.5f, -0.6f,  1.0f,  0.0f,  0.0f,
    0.6f, 0.5f,  0.6f,  1.0f,  0.0f,  0.0f,
    };

    for (int i = 0; i < sizeof(roof) / sizeof(float); i += 6*3) {
        glm::vec3 vertexA(roof[i], roof[i + 1], roof[i + 2]);
        glm::vec3 vertexB(roof[i + 3], roof[i + 4], roof[i + 5]);
        glm::vec3 vertexC(roof[i + 6], roof[i + 7], roof[i + 8]);

        glm::vec3 vectorAB = vertexB - vertexA;
        glm::vec3 vectorAC = vertexC - vertexA;

        glm::vec3 normal = glm::cross(vectorAB, vectorAC);
        normal = glm::normalize(normal);

        roof[i + 3] = normal.x;
        roof[i + 4] = normal.y;
        roof[i + 5] = normal.z;

        roof[i + 6 + 3] = normal.x;
        roof[i + 6 + 4] = normal.y;
        roof[i + 6 + 5] = normal.z;

        roof[i + 12 + 3] = normal.x;
        roof[i + 12 + 4] = normal.y;
        roof[i + 12 + 5] = normal.z;
    }

    float smoke[] = {
        0.0f,  0.4f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.3f, 0.25f,  0.3f,  0.0f,  0.0f,  1.0f,
        -0.3f, 0.25f,  0.3f,  0.0f,  0.0f,  1.0f,

        0.0f,  0.4f,  0.0f,  -1.0f,  0.0f,  1.0f,
        -0.3f, 0.25f,  0.3f,  -1.0f,  0.0f,  1.0f,
        -0.5f, 0.25f,  0.0f,  -1.0f,  0.0f,  1.0f,

        0.0f,  0.4f,  0.0f,  -1.0f,  0.0f,  -1.0f,
        -0.5f, 0.25f,  0.0f,  -1.0f,  0.0f,  -1.0f,
        -0.3f, 0.25f, -0.3f, -1.0f,  0.0f,  -1.0f,

        0.0f,  0.4f,  0.0f,  0.0f,  0.0f,  -1.0f,
        -0.3f, 0.25f, -0.3f, 0.0f,  0.0f,  -1.0f,
        0.3f, 0.25f, -0.3f,   0.0f,  0.0f,  -1.0f,

        0.0f,  0.4f,  0.0f,  1.0f,  0.0f,  -1.0f,
        0.3f, 0.25f, -0.3f,   1.0f,  0.0f,  -1.0f,
        0.5f, 0.25f, 0.0f,   1.0f,  0.0f,  -1.0f,

        0.0f,  0.4f,  0.0f,  1.0f,  0.0f,  1.0f,
        0.5f, 0.25f, 0.0f,   1.0f,  0.0f,  1.0f,
        0.3f, 0.25f,  0.3f,  1.0f,  0.0f,  1.0f,

        //donja
        0.0f, 0.1f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.3f, 0.25f,  0.3f,  0.0f,  0.0f,  1.0f,
        -0.3f, 0.25f,  0.3f,  0.0f,  0.0f,  1.0f,

        0.0f, 0.1f,  0.0f,  -1.0f,  0.0f,  1.0f,
        -0.3f, 0.25f,  0.3f,  -1.0f,  0.0f,  1.0f,
        -0.5f, 0.25f,  0.0f,  -1.0f,  0.0f,  1.0f,

        0.0f, 0.1f,  0.0f,  -1.0f,  0.0f,  -1.0f,
        -0.5f, 0.25f,  0.0f,  -1.0f,  0.0f,  -1.0f,
        -0.3f, 0.25f, -0.3f, -1.0f,  0.0f,  -1.0f,

        0.0f, 0.1f,  0.0f,  0.0f,  0.0f,  -1.0f,
        -0.3f, 0.25f, -0.3f, 0.0f,  0.0f,  -1.0f,
        0.3f, 0.25f, -0.3f,   0.0f,  0.0f,  -1.0f,

        0.0f, 0.1f,  0.0f,   1.0f,  0.0f,  -1.0f,
        0.3f, 0.25f, -0.3f,    1.0f,  0.0f,  -1.0f,
        0.5f, 0.25f, 0.0f,    1.0f,  0.0f,  -1.0f,

        0.0f, 0.1f,  0.0f,  1.0f,  0.0f,  1.0f,
        0.5f, 0.25f, 0.0f,   1.0f,  0.0f,  1.0f,
        0.3f, 0.25f,  0.3f,  1.0f,  0.0f,  1.0f,
    };

    glm::mat4 transformModel = glm::mat4(1.0f);
    transformModel = glm::translate(transformModel, glm::vec3(-0.3f, 0.9f, 0.0f));
    transformModel = glm::scale(transformModel, glm::vec3(0.1f, 0.25f, 0.2f));

    for (int i = 0; i < sizeof(smoke) / sizeof(float); i += 6) {
        glm::vec4 vertex(smoke[i], smoke[i + 1], smoke[i + 2], 1.0f);
        vertex = transformModel * vertex;

        // update the modified vertices
        smoke[i] = vertex.x;
        smoke[i + 1] = vertex.y;
        smoke[i + 2] = vertex.z;

        // update the modified normals
        smoke[i + 3] = 0.0f;
        smoke[i + 4] = 0.0f;
        smoke[i + 5] = 0.1f;
    }

    float fence[3 * sizeof(sun) / sizeof(float)];
    glm::vec3 translations[3] = {
    glm::vec3(0.0f, -0.4f, 1.3f),
    glm::vec3(-0.25f, 0.0f, 1.3f),
    glm::vec3(-0.4f, 0.0f, 1.3f)
    };

    float scales[3] = {
        0.05f,
        0.3f,
        0.02f
    };
    
    int sunVertexCount = sizeof(sun) / sizeof(float) / 8; // Assuming each vertex has 8 components

    for (int i = 0; i < 3; ++i) {
        glm::mat4 transform = glm::mat4(1.0f);
        
        if (i == 1 || i == 2) {
            transform = glm::rotate(transform, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        transform = glm::translate(transform, translations[i]);
        transform = glm::scale(transform, glm::vec3(scales[0], scales[1], scales[2]));

        for (int j = 0; j < sunVertexCount; ++j) {
            glm::vec4 vertex(sun[j * 8], sun[j * 8 + 1], sun[j * 8 + 2], 1.0f);
            vertex = transform * vertex;

            fence[i * sunVertexCount * 8 + j * 8] = vertex.x;
            fence[i * sunVertexCount * 8 + j * 8 + 1] = vertex.y;
            fence[i * sunVertexCount * 8 + j * 8 + 2] = vertex.z;

            glm::vec3 normal(sun[j * 8 + 3], sun[j * 8 + 4], sun[j * 8 + 5]);
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
            normal = glm::normalize(normalMatrix * normal);

            fence[i * sunVertexCount * 8 + j * 8 + 3] = normal.x;
            fence[i * sunVertexCount * 8 + j * 8 + 4] = normal.y;
            fence[i * sunVertexCount * 8 + j * 8 + 5] = normal.z;


            fence[i * sunVertexCount * 8 + j * 8 + 6] = sun[j * 8 + 6];
            fence[i * sunVertexCount * 8 + j * 8 + 7] = sun[j * 8 + 7];
        }
    }

    float indeks[] = {
        // X    Y      S         T
        -1.0,  0.9,   0.0,      0.0,
        -1.0,  1.0,   0.0,      1.0,
        -0.8,  0.9,   1.0,      0.0,
        -0.8,  1.0,   1.0,      1.0,
    };

    unsigned int stride = (3 + 3 + 2) * sizeof(float); 
    
    unsigned int VAO[10];    
    unsigned int VBO[10];
    

    glGenVertexArrays(1, &VAO[0]);
    glBindVertexArray(VAO[0]);

    glGenBuffers(1, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(house), house, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);

    glGenBuffers(1, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(roof), roof, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAO[2]);
    glBindVertexArray(VAO[2]);

    glGenBuffers(1, &VBO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sun), sun, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::mat4 doorModel = glm::mat4(1.0f);    
    doorModel = glm::translate(doorModel, glm::vec3(0.0f, -0.35f, 0.5f));
    doorModel = glm::scale(doorModel, glm::vec3(0.2f, 0.3f, 0.05f));
    float door[sizeof(sun) / sizeof(float)];

    for (int i = 0; i < sizeof(sun) / sizeof(float); i += 8) {
        glm::vec4 vertex(sun[i], sun[i + 1], sun[i + 2], 1.0f);
        vertex = doorModel * vertex;

        // Update the modified vertices
        door[i] = vertex.x;
        door[i + 1] = vertex.y;
        door[i + 2] = vertex.z;

        glm::vec3 normal(sun[i + 3], sun[i + 4], sun[i + 5]);
        // Transpose-inverse of the 3x3 upper-left part of the model matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(doorModel)));
        normal = glm::normalize(normalMatrix * normal);

        // Update the modified normals
        door[i + 3] = normal.x;
        door[i + 4] = normal.y;
        door[i + 5] = normal.z;
    }

    glGenVertexArrays(1, &VAO[3]);
    glBindVertexArray(VAO[3]);

    glGenBuffers(1, &VBO[3]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(door), door, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glm::mat4 chimneyModel = glm::mat4(1.0f);
    chimneyModel = glm::translate(chimneyModel, glm::vec3(-0.35f, 0.7f, 0.0f));
    chimneyModel = glm::scale(chimneyModel, glm::vec3(0.1f, 0.3f, 0.1f));
    float chimney[sizeof(sun) / sizeof(float)];

    for (int i = 0; i < sizeof(sun) / sizeof(float); i += 8) {
        glm::vec4 vertex(sun[i], sun[i + 1], sun[i + 2], 1.0f);
        vertex = chimneyModel * vertex;

        // Update the modified vertices
        chimney[i] = vertex.x;
        chimney[i + 1] = vertex.y;
        chimney[i + 2] = vertex.z;

        glm::vec3 normal(sun[i + 3], sun[i + 4], sun[i + 5]);
        // Transpose-inverse of the 3x3 upper-left part of the model matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(chimneyModel))); 
        normal = glm::normalize(normalMatrix * normal);

        // Update the modified normals
        chimney[i + 3] = normal.x;
        chimney[i + 4] = normal.y;
        chimney[i + 5] = normal.z;
    }

    glGenVertexArrays(1, &VAO[4]);
    glBindVertexArray(VAO[4]);

    glGenBuffers(1, &VBO[4]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[4]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(chimney), chimney, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAO[5]);
    glBindVertexArray(VAO[5]);

    glGenBuffers(1, &VBO[5]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[5]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(smoke), smoke, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &VAO[6]);
    glBindVertexArray(VAO[6]);

    glGenBuffers(1, &VBO[6]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[6]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fence), fence, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glm::mat4 windowModel = glm::mat4(1.0f);
    windowModel = glm::translate(windowModel, glm::vec3(0.0f, -0.6f, 1.0f));
    windowModel = glm::scale(windowModel, glm::vec3(0.6f, 0.4f, 0.05f));
    float window1[sizeof(sun) / sizeof(float)];

    for (int i = 0; i < sizeof(sun) / sizeof(float); i += 8) {
        glm::vec4 vertex(sun[i], sun[i + 1], sun[i + 2], 1.0f);
        vertex = windowModel * vertex;

        // Update the modified vertices
        window1[i] = vertex.x;
        window1[i + 1] = vertex.y;
        window1[i + 2] = vertex.z;

        glm::vec3 normal(sun[i + 3], sun[i + 4], sun[i + 5]);
        // Transpose-inverse of the 3x3 upper-left part of the model matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(windowModel))); 
        normal = glm::normalize(normalMatrix * normal);

        // Update the modified normals
        window1[i + 3] = normal.x;
        window1[i + 4] = normal.y;
        window1[i + 5] = normal.z;
    }

    glGenVertexArrays(1, &VAO[7]);
    glBindVertexArray(VAO[7]);

    glGenBuffers(1, &VBO[7]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[7]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(window1), window1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    windowModel = glm::mat4(1.0f);
    windowModel = glm::translate(windowModel, glm::vec3(0.0f, -0.56f, 0.0f));
    windowModel = glm::scale(windowModel, glm::vec3(4.0f, 0.1f, 4.0f));
    float grass[sizeof(sun) / sizeof(float)];

    for (int i = 0; i < sizeof(sun) / sizeof(float); i += 8) {
        glm::vec4 vertex(sun[i], sun[i + 1], sun[i + 2], 1.0f);
        vertex = windowModel * vertex;

        // Update the modified vertices
        grass[i] = vertex.x;
        grass[i + 1] = vertex.y;
        grass[i + 2] = vertex.z;

        glm::vec3 normal(sun[i + 3], sun[i + 4], sun[i + 5]);
        // Transpose-inverse of the 3x3 upper-left part of the model matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(windowModel)));
        normal = glm::normalize(normalMatrix * normal);

        // Update the modified normals
        grass[i + 3] = normal.x;
        grass[i + 4] = normal.y;
        grass[i + 5] = normal.z;

        grass[i + 6] = sun[i + 6];
        grass[i + 7] = sun[i + 7];
        
    }

    glGenVertexArrays(1, &VAO[8]);
    glBindVertexArray(VAO[8]);

    glGenBuffers(1, &VBO[8]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[8]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grass), grass, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &VAO[9]);
    glBindVertexArray(VAO[9]);

    glGenBuffers(1, &VBO[9]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[9]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(indeks), indeks, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ SEJDERI +++++++++++++++++++++++++++++++++++++++++++++++++
    Model lija("res/13463_Australian_Cattle_Dog_v3.obj");
    Model lego("res/lego.obj");
    Model tree("res/tree.obj");

    Shader modelShader("phong.vert", "phong.frag");
    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");
    unsigned int phongShader = createShader("phong.vert", "phong.frag");
    unsigned int indexShader = createShader("indeks.vert", "indeks.frag");

    glm::mat4 model = glm::mat4(1.0f);
    unsigned int modelLoc = glGetUniformLocation(phongShader, "uM");
    unsigned int modelLocUni = glGetUniformLocation(unifiedShader, "uM");

    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 cameraPosition = glm::vec3(0.0f, 1.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float cameraSpeed = 0.01f;
    view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    unsigned int viewLoc = glGetUniformLocation(phongShader, "uV");
    unsigned int viewLocUni = glGetUniformLocation(unifiedShader, "uV");

    glm::mat4 projectionP = glm::perspective(glm::radians(100.0f), (float)wWidth / (float)wHeight, 0.1f, 100.0f);
    unsigned int projectionLoc = glGetUniformLocation(phongShader, "uP");
    unsigned int projectionLocUni = glGetUniformLocation(unifiedShader, "uP");

    unsigned int viewPosLoc = glGetUniformLocation(phongShader, "uViewPos");

    unsigned int lightPosLoc = glGetUniformLocation(phongShader, "dirLight.pos");
    unsigned int lightALoc = glGetUniformLocation(phongShader, "dirLight.kA");
    unsigned int lightDLoc = glGetUniformLocation(phongShader, "dirLight.kD");
    unsigned int lightSLoc = glGetUniformLocation(phongShader, "dirLight.kS");

    unsigned int pointLightPos1 = glGetUniformLocation(phongShader, "pointLight1.pos");
    unsigned int pointLightA1 = glGetUniformLocation(phongShader, "pointLight1.kA");
    unsigned int pointLightD1 = glGetUniformLocation(phongShader, "pointLight1.kD");
    unsigned int pointLightS1 = glGetUniformLocation(phongShader, "pointLight1.kS");
    unsigned int pointLightC1 = glGetUniformLocation(phongShader, "pointLight1.constant");
    unsigned int pointLightL1 = glGetUniformLocation(phongShader, "pointLight1.linear");
    unsigned int pointLightQ1 = glGetUniformLocation(phongShader, "pointLight1.quadratic");

    unsigned int pointLightPos2 = glGetUniformLocation(phongShader, "pointLight2.pos");
    unsigned int pointLightA2 = glGetUniformLocation(phongShader, "pointLight2.kA");
    unsigned int pointLightD2 = glGetUniformLocation(phongShader, "pointLight2.kD");
    unsigned int pointLightS2 = glGetUniformLocation(phongShader, "pointLight2.kS");
    unsigned int pointLightC2 = glGetUniformLocation(phongShader, "pointLight2.constant");
    unsigned int pointLightL2 = glGetUniformLocation(phongShader, "pointLight2.linear");
    unsigned int pointLightQ2 = glGetUniformLocation(phongShader, "pointLight2.quadratic");

    unsigned int spotLightPos = glGetUniformLocation(phongShader, "spotLight.pos");
    unsigned int spotLightA = glGetUniformLocation(phongShader, "spotLight.kA");
    unsigned int spotLightD = glGetUniformLocation(phongShader, "spotLight.kD");
    unsigned int spotLightS = glGetUniformLocation(phongShader, "spotLight.kS");
    unsigned int spotLightC = glGetUniformLocation(phongShader, "spotLight.constant");
    unsigned int spotLightL = glGetUniformLocation(phongShader, "spotLight.linear");
    unsigned int spotLightQ = glGetUniformLocation(phongShader, "spotLight.quadratic");
    unsigned int spotLightCutOff = glGetUniformLocation(phongShader, "spotLight.cutOff");
    unsigned int spotLightouterCutoff = glGetUniformLocation(phongShader, "spotLight.outerCutOff");
    unsigned int spotLightDir = glGetUniformLocation(phongShader, "spotLight.direction");

    unsigned int materialShineLoc = glGetUniformLocation(phongShader, "uMaterial.shine");
    unsigned int materialALoc = glGetUniformLocation(phongShader, "uMaterial.kA");
    unsigned int materialDLoc = glGetUniformLocation(phongShader, "uMaterial.kD");
    unsigned int materialSLoc = glGetUniformLocation(phongShader, "uMaterial.kS");

    unsigned int alphaLoc = glGetUniformLocation(phongShader, "uAlpha");
    unsigned int uTranslationLoc = glGetUniformLocation(phongShader, "uTranslation");
    unsigned int uScaleLoc = glGetUniformLocation(phongShader, "uScale");
    unsigned int uApplyTransformLoc = glGetUniformLocation(phongShader, "uApplyTransform");
    unsigned int grassLoc = glGetUniformLocation(phongShader, "grass");
    unsigned int houseLoc = glGetUniformLocation(phongShader, "house");
    

    unsigned int sunColorLoc = glGetUniformLocation(unifiedShader, "sunColor");
    unsigned int useSunColorLoc = glGetUniformLocation(unifiedShader, "useSunColor");
    glUniform1i(useSunColorLoc, 0); 

    glUseProgram(phongShader);

    glUniform1f(alphaLoc, 1.0f);
    glUniform1i(uApplyTransformLoc, 0);
    glUniform1i(grassLoc, 0);
    glUniform1i(houseLoc, 0);
    //glUniform1i(modelBLoc, 0);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));

    glUniform3f(viewPosLoc, 0.0, 0.0, 2.0);
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(lightALoc, 0.5f, 0.5f, 0.5f);
    glUniform3f(lightDLoc, 0.8f, 0.8f, 0.8f);
    glUniform3f(lightSLoc, 0.9f, 0.9f, 0.9f);

    glUniform3f(pointLightPos1, 0.0f, 0.48f, 0.0f);
    glUniform3f(pointLightA1, 0.4f, 0.4f, 0.4f);
    glUniform3f(pointLightD1, 0.5f, 0.5f, 0.5f);
    glUniform3f(pointLightS1, 0.7f, 0.7f, 0.7f);
    glUniform1f(pointLightC1, 1.0f);
    glUniform1f(pointLightL1, 1.0f);
    glUniform1f(pointLightQ1, 2.0f);

    glUniform3f(pointLightPos2, 0.0f, -0.05f, 0.0f);
    glUniform3f(pointLightA2, 0.4f, 0.4f, 0.4f);
    glUniform3f(pointLightD2, 0.5f, 0.5f, 0.5f);
    glUniform3f(pointLightS2, 0.7f, 0.7f, 0.7f);
    glUniform1f(pointLightC2, 1.0f);
    glUniform1f(pointLightL2, 1.0f);
    glUniform1f(pointLightQ2, 2.0f);


    glUniform3f(spotLightPos, 0.0f, 0.0f, 0.5f);
    glUniform3f(spotLightA, 0.1f, 0.1f, 0.1f);
    glUniform3f(spotLightD, 1.0f, 1.0f, 1.0f);
    glUniform3f(spotLightS, 1.0f, 1.0f, 1.0f);
    glUniform1f(spotLightC, 1.0f);
    glUniform1f(spotLightL, 0.09f);
    glUniform1f(spotLightQ, 0.032f);
    glUniform1f(spotLightCutOff, glm::cos(glm::radians(12.5f)));
    glUniform3f(spotLightDir, 0.0f, -0.5f, 0.1f);
    glUniform1f(spotLightouterCutoff, glm::cos(glm::radians(15.0f)));

    modelShader.use();
    modelShader.setVec3("uLightPos", lightPos.x, lightPos.y, lightPos.z);
    modelShader.setVec3("uViewPos", 1.0, 0, 1.0);
    modelShader.setVec3("uLightColor", 1, 1, 1);
    modelShader.setMat4("uP", projectionP);
    modelShader.setMat4("uV", view);
    modelShader.setVec3("dirLight.kA", 0.4f, 0.4f, 0.4f);
    modelShader.setVec3("dirLight.kS", 0.5f, 0.5f, 0.5f);
    modelShader.setVec3("dirLight.kD", 0.7f, 0.7f, 0.7f);

    modelShader.setVec3("uMaterial.kA", 0.2f, 0.2f, 0.2f); // Ambient reflection coefficient
    modelShader.setVec3("uMaterial.kD", 0.8f, 0.8f, 0.8f); // Diffuse reflection coefficient
    modelShader.setVec3("uMaterial.kS", 0.5f, 0.5f, 0.5f); // Specular reflection coefficient
    modelShader.setFloat("uMaterial.shine", 32.0f); // Shininess factor

    modelShader.setVec3("pointLight1.pos", 0.0f, 0.48f, 0.0f);
    modelShader.setVec3("pointLight1.kA", 0.4f, 0.4f, 0.4f);
    modelShader.setVec3("pointLight1.kS", 0.5f, 0.5f, 0.5f);
    modelShader.setVec3("pointLight1.kD", 0.7f, 0.7f, 0.7f);
    modelShader.setFloat("pointLight1.constant", 1.0f);
    modelShader.setFloat("pointLight1.linear", 1.0f);
    modelShader.setFloat("pointLight1.quadratic", 2.0f);  

    
    unsigned wallpaperTexture = loadImageToTexture("res/wallpaper.jpg"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, wallpaperTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // moguce je kombinovanje, posebno definisemo za svaku koordinatu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR
    //glBindTexture(GL_TEXTURE_2D, 0);
    unsigned uTexLoc = glGetUniformLocation(phongShader, "uTex");
    glUniform1i(uTexLoc, 0); // Indeks teksturne jedinice (sa koje teksture ce se citati boje)

    unsigned treeTexture = loadImageToTexture("res/tree.png"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, treeTexture); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // moguce je kombinovanje, posebno definisemo za svaku koordinatu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);   //GL_NEAREST, GL_LINEAR

    unsigned diffuseMap = loadImageToTexture("res/Grass_001_COLOR.jpg"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, diffuseMap); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // moguce je kombinovanje, posebno definisemo za svaku koordinatu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned specularMap = loadImageToTexture("res/Grass_001_ROUGH.jpg"); //Ucitavamo teksturu
    glBindTexture(GL_TEXTURE_2D, specularMap); //Podesavamo teksturu
    glGenerateMipmap(GL_TEXTURE_2D); //Generisemo mipmape 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // moguce je kombinovanje, posebno definisemo za svaku koordinatu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUseProgram(indexShader);
    unsigned uTexIndexLoc = glGetUniformLocation(indexShader, "uTex");
    glUniform1i(uTexIndexLoc, 0);
    unsigned indeksTexture = loadImageToTexture("res/indeks.png"); //Ucitavamo teksturu

    glBindTexture(GL_TEXTURE_2D, indeksTexture); //Podesavamo teksturu

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//S = U = X    GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// T = V = Y
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // moguce je kombinovanje, posebno definisemo za svaku koordinatu
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);   //GL_NEAREST, GL_LINEAR
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(phongShader);

    glUniform1i(glGetUniformLocation(phongShader, "diffuseMap"), 1);
    glUniform1i(glGetUniformLocation(phongShader, "specularMap"), 2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ RENDER LOOP - PETLJA ZA CRTANJE +++++++++++++++++++++++++++++++++++++++++++++++++
    float horizontalAngle = 0.0f;
    float rotationSpeed = 1.0f;  // Adjust the rotation speed as needed
    float rotationAngle = -10.0f;
    float windowAlpha = 1.0f;
    /*float gateWidth = 2.0f;*/

    //glClearDepth(1.0f);
    

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
                
        glClearColor(0.7, 0.9, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            cameraPosition += cameraSpeed * glm::normalize(cameraTarget - cameraPosition);
        } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cameraPosition -= cameraSpeed * glm::normalize(cameraTarget - cameraPosition);
        } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            glEnable(GL_DEPTH_TEST);
        } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            glDisable(GL_DEPTH_TEST);
        } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            //glFrontFace(GL_CW); //za prozor
        } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            glDisable(GL_CULL_FACE);
        } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            horizontalAngle += rotationSpeed * deltaTime;
            
        } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            horizontalAngle -= rotationSpeed * deltaTime;   
        } else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        {
            windowAlpha = 0.5f;
        } else if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        {
            windowAlpha = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            glUniform3f(pointLightPos1, 0.0, 0.48, 0.0);
            glUniform3f(pointLightA1, 0.4, 0.4, 0.4);
            glUniform3f(pointLightD1, 0.5, 0.5, 0.5);
            glUniform3f(pointLightS1, 0.7, 0.7, 0.7);
            glUniform1f(pointLightC1, 1.0);
            glUniform1f(pointLightL1, 1.0);
            glUniform1f(pointLightQ1, 2.0);

            glUniform3f(pointLightPos2, 0.0, -0.05, 0.0);
            glUniform3f(pointLightA2, 0.4, 0.4, 0.4);
            glUniform3f(pointLightD2, 0.5, 0.5, 0.5);
            glUniform3f(pointLightS2, 0.7, 0.7, 0.7);
            glUniform1f(pointLightC2, 1.0);
            glUniform1f(pointLightL2, 1.0);
            glUniform1f(pointLightQ2, 2.0);

            modelShader.setVec3("pointLight1.pos", 0.0f, 0.48f, 0.0f);
            modelShader.setVec3("pointLight1.kA", 0.4f, 0.4f, 0.4f);
            modelShader.setVec3("pointLight1.kS", 0.5f, 0.5f, 0.5f);
            modelShader.setVec3("pointLight1.kD", 0.7f, 0.7f, 0.7f);
            modelShader.setFloat("pointLight1.constant", 1.0f);
            modelShader.setFloat("pointLight1.linear", 1.0f);
            modelShader.setFloat("pointLight1.quadratic", 2.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            glUniform3f(pointLightPos1, 0.0, 0.48, 0.0);
            glUniform3f(pointLightA1, 0.0, 0.0, 0.0);
            glUniform3f(pointLightD1, 0.0, 0.0, 0.0);
            glUniform3f(pointLightS1, 0.0, 0.0, 0.0);
            glUniform1f(pointLightC1, 1.0);
            glUniform1f(pointLightL1, 1.0);
            glUniform1f(pointLightQ1, 2.0);

            glUniform3f(pointLightPos2, 0.0, -0.05, 0.0);
            glUniform3f(pointLightA2, 0.0, 0.0, 0.0);
            glUniform3f(pointLightD2, 0.0, 0.0, 0.0);
            glUniform3f(pointLightS2, 0.0, 0.0, 0.0);
            glUniform1f(pointLightC2, 1.0);
            glUniform1f(pointLightL2, 1.0);
            glUniform1f(pointLightQ2, 2.0);

            modelShader.setVec3("pointLight1.pos", 0.0f, 0.48f, 0.0f);
            modelShader.setVec3("pointLight1.kA", 0.0f, 0.0f, 0.0f);
            modelShader.setVec3("pointLight1.kS", 0.0f, 0.0f, 0.0f);
            modelShader.setVec3("pointLight1.kD", 0.0f, 0.0f, 0.0f);
            modelShader.setFloat("pointLight1.constant", 1.0f);
            modelShader.setFloat("pointLight1.linear", 1.0f);
            modelShader.setFloat("pointLight1.quadratic", 2.0f);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            rotationAngle -= 1.0f;
            rotationAngle = glm::clamp(rotationAngle, -90.0f, 90.0f);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            rotationAngle += 1.0f;
            rotationAngle = glm::clamp(rotationAngle, -90.0f, 90.0f);
        }
        
        glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
        view = glm::rotate(view, horizontalAngle, glm::vec3(0.0f, 1.0f, 0.0f));
        
        glm::mat4 modelSun = glm::mat4(1.0f);
        glUseProgram(unifiedShader);

        glUniformMatrix4fv(viewLocUni, 1, GL_FALSE, glm::value_ptr(view));
        modelSun = glm::translate(modelSun, lightPos);
        modelSun = glm::scale(modelSun, glm::vec3(0.1f)); // a smaller cube
        glUniformMatrix4fv(modelLocUni, 1, GL_FALSE, glm::value_ptr(modelSun));
        glUniformMatrix4fv(viewLocUni, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocUni, 1, GL_FALSE, glm::value_ptr(projectionP));

        float sunColor = abs(sin(0.5 * (abs(0.5) + abs(0.5)) * glfwGetTime()));
            //(1.0, max(0.6, sunColor), 0.0
        glUniform1f(sunColorLoc, sunColor);  // Set sun color
        modelSun = glm::rotate(modelSun, glm::radians(20.0f * currentTime), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLocUni, 1, GL_FALSE, glm::value_ptr(modelSun));
        glUniform1i(useSunColorLoc, 1);
        glBindVertexArray(VAO[2]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glUniform1i(useSunColorLoc, 0);

        glUseProgram(phongShader);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionP));
        

        // TRAVA
        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(lightALoc, 0.5f, 0.5f, 0.5f);
        glUniform3f(lightDLoc, 0.6f, 0.6f, 0.6f);
        glUniform3f(lightSLoc, 1.0f, 1.0f, 1.0f);
        glUniform1i(uApplyTransformLoc, 0);
        glUniform1f(materialShineLoc, 120.0f);
        // bind diffuse map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // bind specular map
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glUniform1i(grassLoc, 1);
        glBindVertexArray(VAO[8]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(grass) / sizeof(float) / 8);
        glUniform1i(grassLoc, 0);

        
        // OGRADA
        glUniform1i(uApplyTransformLoc, 1);
        glUniform1f(uScaleLoc, 1.0);
        glUniform3f(materialALoc, 1.0, 1.0, 1.0);
        glUniform3f(materialDLoc, 1.0, 1.0, 1.0);
        glUniform3f(materialSLoc, 1.0, 1.0, 1.0);
        float translationsX[] = { -0.3, -0.6, -0.9, -1.2, -1.5, 0.3, 0.6, 0.9, 1.2, 1.5, 0.0 };
        
        for (int i = 0; i < sizeof(translationsX) / sizeof(float); ++i) {
            // back side
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(uTranslationLoc, translationsX[i], 0.0, -2.9);
            glBindVertexArray(VAO[6]);
            glDrawArrays(GL_TRIANGLES, 0, sizeof(fence) / (8 * sizeof(float)));

            // front side
            if (translationsX[i] != 0.0) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                glUniform3f(uTranslationLoc, translationsX[i], 0.0, 0.0);
                glBindVertexArray(VAO[6]);
                glDrawArrays(GL_TRIANGLES, 0, sizeof(fence) / (8 * sizeof(float)));

                glm::mat4 modelMatrix = glm::mat4(1.0f);
                glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                modelMatrix = modelMatrix * rotationMatrix;
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniform3f(uTranslationLoc, 1.5 - i * 0.3, 0.0, -2.9);
                glBindVertexArray(VAO[6]);
                glDrawArrays(GL_TRIANGLES, 0, sizeof(fence) / (8 * sizeof(float)));


                rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                glUniform3f(uTranslationLoc, 1.2 - i * 0.3, 0.0, -2.9);
                modelMatrix = modelMatrix * rotationMatrix;
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glBindVertexArray(VAO[6]);
                glDrawArrays(GL_TRIANGLES, 0, sizeof(fence) / (8 * sizeof(float)));
            }
        }
        // KAPIJA
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        float gateWidth = 1.0f;

        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.15f, 0.0f, 1.3f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.15f, 0.0f, -1.3f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniform1i(uApplyTransformLoc, 1);
        glUniform3f(uTranslationLoc, 0.0, 0.0, 0.0);
        glBindVertexArray(VAO[6]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(fence) / (8 * sizeof(float)));
        glUniform1i(uApplyTransformLoc, 0);


        // KUCA
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(houseLoc, 1);
        glBindVertexArray(VAO[0]);
        glUniform1f(materialShineLoc, 20.0f);
        glUniform3f(materialALoc, 1.0f, 0.9f, 0.0f);   // Ambient color (yellow)
        glUniform3f(materialDLoc, 1.0f, 0.9f, 0.3f);   // Diffuse color (yellow)
        glUniform3f(materialSLoc, 1.0f, 0.9f, 0.6f);   // Specular color (yellow)

        glActiveTexture(GL_TEXTURE0); //tekstura koja se bind-uje nakon ovoga ce se koristiti sa SAMPLER2D uniformom u sejderu koja odgovara njenom indeksu
        glBindTexture(GL_TEXTURE_2D, wallpaperTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDrawArrays(GL_TRIANGLES, 36, 36);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(houseLoc, 0);
        // KROV
        glUniform1f(materialShineLoc, 10.f);
        glUniform3f(materialALoc, 0.1745, 0.01175, 0.1175);
        glUniform3f(materialDLoc, 0.61424, 0.04136, 0.04136);
        glUniform3f(materialSLoc, 0.72811, 0.626959, 0.62959);
        glBindVertexArray(VAO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        
        // VRATA
        glUniform1f(materialShineLoc, 10.0f);
        glUniform3f(materialALoc, 0.25f, 0.15f, 0.05f);     // Ambient color
        glUniform3f(materialDLoc, 0.6f, 0.3f, 0.1f);        // Diffuse color
        glUniform3f(materialSLoc, 0.2f, 0.1f, 0.05f);       // Specular 
        //glUniformMatrix4fv(modelLocUni, 1, GL_FALSE, glm::value_ptr(doorModel));
        glBindVertexArray(VAO[3]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(door) / sizeof(float) / 8);


        // DIMNJAK
        glUniform1i(houseLoc, 1);
        glUniform1f(materialShineLoc, 10.0f);
        glUniform3f(materialALoc, 0.1745, 0.01175, 0.1175);
        glUniform3f(materialDLoc, 0.61424, 0.04136, 0.04136);
        glUniform3f(materialSLoc, 0.72811, 0.626959, 0.62959);
        glBindVertexArray(VAO[4]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(chimney) / sizeof(float) / 8);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(houseLoc, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // DIM
        glUniform1f(alphaLoc, abs(sin(0.75 * (abs(0.5) + abs(0.5)) * currentTime)));
        glUniform3f(materialALoc, 0.7, 0.7, 0.7);
        glUniform3f(materialDLoc, 0.7, 0.7, 0.7);       
        glUniform3f(materialSLoc, 0.7, 0.7, 0.7);       
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glUniform1f(alphaLoc, abs(sin(0.65 * (abs(0.5) + abs(0.5)) * currentTime)));
        glUniform1i(uApplyTransformLoc, 1);
        glUniform3f(uTranslationLoc, 0.0, 0.3, 0.0);
        glUniform1f(uScaleLoc, 0.75);
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLES, 0, 36);     

        glUniform1f(alphaLoc, abs(sin(0.55 * (abs(0.5) + abs(0.5)) * currentTime)));
        glUniform1i(uApplyTransformLoc, 1);
        glUniform3f(uTranslationLoc, 0.00, 0.6, 0.0);
        glUniform1f(uScaleLoc, 0.5);
        glBindVertexArray(VAO[5]);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // PROZOR
        glUniform1f(alphaLoc, windowAlpha);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1f(materialShineLoc, 1.0f);
        glUniform3f(materialALoc, 0.0f, 0.05f, 0.1f);     // Ambient color
        glUniform3f(materialDLoc, 0.4f, 0.7f, 1.0f);       // Diffuse color (light blue)
        glUniform3f(materialSLoc, 0.8f, 0.8f, 0.9f);       // 
        glBindVertexArray(VAO[7]);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(window1) / sizeof(float) / 8);
        
        
        glDisable(GL_BLEND);
        glUniform1f(alphaLoc, 1.0f);

        modelShader.use();
        modelShader.setMat4("uV", view);

        glm::mat4 dogModel = glm::mat4(1.0f);

        dogModel = glm::rotate(dogModel, glm::radians(currentTime*5), glm::vec3(0.0f, 1.0f, 0.0f));
        dogModel = glm::translate(dogModel, glm::vec3(-0.7f, -0.5f, -0.7f));
        dogModel = glm::rotate(dogModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        
        dogModel = glm::scale(dogModel, glm::vec3(0.01f));

        modelShader.setMat4("uM", dogModel);
        modelShader.setBool("model", true);
        lija.Draw(modelShader);

        dogModel = glm::mat4(1.0f);
        dogModel = glm::translate(dogModel, glm::vec3(-1.0f, -0.5f, -0.9f));
        dogModel = glm::scale(dogModel, glm::vec3(0.01f));

        modelShader.setMat4("uM", dogModel);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, treeTexture);
        tree.Draw(modelShader);
        modelShader.setBool("model", false);

        glm::mat4 legoModel = glm::mat4(1.0f);
        legoModel = glm::translate(legoModel, glm::vec3(0.0f, 0.15f, 0.0f));
        legoModel = glm::scale(legoModel, glm::vec3(0.1f));
        modelShader.setMat4("uM", legoModel);
        modelShader.setBool("lego", true);
        lego.Draw(modelShader);
        modelShader.setBool("lego", false);

        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(indexShader);
        glBindVertexArray(VAO[9]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, indeksTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(indeks) / (4 * sizeof(float)));
        

        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
       

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++ POSPREMANJE +++++++++++++++++++++++++++++++++++++++++++++++++


    glDeleteBuffers(10, VBO);
    glDeleteVertexArrays(10, VAO);
    glDeleteProgram(phongShader);
    glDeleteProgram(unifiedShader);
    glDeleteProgram(indexShader);
    glDeleteTextures(1, &wallpaperTexture);

    glfwTerminate();
    return 0;
}

unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}
unsigned int createShader(const char* vsSource, const char* fsSource)
{
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    // 0 znaci nema ogranicenja, tako ce uvek biti
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        //Slike se osnovno ucitavaju naopako pa se moraju ispraviti da budu uspravne
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        // Provjerava koji je format boja ucitane slike
        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        // oslobadjanje memorije zauzete sa stbi_load posto vise nije potrebna
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
