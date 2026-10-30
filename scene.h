#ifndef SCENE_H
#define SCENE_H

#include "math.h"
#include "Camera.h"
#include <string>
#include <vector>



struct PointLight {
    int id = 0;
    Vec3 position;
    Vec3 intensity;
};

struct Material {
    int id = 0;
    bool isMirror = false;
    Vec3 ambientReflectance;
    Vec3 diffuseReflectance;
    Vec3 specularReflectance;
    Vec3 mirrorReflectance;
    double phongExponent = 0.0;
};

struct Triangle {
    int materialId = 0; // 1-based ID as given in JSON (or subtract 1 when indexing)
    int v0 = 0;         // 1-based vertex IDs as given in JSON
    int v1 = 0;
    int v2 = 0;
};

struct Mesh {
    int materialId = 0;
    std::vector<Triangle> faces;
};

struct Sphere {
    int materialId = 0;
    int centerVertexId = 0;
    double radius = 0.0;

    Vec3 center;
};

struct Plane {
    int materialId = 0;
    int centerVertexId = 0;
    Vec3 normal;
    Vec3 point;
};

struct Cylinder {
    int materialId = 0;
    int centerVertexId = 0;
    Vec3 axis;
    double radius = 0.0;
    double height = 0.0;
};

struct Scene {
    Vec3 backgroundColor;
    double shadowRayEpsilon = 0.0;
    int maxRecursionDepth = 0;

    std::vector<Camera> cameras;
    Vec3 ambientLight;
    std::vector<PointLight> pointLights;
    std::vector<Material> materials;
    std::vector<Vec3> vertices;

    std::vector<Mesh> meshes;
    std::vector<Triangle> triangles;
    std::vector<Sphere> spheres;
    std::vector<Plane> planes;
    std::vector<Cylinder> cylinders;
};

void loadScene(const std::string& filename, Scene& scene);

#endif // SCENE_H