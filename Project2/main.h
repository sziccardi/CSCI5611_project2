#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <cmath>
#include <chrono> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <algorithm>

#include "vertex.h"
#include "2d_mesh.h"
#include "matrices.h"

/* References */
//https://learnopengl.com/Getting-started/Camera
//https://learnopengl.com/Getting-started/Textures
//https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/3.2.blending_sort/blending_sorted.cpp
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/mesh.h
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader.h
//https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/model.h
//https://learnopengl.com/code_viewer_gh.php?code=src/3.model_loading/1.model_loading/model_loading.cpp

#define glCheckError() glCheckError_(__FILE__, __LINE__) 
#define GLM_ENABLE_EXPERIMENTAL

#define M_PI 3.14159265358979323846

using namespace std::chrono;

/* FPS counting things */
int framesSinceLast = 0;
long lastTimeSecond = 0;
float previousFrame = 0.0f;
float deltaTime = 0.01667f;


/* Camera things*/
glm::vec3 cameraPos = glm::vec3(0.0f, 25.0f, -80.0f);
glm::vec3 cameraFront = glm::vec3(0.f, -1.0f, 3.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec2 mouseAngles = glm::vec2(0, 0);

float horizontal = 3.14f;
float vertical = 0.0f;

float cameraDepth = 10000.f;
float cameraSpeed = 5.f;
float cameraSpeedScale = 100.f;
float moveMult = 5.0f;
float mouseSpeed = 0.0f;

/* Input handlers */
unsigned char keyStates[256] = { 0 };

void keyPressed(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void keyOperations(void);

int mShaderProgram;
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"   TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D sampler;\n"
"void main()\n"
"{\n"
"   vec4 texColor = texture(sampler, TexCoord);\n"
"   FragColor = texColor;\n"
"}\n\0";

/* Cloth things */
Mesh2D* mCloth = nullptr;
const char* mClothTexture = "ghostTexture.png";
float mStartHeight = 30.f;
int mNumRows = 30;
int mNumCols = 25;
float mSpringSize = 2.f; //rest length
glm::vec3 mGravity = glm::vec3(0, -5.f, 0);
float mMass = 1; 
float mK = 550; 
float mKv = 1.5; 
float mKf = 0.6;

void initClothVerts();
void updateVerts();

/* Sphere things */
float mSphereBounceScale = 0.00000f;
float mSphereR = 7;
int mSphereNumVertSlices = 5;
int mSphereNumHorizSlices = 5;
Mesh2D* mSphere = nullptr;
const char* mSphereTexture = "sphereTexture.png";

void initSphereVerts();

/* Background things */
Mesh2D* mBackground = nullptr;
const char* mBackgroundTexture = "backgroundTexture.jpg";
float mBackHeight = 312.5f;
float mBackWidth = 500.f;
void initBackgroundVerts();

/* Display things */
void display();

void initGL();
void initShader();

void reshape(GLsizei width, GLsizei height);


/* Animation things */
void animLoop(int val);


/* Debugging things*/
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        cout << error << " | " << file << " (" << line << ")" << endl;
    }
    return errorCode;
}