#include <cstdlib>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GLSL.h"

#include "../src/PerspectiveCamera.h"
#include "../src/OBJMesh.h"
#include "../src/BlinnPhong.h"
#include "../src/Shader.h"

int CheckGLErrors(const char *s)
{
    int errCount = 0;
    return errCount;
}

void loadOBJ(const std::string &filename, std::vector<float> &outBuffer)
{
    std::ifstream file(filename);
    std::string line;

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            ss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "f") {
            for (int i = 0; i < 3; ++i) {
                std::string vertexStr;
                ss >> vertexStr;

                int vIndex, nIndex;
                sscanf(vertexStr.c_str(), "%d//%d", &vIndex, &nIndex);
                
                glm::vec3 vert = vertices[vIndex - 1];
                glm::vec3 norm = normals[nIndex - 1];
  
                outBuffer.push_back(vert.x);
                outBuffer.push_back(vert.y);
                outBuffer.push_back(vert.z);

                outBuffer.push_back(norm.x);
                outBuffer.push_back(norm.y);
                outBuffer.push_back(norm.z);
            }
        }
    }
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit()) {
        exit (-1);
    }
    // throw std::runtime_error("Error! initialization of glfw failed!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    glewExperimental = GL_TRUE;
    GLenum err=glewInit();
    if(err != GLEW_OK) {
        std::cerr <<"GLEW Error! glewInit failed, exiting."<< std::endl;
        exit(EXIT_FAILURE);
    }

    const GLubyte* renderer = glGetString (GL_RENDERER);
    const GLubyte* version = glGetString (GL_VERSION);
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(179.0/255.0, 179.0/255.0, 179.0/255.0, 1.0);

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
    glm::mat4 perspMat = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;
    printf("%s\n", glfwGetVersionString());

    double timeDiff = 0.0, startFrameTime = 0.0, endFrameTime = 0.0;

    // Initialize all my data and get it on the GPU.
    // Load the scene file...
    // Get my shapes in the scene...

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
    BlinnPhong* defaultShader = new BlinnPhong(color(230.0/255.0, 126.0/255.0, 219.0/255.0), 64.0f);
    OBJMesh myMesh("C:\\Users\\evael\\Documents\\Computer Graphics\\starterCode\\src\\sceneData\\scenes_B\\objFiles\\al.obj", defaultShader);
    host_VertexBuffer = myMesh.getVertexBuffer();
    std::cout << "Vertex count: " << myMesh.getVertexBuffer().size() << std::endl;

    // std::vector< float > host_VertexBuffer{ //vertex, normal
    //                                         -3.0f, -3.0f, 0.0f, 0.0f, 0.0f, 1.0f, //255.0/255.0f, 172.0/255.0f, 227.0/255.0f,
    //                                         3.0f, -3.0f, 0.0f, 0.0f, 0.0f, 1.0f,//117.0/255.0f, 122.0/255.0f, 255.0/255.0f,
    //                                         0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 1.0f,//129.0/255.0f, 255.0/255.0f, 117.0/255.0f
    //                                     };

    // //                                        -0.8f, -0.2f, 0.0f /* v0 */, 255.0/255.0f, 172.0/255.0f, 227.0/255.0f, // color 0                             
    // //                                         0.2f, -0.7f, 0.0f /* v1 */, 117.0/255.0f, 122.0/255.0f, 255.0/255.0f, // color 1
    // //                                         0.6f, 0.6f, 0.0f /* v2 */, 129.0/255.0f, 255.0/255.0f, 117.0/255.0f, // color 2


    //                                         // 0.6f, -0.5f, 0.0f /* v0 */, 255.0/255.0f, 172.0/255.0f, 227.0/255.0f, // color 0                             
    //                                         // 0.8f, -0.7f, 0.0f /* v1 */, 117.0/255.0f, 122.0/255.0f, 255.0/255.0f, // color 1
    //                                         // 1.0f, 0.6f, 0.0f /* v2 */, 129.0/255.0f, 255.0/255.0f, 117.0/255.0f // color 2
    //                                     // };       
                                        
    // std::vector<float> allMYVVerts;

    // for (all triangle in shape) {
    //     for (all vertices in my triangle) {
    //     allMYVerts.push_back(vert.x);
    //     allMYVerts.push_back(vert.y);
    //     allMYVerts.push_back(vert.z);
    // }

    int numBytes = host_VertexBuffer.size() * sizeof(float);

    // copy the numBytes from host_VertexBuffer t the GPU and store in                                      
    // the currently bound VBO                                                                              
    glBufferData(GL_ARRAY_BUFFER, numBytes, host_VertexBuffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    int vertexCount = host_VertexBuffer.size() / 6;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

    // Vertex, Previously: Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
    
    // Create a shader using my GLSLObject class                                                            
    shader.addShader( "OpenGL\\vertexShader_PrepForPerFragment.glsl", sivelab::GLSLObject::VERTEX_SHADER );
    shader.addShader( "OpenGL\\fragmentShader_BlinnPhong.glsl", sivelab::GLSLObject::FRAGMENT_SHADER );
    shader.createProgram();

    GLuint projMatrixID, viewMatrixID, modelMatrixID, lightPosID, normalMatrixID;
    projMatrixID = shader.createUniform("projMatrix");
    viewMatrixID = shader.createUniform("viewMatrix");
    modelMatrixID = shader.createUniform("modelMatrix");
    normalMatrixID = shader.createUniform("normalMatrix");
    lightPosID = shader.createUniform("lightPosWorld");
    GLuint diffuseID = shader.createUniform("diffuseComponent");

    GLuint specularID = shader.createUniform("specularComponent");
    GLuint phongExpID = shader.createUniform("phongExponent");
    
    
    // Set the identity matrix and then set the rotation M = rot * M
    glm::mat4 modelTransform = glm::mat4 (1.0);
    float rotAngle = 0.0f;
    modelTransform = glm::rotate(modelTransform , rotAngle , glm::vec3(0, 1, 0));
    
    // glm::vec3 m_pos(0,0,0), m_viewDir(0,0,-1);
    // glm::vec3 m_U(1,0,0), m_V(0,1,0), m_W(0,0,1);
    glm::vec4 lightPosWorld(5.0f, 5.0f, 5.0f, 1.0f);
    
    PerspectiveCamera cam(glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,-1.0f), 800, 600, 45.0f);
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        endFrameTime = glfwGetTime();
        timeDiff = endFrameTime - startFrameTime;
        startFrameTime = glfwGetTime();
        
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
        glm::mat4 M_proj = cam.getProjectionMatrix();
        
        /* Render your objects here */
        shader.activate();
        glm::mat4 modelTransform = glm::mat4 (1.0);
        
        modelTransform = glm::rotate(modelTransform, rotAngle, glm::vec3(0, 1, 1));
        
        rotAngle += 0.0001f;
        if (rotAngle > 2.0 * 3.14159f) {
            rotAngle = 0.0f;
        }
        
        
        // copy from the host to the device the view matrix and the projection matrix                                                                                       
        // glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr( M_ortho ));
        glUniformMatrix4fv(projMatrixID , 1, GL_FALSE , glm:: value_ptr( M_proj ));
        glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelTransform));
        
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelTransform));
        glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform4fv(lightPosID, 1, glm::value_ptr(lightPosWorld));
        
        glm::vec3 diffuseColor(117.0/255.0f, 122.0/255.0f, 255.0/255.0f);
        glUniform3fv(diffuseID, 1, glm::value_ptr(diffuseColor));

        glm::vec3 specularColor(1.0f, 1.0f, 1.0f);
        float phongExponent = 32.0f;

        glUniform3fv(specularID, 1, glm::value_ptr(specularColor));
        glUniform1f(phongExpID, phongExponent);

        glBindVertexArray(m_VAO);

        glDrawArrays(GL_TRIANGLES, 0, myMesh.getVertexBuffer().size() / 6);
        glBindVertexArray(0);

        shader.deactivate();
        // Swap the front and back buffers
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        float moveRatePerFrame = 0.005;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cam.position = cam.position - cam.W * moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cam.position = cam.position - cam.U * moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cam.position = cam.position + cam.W * moveRatePerFrame;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cam.position = cam.position + cam.U * moveRatePerFrame;
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
