#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSL.h"
#include "png++/png.hpp"

#include "../src/PerspectiveCamera.h"
#include "../src/OBJMesh.h"
#include "../src/BlinnPhong.h"
#include "../src/Shader.h"

#include "../renderlib/vec3.h"


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int CheckGLErrors(const char *s)
{
    int errCount = 0;
    return errCount;
}

static vec3 normalize(const vec3& v) {
    float len = std::sqrt(v.x()*v.x() + v.y()*v.y() + v.z()*v.z());
    return { v.x()/len, v.y()/len, v.z()/len };
}

static vec3 midpoint(const vec3& a, const vec3& b) {
    return {(a + b) * 0.5};
}

static std::vector<vec3> createIcosahedron(float r)
{
    const float t = (1.0 + std::sqrt(5.0)) / 2.0;

    std::vector<vec3> v = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };

    for (auto& p : v) {
        vec3 n = normalize(p);
        p = {n.x() * r, n.y() * r, n.z() * r};
    }

    return v;
}

static void subdivide(
    const vec3& v0,
    const vec3& v1,
    const vec3& v2,
    int depth,
    float radius,
    std::vector<float>& out)
{
    if (depth == 0)
    {
        vec3 n0 = normalize(v0);
        vec3 n1 = normalize(v1);
        vec3 n2 = normalize(v2);
        // triangle -> push to VBO
        float u0 = 0.5f - atan2(n0.z(), n0.x()) / (2.0f * M_PI);
        float v0t = 0.5f + asin(n0.y()) / M_PI;

        float u1 = 0.5f - atan2(n1.z(), n1.x()) / (2.0f * M_PI);
        float v1t = 0.5f + asin(n1.y()) / M_PI;

        float u2 = 0.5f - atan2(n2.z(), n2.x()) / (2.0f * M_PI);
        float v2t = 0.5f + asin(n2.y()) / M_PI;

        // 🔧 FIX: handle seam wrapping
        if (fabs(u0 - u1) > 0.5f) {
            if (u0 < 0.5f) u0 += 1.0f;
            else u1 += 1.0f;
        }
        if (fabs(u1 - u2) > 0.5f) {
            if (u1 < 0.5f) u1 += 1.0f;
            else u2 += 1.0f;
        }
        if (fabs(u2 - u0) > 0.5f) {
            if (u2 < 0.5f) u2 += 1.0f;
            else u0 += 1.0f;
        }

        // push vertices (now with UVs)
        out.insert(out.end(), {(float)v0.x(), (float)v0.y(), (float)v0.z(),
                               (float)n0.x(), (float)n0.y(), (float)n0.z(),
                               u0, v0t});

        out.insert(out.end(), {(float)v1.x(), (float)v1.y(), (float)v1.z(),
                               (float)n1.x(), (float)n1.y(), (float)n1.z(),
                               u1, v1t});

        out.insert(out.end(), {(float)v2.x(), (float)v2.y(), (float)v2.z(),
                               (float)n2.x(), (float)n2.y(), (float)n2.z(),
                               u2, v2t});
        return;
    };

    vec3 m0 = normalize(midpoint(v0, v1));
    vec3 m1 = normalize(midpoint(v1, v2));
    vec3 m2 = normalize(midpoint(v2, v0));

    m0 = {m0.x() * radius, m0.y() * radius, m0.z() * radius};
    m1 = {m1.x() * radius, m1.y() * radius, m1.z() * radius};
    m2 = {m2.x() * radius, m2.y() * radius, m2.z() * radius};

    subdivide(v0, m0, m2, depth-1, radius, out);
    subdivide(m0, v1, m1, depth-1, radius, out);
    subdivide(m2, m1, v2, depth-1, radius, out);
    subdivide(m0, m1, m2, depth-1, radius, out);
}

