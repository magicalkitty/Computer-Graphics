#include <iostream>
#include <fstream>
#include <vector>
#include "../PerspectiveCamera.h"
#include "../Framebuffer.h"
#include "../Scene.h"
#include "../triangle.h"
#include "../Lambertian.h"
#include "../BlinnPhong.h"
#include "../../renderlib/vec3.h"
#include <chrono>

#ifdef _OPENMP
#include <omp.h>
#endif

void readFloatsFromFile(const std::string& filename, std::vector<float> &allFloats)
{
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open file\n";
        return;
    }

    float value;
    char c;

    while (file >> value) {
        allFloats.push_back(value);

        // Consume comma if there
        if (file.peek() == ',') file.get(c);
    }
}

int main(int argc, char* argv[])
{
    int nx = 600;
    int ny = 600;
    Framebuffer fb(nx, ny);
    
    PerspectiveCamera cam(vec3(0.0, 0.0, 5.0), nx, ny, 1.0f);
    
    Scene world;

    Light light(vec3(20.0, 10.0, 5.0), vec3(1.0, 1.0, 1.0));

    world.clear();

    std::vector<float> allFloats;
    readFloatsFromFile( "C:\\Users\\evael\\Documents\\Computer Graphics\\starterCode\\src\\triangleChallenge\\trilist.dat", allFloats );
    
    std::cout << "Number of floats: " << allFloats.size() << std::endl;
    int numVertices = allFloats.size() / 3;
    std::cout << "Interpreted as Vertices: " << numVertices << std::endl;
    int numTriangles = numVertices / 3;
    std::cout << "Interpreted as Triangles: " << numTriangles << std::endl;
    std::shared_ptr<Shader> lambertShaderTri = std::make_shared<BlinnPhong>(color(245.0/255.0, 147.0/255.0, 227.0/255.0));

    for (int i = 0; i + 8 < allFloats.size(); i += 9) {
        
        vec3 v1(allFloats[i], allFloats[i+1], allFloats[i+2]);
        vec3 v2(allFloats[i+3], allFloats[i+4], allFloats[i+5]);
        vec3 v3(allFloats[i+6], allFloats[i+7], allFloats[i+8]);
        
        world.add(std::make_shared<Triangle>(v1, v2, v3, lambertShaderTri));
        
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    #pragma omp parallel for schedule(dynamic)
    for (int x=0; x<fb.get_width(); ++x) {
        for (int y=0; y<fb.get_height(); ++y) {
            ray r;
            cam.generateRay(x, y, r);

            color c = world.computeRayColor(r, 1.0, INFINITY, light, 10);

            fb.setPixelColor(x, y, c);
        }
        printf("Yes, I am still goin <3, x = %d\n", x);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSeconds = endTime - startTime;
    std::cout << "Render time: " << elapsedSeconds.count() << " seconds\n";
    // fb.exportToPNG("triangleChallenge_output");
    fb.exportToPNG("triangleChallenge_output_omp");
}
