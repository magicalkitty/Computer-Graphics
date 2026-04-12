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

#include "../renderlib/vec3.h"

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
        out.insert(out.end(), {(float)v0.x(), (float)v0.y(), (float)v0.z(), (float)n0.x(), (float)n0.y(), (float)n0.z()});
        out.insert(out.end(), {(float)v1.x(), (float)v1.y(), (float)v1.z(), (float)n1.x(), (float)n1.y(), (float)n1.z()});
        out.insert(out.end(), {(float)v2.x(), (float)v2.y(), (float)v2.z(), (float)n2.x(), (float)n2.y(), (float)n2.z()});
        return;
    }

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
    float aspect = (float)fb_width / (float)fb_height;
    
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
    glm::mat4 perspMat = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    GLint major_version;
    glGetIntegerv(GL_MAJOR_VERSION, &major_version);
    std::cout << "GL_MAJOR_VERSION: " << major_version << std::endl;
    printf("%s\n", glfwGetVersionString());

    double timeDiff = 0.0, startFrameTime = 0.0, endFrameTime = 0.0;

    // Initialize all my data and get it on the GPU.
    // Load the scene file...
    // Get my shapes in the scene...

    GLuint m_triangleVBO[1], m_VAO;
    GLuint sphereVBO, sphereVAO;
    GLuint objVBO, objVAO;
    sivelab::GLSLObject lambertianShader, blinnShader, shader;

    // Load a triangle
    // create a Vertex Array Bugger to hold our triangle data

    // create a Vertex Array Buffer to hold our triangle data                                               
    glGenBuffers(1, m_triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_triangleVBO[0]);


    // this is the actual triangle data that will be copied to                                              
    // the GPU memory

    std::vector<float> host_VertexBuffer;
    BlinnPhong* defaultShader = new BlinnPhong(color(230.0/255.0, 126.0/255.0, 219.0/255.0), 64.0f);
    // OBJMesh myMesh("C:\\Users\\evael\\Documents\\Computer Graphics\\starterCode\\src\\sceneData\\rico\\source\\Rico\\rico.obj", defaultShader);
    // host_VertexBuffer = myMesh.getVertexBuffer();

    host_VertexBuffer = generateSphere(2.0f, 3);
    std::cout << "Vertex count: " << host_VertexBuffer.size() / 6 << std::endl;

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

    // --------- SPHERE MESH LOADING ---------
    std::vector<float> sphereBuffer = generateSphere(2.0f, 3);
    int sphereVertexCount = sphereBuffer.size() / 6;

    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereBuffer.size() * sizeof(float), sphereBuffer.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // --------- OBJ MESH LOADING ---------
    BlinnPhong* shaderMat = new BlinnPhong(color(230.0/255.0, 126.0/255.0, 219.0/255.0), 64.0f);
    OBJMesh myMesh("C:\\Users\\evael\\Documents\\Computer Graphics\\starterCode\\src\\sceneData\\rico\\source\\Rico\\rico.obj", shaderMat);
    std::vector<float> objBuffer = myMesh.getVertexBuffer();
    int objVertexCount = objBuffer.size() / 6;

    glGenBuffers(1, &objVBO);
    glBindBuffer(GL_ARRAY_BUFFER, objVBO);
    glBufferData(GL_ARRAY_BUFFER, objBuffer.size() * sizeof(float), objBuffer.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &objVAO);
    glBindVertexArray(objVAO);
    glBindBuffer(GL_ARRAY_BUFFER, objVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindVertexArray(0);
    
    // Create a shader using my GLSLObject class

    shader.addShader( "OpenGL\\vertexShader_normal.glsl", sivelab::GLSLObject::VERTEX_SHADER );
    shader.addShader( "OpenGL\\fragmentShader_normal.glsl", sivelab::GLSLObject::FRAGMENT_SHADER );
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
    

    lambertianShader.addShader( "OpenGL\\vertexShader_PrepForPerFragment.glsl", sivelab::GLSLObject::VERTEX_SHADER );
    lambertianShader.addShader( "OpenGL\\fragmentShader_Lambertian.glsl", sivelab::GLSLObject::FRAGMENT_SHADER );
    lambertianShader.createProgram();

    GLuint lambertianProjMatrixID = lambertianShader.createUniform("projMatrix");
    GLuint lambertianViewMatrixID = lambertianShader.createUniform("viewMatrix");
    GLuint lambertianModelMatrixID = lambertianShader.createUniform("modelMatrix");
    GLuint lambertianNormalMatrixID = lambertianShader.createUniform("normalMatrix");
    GLuint lambertianLightPosID = lambertianShader.createUniform("lightPosWorld");
    GLuint lambertianDiffuseID = lambertianShader.createUniform("diffuseComponent");
    GLuint lambertianCameraPosID = lambertianShader.createUniform("cameraPos");

    blinnShader.addShader( "OpenGL\\vertexShader_PrepForPerFragment.glsl", sivelab::GLSLObject::VERTEX_SHADER );
    blinnShader.addShader( "OpenGL\\fragmentShader_BlinnPhong.glsl", sivelab::GLSLObject::FRAGMENT_SHADER );
    blinnShader.createProgram();

    GLuint blinnProjMatrixID = blinnShader.createUniform("projMatrix");
    GLuint blinnViewMatrixID = blinnShader.createUniform("viewMatrix");
    GLuint blinnModelMatrixID = blinnShader.createUniform("modelMatrix");
    GLuint blinnNormalMatrixID = blinnShader.createUniform("normalMatrix");
    GLuint blinnLightPosID = blinnShader.createUniform("lightPosWorld");
    GLuint blinnDiffuseID = blinnShader.createUniform("diffuseComponent");
    GLuint blinnSpecularID = blinnShader.createUniform("specularComponent");
    GLuint blinnPhongExpID = blinnShader.createUniform("phongExponent");
    GLuint blinnCameraPosID = blinnShader.createUniform("cameraPos");

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
        glm::mat4 M_proj = cam.getProjectionMatrix(aspect);
        
        /* Render your objects here */
        // shader.activate();
        // glm::mat4 modelTransform = glm::mat4 (1.0);
        
        // modelTransform = glm::rotate(modelTransform, rotAngle, glm::vec3(0, 1, 1));
        
        // rotAngle += 0.0001f;
        // if (rotAngle > 2.0 * 3.14159f) {
        //     rotAngle = 0.0f;
        // }
        
        
        // // copy from the host to the device the view matrix and the projection matrix                                                                                       
        // // glUniformMatrix4fv(projMatrixID, 1, GL_FALSE, glm::value_ptr( M_ortho ));
        // glUniformMatrix4fv(projMatrixID , 1, GL_FALSE , glm:: value_ptr( M_proj ));
        // glUniformMatrix4fv(viewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        // glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, glm::value_ptr(modelTransform));
        
        // glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelTransform));
        // glUniformMatrix4fv(normalMatrixID, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        // glUniform4fv(lightPosID, 1, glm::value_ptr(lightPosWorld));
        
        // glm::vec3 diffuseColor(117.0/255.0f, 122.0/255.0f, 255.0/255.0f);
        // glUniform3fv(diffuseID, 1, glm::value_ptr(diffuseColor));

        // glm::vec3 specularColor(1.0f, 1.0f, 1.0f);
        // float phongExponent = 32.0f;

        // glUniform3fv(specularID, 1, glm::value_ptr(specularColor));
        // glUniform1f(phongExpID, phongExponent);

        // glBindVertexArray(m_VAO);

        // glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        // glBindVertexArray(0);

        // shader.deactivate();

        lambertianShader.activate();
        glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f),
                                       glm::vec3(-3.0f, 0.0f, 0.0f));

        glUniformMatrix4fv(lambertianProjMatrixID , 1, GL_FALSE , glm:: value_ptr( M_proj ));
        glUniformMatrix4fv(lambertianViewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        glUniformMatrix4fv(lambertianModelMatrixID, 1, GL_FALSE, glm::value_ptr(sphereModel));

        glm::mat4 SpherenormalMatrix = glm::transpose(glm::inverse(sphereModel));
        glUniformMatrix4fv(lambertianNormalMatrixID, 1, GL_FALSE, glm::value_ptr(SpherenormalMatrix));
        glUniform4fv(lambertianLightPosID, 1, glm::value_ptr(lightPosWorld));
        glm::vec3 lambertianDiffuseColor(117.0/255.0f, 122.0/255.0f, 255.0/255.0f);
        glUniform3fv(lambertianDiffuseID, 1, glm::value_ptr(lambertianDiffuseColor));
        glUniform3f(
            lambertianCameraPosID,
            cam.position.x(),
            cam.position.y(),
            cam.position.z()
        );
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, sphereVertexCount);
        glBindVertexArray(0);

        lambertianShader.deactivate();

        blinnShader.activate();
        rotAngle += 0.0001f;
        if (rotAngle > 2.0 * 3.14159f) {
            rotAngle = 0.0f;
        }

        glm::mat4 objModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        objModel = glm::scale(objModel, glm::vec3(5.0f, 5.0f, 5.0f));
        objModel = glm::rotate(objModel, rotAngle, glm::vec3(0, 1, 1));
        glUniformMatrix4fv(blinnProjMatrixID , 1, GL_FALSE , glm:: value_ptr( M_proj ));
        glUniformMatrix4fv(blinnViewMatrixID, 1, GL_FALSE, glm::value_ptr( M_view ));
        glUniformMatrix4fv(blinnModelMatrixID, 1, GL_FALSE, glm::value_ptr(objModel));

        glm::mat4 ObjnormalMatrix = glm::transpose(glm::inverse(objModel));
        glUniformMatrix4fv(blinnNormalMatrixID, 1, GL_FALSE, glm::value_ptr(ObjnormalMatrix));
        glUniform4fv(blinnLightPosID, 1, glm::value_ptr(lightPosWorld));
        glm::vec3 blinnDiffuseColor(183.0/255.0f, 122.0/255.0f, 240.0/255.0f);
        glUniform3fv(blinnDiffuseID, 1, glm::value_ptr(blinnDiffuseColor));

        glUniform3f(
            blinnCameraPosID,
            cam.position.x(),
            cam.position.y(),
            cam.position.z()
        );

        glm::vec3 specularColor(1.0f, 1.0f, 1.0f);
        float phongExponent = 32.0f;

        glUniform3fv(blinnSpecularID, 1, glm::value_ptr(specularColor));
        glUniform1f(blinnPhongExpID, phongExponent);



        glBindVertexArray(objVAO);
        glDrawArrays(GL_TRIANGLES, 0, objVertexCount);
        glBindVertexArray(0);
        blinnShader.deactivate();

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