static std::vector<unsigned int> icoIndices = {
    0,11,5,  0,5,1,  0,1,7,  0,7,10, 0,10,11,
    1,5,9,   5,11,4, 11,10,2, 10,7,6, 7,1,8,
    3,9,4,   3,4,2,  3,2,6,  3,6,8,  3,8,9,
    4,9,5,   2,4,11, 6,2,10, 8,6,7,  9,8,1
};

std::vector<float> generateSphere(float radius, int depth)
{
    std::vector<float> buffer;

    auto verts = createIcosahedron(radius);

    for (size_t i = 0; i < icoIndices.size(); i += 3)
    {
        vec3 v0 = verts[icoIndices[i]];
        vec3 v1 = verts[icoIndices[i+1]];
        vec3 v2 = verts[icoIndices[i+2]];

        subdivide(v0, v1, v2, depth, radius, buffer);
    }

    return buffer;
}

struct TextureData {
    int width;
    int height;
    std::vector<float> data; // RGB values in [0, 1]
};

TextureData getTexture(const std::string& texFilename) {
    std::cout << "Reading texture map data from file: " << texFilename << std::endl;

    png::image<png::rgb_pixel> texPNGImage;
    texPNGImage.read(texFilename);
    
    int pngWidth = texPNGImage.get_width();
    int pngHeight = texPNGImage.get_height();
    
    std::vector<float> texData(pngHeight * pngWidth * 3);

    size_t idx = 0;
    for (size_t row = 0; row < pngHeight; ++row) {
        for (size_t col = 0; col < pngWidth; ++col) {
            png::rgb_pixel pixel = texPNGImage[pngHeight - row - 1][col]; // <-- notice the flip of height!!!
            texData[idx++] = pixel.red / 255.0f;
            texData[idx++] = pixel.green / 255.0f;
            texData[idx++] = pixel.blue / 255.0f;
        }
    }

    return {pngWidth, pngHeight, texData};
}

GLuint createTexture(const TextureData &tex) {
    GLuint texID;
    glGenTextures(1, &texID);

    glBindTexture(GL_TEXTURE_2D, texID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                 tex.width, tex.height,
                 0, GL_RGB, GL_FLOAT,
                 tex.data.data());

    glBindTexture(GL_TEXTURE_2D, 0);

    return texID;
}

static float  g_yaw              = -90.0f; // degrees
static float  g_pitch            =   0.0f; // degrees
static float  g_mouseSensitivity =   0.1f; // lower = slower look, raise if too sluggish
static double g_lastMouseX       =   0.0;
static double g_lastMouseY       =   0.0;
static bool   g_firstMouse       = true;   // skip the jump on the very first event
 
static glm::vec3 g_moveUp(0,1,0);

// GLFW cursor-position callback.
static void mouseCallback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    if (g_firstMouse) {
        g_lastMouseX = xpos;
        g_lastMouseY = ypos;
        g_firstMouse = false;
        return;
    }
 
    float dx =  (float)(xpos - g_lastMouseX) * g_mouseSensitivity;
    float dy =  (float)(g_lastMouseY - ypos)  * g_mouseSensitivity; // Y flipped: screen-Y grows downward
    g_lastMouseX = xpos;
    g_lastMouseY = ypos;
 
    g_yaw   += dx;
    g_pitch += dy;
 
    // Hard clamp so we never flip upside-down
    if (g_pitch >  89.0f) g_pitch =  89.0f;
    if (g_pitch < -89.0f) g_pitch = -89.0f;
}


static void updateCameraFromMouse(PerspectiveCamera& cam)
{
    float yawRad   = glm::radians(g_yaw);
    float pitchRad = glm::radians(g_pitch);

    glm::vec3 forward;
    forward.x = std::cos(pitchRad) * std::cos(yawRad);
    forward.y = std::sin(pitchRad);
    forward.z = std::cos(pitchRad) * std::sin(yawRad);
    forward = glm::normalize(forward);

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, worldUp));
    glm::vec3 up    = glm::normalize(glm::cross(right, forward));

    cam.W = vec3(-forward.x, -forward.y, -forward.z);
    cam.U = vec3(right.x, right.y, right.z);
    cam.V = vec3(up.x, up.y, up.z);

} 

