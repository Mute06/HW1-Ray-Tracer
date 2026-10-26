#include "scene.h"
#include "ppm.h"
#include <iostream>
#include "math.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./raytracer <scene.json>\n";
        return 1;
    }

    try {
        Scene scene;
        loadScene(argv[1], scene);

        std::cout << "Parsed " << argv[1] << " successfully:\n"
                  << "  ShadowRayEpsilon:  " << scene.shadowRayEpsilon << "\n"
                  << "  MaxRecursionDepth: " << scene.maxRecursionDepth << "\n"
                  << "  Cameras:           " << scene.cameras.size() << "\n"
                  << "  PointLights:       " << scene.pointLights.size() << "\n"
                  << "  Materials:         " << scene.materials.size() << "\n"
                  << "  Vertices:          " << scene.vertices.size() << "\n"
                  << "  Meshes:            " << scene.meshes.size() << " (faces in mesh 0: "
                  << (scene.meshes.empty() ? 0 : scene.meshes[0].faces.size()) << ")\n"
                  << "  Triangles:         " << scene.triangles.size() << "\n"
                  << "  Spheres:           " << scene.spheres.size() << "\n";

        for (const auto& cam : scene.cameras) {
            std::vector<Vec3> pixels(cam.imageWidth * cam.imageHeight, scene.backgroundColor);
            writePPM(cam.imageName, cam.imageWidth, cam.imageHeight, pixels);
            std::cout << "Wrote " << cam.imageName << " (" << cam.imageWidth << "x" << cam.imageHeight << ")\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}