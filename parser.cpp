#include "scene.h"
#include "json.hpp"
#include "Camera.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

using json = nlohmann::json;

namespace {

// Spec: "If there is more than one element of the same kind, its value is an array of objects.
// If there is only one, its value may be a single object instead of an array."
json asArray(const json& node) {
    if (node.is_array()) {
        return node;
    }
    if (node.is_object()) {
        return json::array({node});
    }
    return json::array();
}

Vec3 parseVec3(const std::string& str) {
    Vec3 v;
    std::stringstream ss(str);
    ss >> v.x >> v.y >> v.z;
    return v;
}

double parseDouble(const std::string& str) {
    double val = 0.0;
    std::stringstream ss(str);
    ss >> val;
    return val;
}

int parseInt(const std::string& str) {
    int val = 0;
    std::stringstream ss(str);
    ss >> val;
    return val;
}

} // namespace

void loadScene(const std::string& filename, Scene& scene) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open scene file: " + filename);
    }

    json root;
    file >> root;
    const json& s = root.at("Scene");

    if (s.contains("BackgroundColor")) {
        scene.backgroundColor = parseVec3(s["BackgroundColor"].get<std::string>());
    }
    if (s.contains("ShadowRayEpsilon")) {
        scene.shadowRayEpsilon = parseDouble(s["ShadowRayEpsilon"].get<std::string>());
    }
    if (s.contains("MaxRecursionDepth")) {
        scene.maxRecursionDepth = parseInt(s["MaxRecursionDepth"].get<std::string>());
    }

    // Cameras
    // Cameras
    if (s.contains("Cameras") && s["Cameras"].contains("Camera")) {
        for (const auto& camNode : asArray(s["Cameras"]["Camera"])) {
            Camera cam;
            cam.id = parseInt(camNode.at("_id").get<std::string>());
            cam.Position = parseVec3(camNode.at("Position").get<std::string>());
            cam.Gaze = parseVec3(camNode.at("Gaze").get<std::string>());
            cam.Up = parseVec3(camNode.at("Up").get<std::string>());

            // Handle "NearPlane" or "Near Plane"
            std::string nearPlaneStr = camNode.contains("NearPlane")
                ? camNode["NearPlane"].get<std::string>()
                : camNode.at("Near Plane").get<std::string>();
            std::stringstream np(nearPlaneStr);
            np >> cam.NearPlaneLeft >> cam.NearPlaneRight >> cam.NearPlaneBottom >> cam.NearPlaneTop;

            // Handle "NearDistance" or "Near Distance"
            std::string nearDistStr = camNode.contains("NearDistance")
                ? camNode["NearDistance"].get<std::string>()
                : camNode.at("Near Distance").get<std::string>();
            cam.NearPlaneDistance = parseDouble(nearDistStr);

            // Handle "ImageResolution" or "Image Resolution"
            std::string imgResStr = camNode.contains("ImageResolution")
                ? camNode["ImageResolution"].get<std::string>()
                : camNode.at("Image Resolution").get<std::string>();
            std::stringstream res(imgResStr);
            res >> cam.ImageWidth >> cam.ImageHeight;

            // Handle "ImageName" or "Image Name"
            cam.ImageName = camNode.contains("ImageName")
                ? camNode["ImageName"].get<std::string>()
                : camNode.at("Image Name").get<std::string>();

            // Pre-calculate u, v, w, q, Pixel_Width, and Pixel_Height
            cam.ComputeBasis();

            scene.cameras.push_back(cam);
        }
    }

    // Lights
    if (s.contains("Lights")) {
        const json& lights = s["Lights"];
        if (lights.contains("AmbientLight")) {
            scene.ambientLight = parseVec3(lights["AmbientLight"].get<std::string>());
        }
        if (lights.contains("PointLight")) {
            for (const auto& plNode : asArray(lights["PointLight"])) {
                PointLight pl;
                pl.id = parseInt(plNode.at("_id").get<std::string>());
                pl.position = parseVec3(plNode.at("Position").get<std::string>());
                pl.intensity = parseVec3(plNode.at("Intensity").get<std::string>());
                scene.pointLights.push_back(pl);
            }
        }
    }

    // Materials
    if (s.contains("Materials") && s["Materials"].contains("Material")) {
        for (const auto& matNode : asArray(s["Materials"]["Material"])) {
            Material mat;
            mat.id = parseInt(matNode.at("_id").get<std::string>());
            if (matNode.contains("_type") && matNode["_type"].get<std::string>() == "mirror") {
                mat.isMirror = true;
            }
            mat.ambientReflectance = parseVec3(matNode.at("AmbientReflectance").get<std::string>());
            mat.diffuseReflectance = parseVec3(matNode.at("DiffuseReflectance").get<std::string>());
            mat.specularReflectance = parseVec3(matNode.at("SpecularReflectance").get<std::string>());
            if (matNode.contains("MirrorReflectance")) {
                mat.mirrorReflectance = parseVec3(matNode["MirrorReflectance"].get<std::string>());
            }
            if (matNode.contains("PhongExponent")) {
                mat.phongExponent = parseDouble(matNode["PhongExponent"].get<std::string>());
            }
            scene.materials.push_back(mat);
        }
    }

    // VertexData
    if (s.contains("VertexData") && s["VertexData"].contains("_data")) {
        std::stringstream ss(s["VertexData"]["_data"].get<std::string>());
        Vec3 v;
        while (ss >> v.x >> v.y >> v.z) {
            scene.vertices.push_back(v);
        }
    }

    // Objects
    if (s.contains("Objects")) {
        const json& objs = s["Objects"];

        if (objs.contains("Mesh")) {
            for (const auto& meshNode : asArray(objs["Mesh"])) {
                Mesh mesh;
                mesh.materialId = parseInt(meshNode.at("Material").get<std::string>()) - 1;
                if (meshNode.contains("Faces") && meshNode["Faces"].contains("_data")) {
                    std::stringstream ss(meshNode["Faces"]["_data"].get<std::string>());
                    int i0, i1, i2;
                    while (ss >> i0 >> i1 >> i2) {
                        Triangle tri;
                        tri.materialId = mesh.materialId;
                        tri.v0 = i0 - 1;
                        tri.v1 = i1 - 1;
                        tri.v2 = i2 - 1;

                        tri.A = scene.vertices[tri.v0];
                        tri.B = scene.vertices[tri.v1];
                        tri.C = scene.vertices[tri.v2];



                        mesh.faces.push_back(tri);
                    }
                }
                scene.meshes.push_back(mesh);
            }
        }

        if (objs.contains("Triangle")) {
            for (const auto& triNode : asArray(objs["Triangle"])) {
                Triangle tri;
                tri.materialId = parseInt(triNode.at("Material").get<std::string>()) - 1;
                std::stringstream ss(triNode.at("Indices").get<std::string>());
                int i0, i1, i2;
                ss >> i0 >> i1 >> i2;
                tri.v0 = i0 - 1;
                tri.v1 = i1 - 1;
                tri.v2 = i2 - 1;

                tri.A = scene.vertices[tri.v0];
                tri.B = scene.vertices[tri.v1];
                tri.C = scene.vertices[tri.v2];


                scene.triangles.push_back(tri);
            }
        }

        if (objs.contains("Sphere")) {
            for (const auto& sphNode : asArray(objs["Sphere"])) {
                Sphere sph;
                sph.materialId = parseInt(sphNode.at("Material").get<std::string>()) - 1;
                sph.centerVertexId = parseInt(sphNode.at("Center").get<std::string>()) - 1;
                sph.radius = parseDouble(sphNode.at("Radius").get<std::string>());

                sph.center = scene.vertices[sph.centerVertexId];

                scene.spheres.push_back(sph);
            }
        }

        if (objs.contains("Plane")) {
            for (const auto& plNode : asArray(objs["Plane"])) {
                Plane pl;
                pl.materialId = parseInt(plNode.at("Material").get<std::string>()) - 1;
                pl.centerVertexId = parseInt(plNode.at("Center").get<std::string>()) - 1;
                pl.normal = parseVec3(plNode.at("Normal").get<std::string>());
                pl.normal = pl.normal.Normalize(); //Ensure normalized plane normals

                //Find the point on a plane by using center vertex given
                pl.point = scene.vertices[pl.centerVertexId];

                scene.planes.push_back(pl);
            }
        }

        if (objs.contains("Cylinder")) {
            for (const auto& cylNode : asArray(objs["Cylinder"])) {
                Cylinder cyl;
                cyl.materialId = parseInt(cylNode.at("Material").get<std::string>()) - 1;
                cyl.centerVertexId = parseInt(cylNode.at("Center").get<std::string>()) - 1;
                cyl.axis = parseVec3(cylNode.at("Axis").get<std::string>());
                cyl.radius = parseDouble(cylNode.at("Radius").get<std::string>());
                cyl.height = parseDouble(cylNode.at("Height").get<std::string>());

                cyl.center = scene.vertices[cyl.centerVertexId];
                scene.cylinders.push_back(cyl);
            }
        }
    }
}