static const double METERS_PER_UNIT = 1.496e11 / 29.5;   // ~5.07e9 m per scene unit
static const double V_EARTH_REAL = 29780.0;            // m/s
static const double V_EARTH_SCENE = V_EARTH_REAL / METERS_PER_UNIT; // scene-units/s
static const double R_EARTH_SCENE = 29.5;
static const double GM_SCENE = V_EARTH_SCENE * V_EARTH_SCENE * R_EARTH_SCENE;
static const double TIME_SCALE = 5.0e5;

struct Sphere {
    GLuint textureID;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;

    glm::vec3 velocity;
    float mass;
};

Sphere createSphere(GLuint texID, glm::vec3 position, glm::vec3 scale = glm::vec3(1.0f), glm::vec3 rotation = glm::vec3(0.0f), bool isOrbitting = true) {
    Sphere s;
    s.textureID = texID;
    s.position = position;
    s.scale = scale;
    s.rotation = rotation;
    s.mass = 1.0f;

    if (isOrbitting) {
        float r = glm::length(position);
        float vCirc = (r > 0.001f)
            ? (float)std::sqrt(GM_SCENE / (double)r)
            : 0.0f;
        glm::vec3 dir = glm::normalize(glm::vec3(position.z, 0.0f, -position.x));
        s.velocity = dir * vCirc;
    } else {
        s.velocity = glm::vec3(0.0f);
    }
    return s;
}

void updateOrbits(std::vector<Sphere>& planets,
                  double dt,
                  double timeScale,
                  int    sunIndex = 0)
{
    // Subcycle: split each frame into many small steps so inner
    // planets (Mercury especially) stay on stable circular orbits
    const int   SUBSTEPS = 100;
    double simDt = (dt * timeScale) / SUBSTEPS;

    for (int step = 0; step < SUBSTEPS; ++step)
    {
        for (int i = 0; i < (int)planets.size(); ++i)
        {
            if (i == sunIndex) continue;

            glm::vec3 toSun = glm::vec3(0.0f) - planets[i].position;
            float r = glm::length(toSun);
            if (r < 0.001f) continue;

            glm::vec3 accel0 = (toSun / r) * (float)(GM_SCENE / ((double)r * (double)r));

            planets[i].position += planets[i].velocity * (float)simDt
                                  + accel0 * 0.5f * (float)(simDt * simDt);

            glm::vec3 toSun2 = glm::vec3(0.0f) - planets[i].position;
            float r2 = glm::length(toSun2);
            if (r2 < 0.001f) continue;

            glm::vec3 accel1 = (toSun2 / r2) * (float)(GM_SCENE / ((double)r2 * (double)r2));

            planets[i].velocity += (accel0 + accel1) * 0.5f * (float)simDt;
        }
    }
}

