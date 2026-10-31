#include "scene.h"
#include "ppm.h"
#include <iostream>
#include <chrono>
#include "math.h"
#include "Camera.h"
#include "intersect.h"
#include "shader.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./raytracer <scene.json>\n";
        return 1;
    }

    auto start = std::chrono::steady_clock::now();

    try {
        Scene scene;
        loadScene(argv[1], scene);

        for (const Camera& cam : scene.cameras) {
            std::vector<Vec3> pixels(cam.ImageWidth * cam.ImageHeight);

            for (int j = 0; j < cam.ImageHeight; j++) {
                for (int i = 0; i < cam.ImageWidth; i++) {
                    Ray ray = cam.GenerateRay(i,j);

                    Vec3 color = computeColor(ray,scene);

                    pixels[j * cam.ImageWidth + i] = color;
                }
            }

            writePPM(cam.ImageName, cam.ImageWidth, cam.ImageHeight, pixels);
            std::cout << "Rendered " << cam.ImageName << " successfully.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    auto end = std::chrono::steady_clock::now();

    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Execution time: " << elapsed.count() << " ms\n";

    return 0;
}