void drawSphere(const Sphere &s, GLuint modelMatrixID, GLuint normalMatrixID, GLuint VAO, int vertexCount) {

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, s.position);
    model = glm::rotate(model, s.rotation.x, glm::vec3(1,0,0));
    model = glm::rotate(model, s.rotation.y, glm::vec3(0,1,0));
    model = glm::rotate(model, s.rotation.z, glm::vec3(0,0,1));
    model = glm::scale(model, s.scale);

    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 normal = glm::transpose(glm::inverse(model));
    glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, glm::value_ptr(normal));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s.textureID);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit()) {
        exit (-1);
    }
    // throw std::runtime_error("Error! initialization of glfw failed!");
    
    /* Create a windowed mode window and its OpenGL context */
    int winWidth = 1000;
    float aspectRatio = 1.0; // 16.0 / 9.0; // winWidth / (float)winHeight;
    int winHeight = winWidth / aspectRatio;
    
    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "GLFW Example", NULL, NULL);
    if (!window) {
        std::cerr << "GLFW did not create a window!" << std::endl;
        
        glfwTerminate();
        return -1;
    }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetCursorPosCallback(window, mouseCallback);
    
    glewExperimental = GL_TRUE;
    GLenum err=glewInit();
    if(err != GLEW_OK) {
        std::cerr <<"GLEW Error! glewInit failed, exiting."<< std::endl;
        exit(EXIT_FAILURE);
    }
    
    TextureData sunTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\sun_texture_8k.png");
    GLuint sunTexID = createTexture(sunTex);

    TextureData mercuryTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\mercury_8k.png");
    GLuint mercuryTexID = createTexture(mercuryTex);

    TextureData venusTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\venus_atmosphere_2k.png");
    GLuint venusTexID = createTexture(venusTex);
    
    TextureData earthTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\earthmap1k.png");
    GLuint earthTexID = createTexture(earthTex);
    
    TextureData marsTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\mars_2k.png");
    GLuint marsTexID = createTexture(marsTex);
    
    TextureData jupiterTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\jupiter_2k.png");
    GLuint jupiterTexID = createTexture(jupiterTex);

    TextureData saturnTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\saturn_8k.png");
    GLuint saturnTexID = createTexture(saturnTex);
    
    TextureData uranusTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\uranus_2k.png");
    GLuint uranusTexID = createTexture(uranusTex);

    TextureData neptuneTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\neptune_2k.png");
    GLuint neptuneTexID = createTexture(neptuneTex);    
    
    TextureData moonTex = getTexture("C:\\Users\\evael\\Documents\\Computer Graphics\\Computer-Graphics\\src\\sceneData\\scenes_B\\images\\moon_2k.png");
    GLuint moonTexID = createTexture(moonTex);

    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0/255.0, 0.0/255.0, 0.0/255.0, 1.0);

    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);
    glViewport(0, 0, fb_width, fb_height);

    // Need to set a projection matrix that fits the aspect ratio set
    // by the window frame.
    //
    // The ortho parameters, in order: left, right, bottom, top, zNear, zFar
    float halfWidth = 15.0 / 2.0;
    float halfHeight = halfWidth; // / aspectRatio;

    float left = -halfWidth;
    float right = halfWidth;

    float bottom = -halfHeight;
    float top = halfHeight;

    float near = 10.0f;
    float far = -10.0f;

    // glm::mat4 projectionMatrix = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -10.0f, 10.0f);
    // glm::mat4 M_ortho = glm::ortho(left, right, bottom, top, near, far);
    float aspect = (float)fb_width / (float)fb_height;
    glm::mat4 perspMat = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 5000.0f);

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;
    printf("%s\n", glfwGetVersionString());

    double timeDiff = 0.0, startFrameTime = 0.0, endFrameTime = 0.0;

    // Bloom Step 1

    GLuint hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    GLuint colorBuffers[2];
    glGenTextures(2, colorBuffers);
    for (unsigned int i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, fb_width, fb_height, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
    }
    // Depth renderbuffer for the scene pass
    GLuint hdrRBO;
    glGenRenderbuffers(1, &hdrRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb_width, fb_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, hdrRBO);

    // Tell OpenGL to write to BOTH colour attachments simultaneously
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "HDR Framebuffer is not complete!" << std::endl;
        exit(EXIT_FAILURE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Bloom step 2
    GLuint pingpongFBO[2];
    GLuint pingpongBuffer[2];
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffer);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                     fb_width/2, fb_height/2, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, pingpongBuffer[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Ping-pong FBO " << i << " is not complete!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Initialize all my data and get it on the GPU.
    // Load the scene file...
    // Get my shapes in the scene...

    // Bloom Screen Quad VAO/VBO
    float quadVertices[] = {
        // positions (NDC)    // tex coords
        -1.0f,  1.0f,         0.0f, 1.0f,
        -1.0f, -1.0f,         0.0f, 0.0f,
         1.0f,  1.0f,         1.0f, 1.0f,
         1.0f, -1.0f,         1.0f, 0.0f,
    };
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    // location 0: vec2 position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // location 1: vec2 texcoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

    GLuint m_triangleVBO[1], m_VAO;
    sivelab::GLSLObject shader;

    // Load a triangle
    // create a Vertex Array Bugger to hold our triangle data

    // create a Vertex Array Buffer to hold our triangle data                                               
    glGenBuffers(1, m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);

    // this is the actual triangle data that will be copied to                                              
    // the GPU memory

    std::vector<float> host_VertexBuffer;

    
    host_VertexBuffer = generateSphere(1.0, 3);

    // host_VertexBuffer = generateSphere(2.0f, 3);
    std::cout << "Vertex count: " << host_VertexBuffer.size() / 8 << std::endl;

    
    int numBytes = host_VertexBuffer.size() * sizeof(float);

    // copy the numBytes from host_VertexBuffer t the GPU and store in                                      
    // the currently bound VBO                                                                              
    glBufferData(GL_ARRAY_BUFFER, numBytes, host_VertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int vertexCount = host_VertexBuffer.size() / 8;

    // once copied, we no longer need the data on the host                                                  
    host_VertexBuffer.clear();

    // VAO for the VBO
    // create a vertex array object that will map the attributes in                                         
    // our vertex buffer to different location attributes for our                                           
    // shaders                                                                                              
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);
    // VAO details here - we only have 1 attribute or location                                              
    // (Position of the vertex)                                                                             
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);

    // Vertex, Previously: Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
    
    // Texture coordinates (u, v)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);
    
    // Create a shader using my GLSLObject class                                                            
    shader.addShader( "OpenGL\\vertexShader_PrepForPerFragment.glsl", sivelab::GLSLObject::VERTEX_SHADER );
    shader.addShader( "OpenGL\\fragmentShader_Lambertian_texture.glsl", sivelab::GLSLObject::FRAGMENT_SHADER );
    shader.createProgram();

    // Lambertian Shader
    GLuint projMatrixID, viewMatrixID, modelMatrixID, lightPosID, normalMatrixID, cameraPosID;
    projMatrixID = shader.createUniform("projMatrix");
    viewMatrixID = shader.createUniform("viewMatrix");
    modelMatrixID = shader.createUniform("modelMatrix");
    normalMatrixID = shader.createUniform("normalMatrix");
    lightPosID = shader.createUniform("lightPosWorld");
    cameraPosID = shader.createUniform("cameraPos");
    GLuint diffuseID = shader.createUniform("diffuseComponent");

    GLuint specularID = shader.createUniform("specularComponent");
    GLuint phongExpID = shader.createUniform("phongExponent");
    
    GLuint texUnitID = shader.createUniform("texImage");
    GLuint emissiveID = shader.createUniform("emissiveStrength");

    // Gaussian Blur Shader
    sivelab::GLSLObject blurShader;
    blurShader.addShader("OpenGL\\vertexShader_quad.glsl", sivelab::GLSLObject::VERTEX_SHADER);
    blurShader.addShader("OpenGL\\fragmentShader_blur.glsl", sivelab::GLSLObject::FRAGMENT_SHADER);
    blurShader.createProgram();

    GLuint blurImageID = blurShader.createUniform("image");
    GLuint blurHorizontalID = blurShader.createUniform("horizontal");

    // Final composite shader
    sivelab::GLSLObject finalShader;
    finalShader.addShader("OpenGL\\vertexShader_quad.glsl", sivelab::GLSLObject::VERTEX_SHADER);
    finalShader.addShader("OpenGL\\fragmentShader_finalComposite.glsl", sivelab::GLSLObject::FRAGMENT_SHADER);
    finalShader.createProgram();

    GLuint finalSceneID = finalShader.createUniform("scene");
    GLuint finalBloomID = finalShader.createUniform("bloomBlur");
    GLuint finalExposureID = finalShader.createUniform("exposure");


    // Set the identity matrix and then set the rotation M = rot * M
    glm::mat4 modelTransform = glm::mat4(1.0);
    float rotAngle = 0.0f;

    // glm::vec3 m_pos(0,0,0), m_viewDir(0,0,-1);
    // glm::vec3 m_U(1,0,0), m_V(0,1,0), m_W(0,0,1);
    //glm::vec4 lightPosWorld(5.0f, 5.0f, 5.0f, 1.0f);
    
    PerspectiveCamera cam(glm::vec3(0.0f,0.0f,100.0f), glm::vec3(0.0f,0.0f,-1.0f), 800, 600, 45.0f);

    std::vector<Sphere> planets;

    planets.push_back(createSphere(sunTexID, {0.0f, 0.0f, 0.0f}, {6.0f, 6.0f, 6.0f}, {0, 0, 0}, false));

    planets.push_back(createSphere(mercuryTexID, {16.0f, 0.0f, 0.0f}, {0.6f, 0.6f, 0.6f}, {0,0,0}, true));
    planets.push_back(createSphere(venusTexID,    {22.0f, 0.0f, 0.0f}, {0.9f, 0.9f, 0.9f}, {0,0,0}, true));
    planets.push_back(createSphere(earthTexID,    {29.5f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0,0,0}, true));
    planets.push_back(createSphere(marsTexID,     {36.0f, 0.0f, 0.0f}, {0.7f, 0.7f, 0.7f}, {0,0,0}, true));

    planets.push_back(createSphere(jupiterTexID,  {52.0f, 0.0f, 0.0f}, {3.5f, 3.5f, 3.5f}, {0,0,0}, true));
    planets.push_back(createSphere(saturnTexID,   {68.0f, 0.0f, 0.0f}, {3.0f, 3.0f, 3.0f}, {0,0,0}, true));
    planets.push_back(createSphere(uranusTexID,   {84.0f, 0.0f, 0.0f}, {2.2f, 2.2f, 2.2f}, {0,0,0}, true));
    planets.push_back(createSphere(neptuneTexID,  {100.0f, 0.0f, 0.0f}, {2.1f, 2.1f, 2.1f}, {0,0,0}, true));
    //planets.push_back(createSphere(moonTexID, {5.0f, 0.0f, -6.0f}, {0.27f, 0.27f, 0.27f}, {0, 0, 0}));


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        endFrameTime = glfwGetTime();
        timeDiff = endFrameTime - startFrameTime;
        startFrameTime = glfwGetTime();

        updateCameraFromMouse(cam);

        updateOrbits(planets, timeDiff, TIME_SCALE, /*sunIndex=*/0);
        
        // Clear the window's buffer (or clear the screen to our
        // background color)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Calculate the model
        // glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-2));
        // glm::mat4 view = glm::mat4(1.0f);
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1.33f, 0.1f, 100.0f);
        // glm::mat4 mvp = projection * view * model;
        
        // create the view matrix from our camera data 
                                                                                                          
        glm::mat4 M_view = cam.getViewMatrix();
        glm::mat4 M_proj = cam.getProjectionMatrix((float)fb_width / (float)fb_height);
        
        // PASS 1
        glEnable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* Render your objects here */
        shader.activate();
        glUniform3f(cameraPosID, cam.position.x(), cam.position.y(), cam.position.z());

        glUniform1i(texUnitID, 0);
        
        // copy from the host to the device the view matrix and the projection matrix                                                                                       
        // glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr( M_ortho ));
        glUniformMatrix4fv(projMatrixID , 1, GL_FALSE , glm:: value_ptr( M_proj ));
        glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelTransform));
        
        // // =========================
        // // OBJECT 1: EARTH
        // // =========================
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, sunTexID);   // earth texture
        // glUniform1i(texUnitID, 0);
        // glUniform1f(emissiveID, 2.0f);

        // glm::mat4 modelEarth = glm::mat4(1.0f);
        // modelEarth = glm::translate(modelEarth, glm::vec3(-2.0f, 0.0f, -6.0f));
        // modelEarth = glm::rotate(modelEarth, rotAngle, glm::vec3(0, 1, 0));

        // glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelEarth));

        // glm::mat4 normalEarth = glm::transpose(glm::inverse(modelEarth));
        // glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, glm::value_ptr(normalEarth));

        //glUniform4fv(lightPosID, 1, glm::value_ptr(lightPosWorld));

        // glUniform3fv(diffuseID, 1,
        //     glm::value_ptr(glm::vec3(94.0/255.0f, 235.0/255.0f, 52.0/255.0f)));

        // glUniform3fv(specularID, 1,
        //     glm::value_ptr(glm::vec3(1.0f)));

        // glUniform1f(phongExpID, 32.0f);

        // glBindVertexArray(m_VAO);
        // glDrawArrays(GL_TRIANGLES, 0, vertexCount);


        // // =========================
        // // OBJECT 2: MOON (different texture)
        // // =========================
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, moonTexID);   // <-- second texture
        // glUniform1i(texUnitID, 0);

        // glUniform1f(emissiveID, 1.5f);

        // glm::mat4 modelMoon = glm::mat4(1.0f);
        // modelMoon = glm::translate(modelMoon, glm::vec3(2.5f, 0.0f, -6.0f));
        // modelMoon = glm::rotate(modelMoon, rotAngle * 1.5f, glm::vec3(0, 1, 0));
        // modelMoon = glm::scale(modelMoon, glm::vec3(0.27f, 0.27f, 1.0f));

        // glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelMoon));

        // glm::mat4 normalMoon = glm::transpose(glm::inverse(modelMoon));
        // glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, glm::value_ptr(normalMoon));

        // glDrawArrays(GL_TRIANGLES, 0, vertexCount);

        for (auto &p : planets)
        {
            //p.rotation.y += timeDiff;   // spin

            glUniform1f(emissiveID, (p.textureID == sunTexID || p.textureID == moonTexID) ? 2.0f : 0.0f);

            drawSphere(p,
                    modelMatrixID,
                    normalMatrixID,
                    m_VAO,
                    vertexCount);
        }
        glBindVertexArray(0);



        shader.deactivate();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Pass 2a - Extract bright feature (Pass 1)
        // Pass 2b - blur acticate!

        glDisable(GL_DEPTH_TEST);

        bool horizontal = true;
        bool first_iterations = true;
        const int blurAmount = 25;

        blurShader.activate();

        for (unsigned int i = 0; i < blurAmount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
            glViewport(0, 0, fb_width / 2, fb_height / 2);
            glClear(GL_COLOR_BUFFER_BIT);
            glUniform1i(blurHorizontalID, horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iterations ? colorBuffers[1]
                : pingpongBuffer[!horizontal]);
            glUniform1i(blurImageID, 0);

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);

            horizontal = !horizontal;
            first_iterations = false;
        }
        blurShader.deactivate();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // PASS 2c - Final Composite
        glViewport(0, 0, fb_width, fb_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        finalShader.activate();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
        glUniform1i(finalSceneID, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
        glUniform1i(finalBloomID, 1);

        glUniform1f(finalExposureID, 1.0f);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
        
        finalShader.deactivate();


        // Swap the front and back buffers
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        float moveRatePerFrame = 0.05;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cam.position = cam.position - vec3(cam.W.x(), cam.W.y(), cam.W.z()) * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cam.position = cam.position + vec3(cam.W.x(), cam.W.y(), cam.W.z()) * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cam.position = cam.position - vec3(cam.U.x(), cam.U.y(), cam.U.z()) * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cam.position = cam.position + vec3(cam.U.x(), cam.U.y(), cam.U.z()) * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            cam.position = cam.position + vec3(0, 1, 0) * moveRatePerFrame;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cam.position = cam.position - vec3(0, 1, 0) * moveRatePerFrame;
        }

        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        std::cout << "fps: " << 1.0 / timeDiff << std::endl;
        }
        if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, 1);
        }
    }
  
    glfwTerminate();
    return 0;
}
