#include "shader.h"

Vec3 applyShading(const Ray &ray, const HitRecord &hit, const Scene &scene) {

    //Ambient lighting
    Material mat = scene.materials[hit.materialId];
    //only ambient lightnt for now
    Vec3 totalColor = mat.ambientReflectance * scene.ambientLight;

    bool isMirror = mat.isMirror || mat.mirrorReflectance.x > 1e-6 || mat.mirrorReflectance.y > 1e-6 || mat.mirrorReflectance.z > 1e-6;

    if (isMirror && ray.depth < scene.maxRecursionDepth) {
        //Add the  color from mirror direction

        Vec3 unitDir = ray.direction.Normalize();
        Vec3 n = hit.normal.Normalize();

        Vec3 reflectionDir = Vec3::Reflect(unitDir, n).Normalize();

        Vec3 reflOrigin = hit.point + n * scene.shadowRayEpsilon;

        Ray reflectionRay(reflOrigin, reflectionDir, ray.depth + 1, 0);
        totalColor = totalColor + computeColor(reflectionRay, scene) * mat.mirrorReflectance;
    }

    for (const PointLight &light : scene.pointLights) {

        if (isInShadow(hit.point, hit.normal, light, scene)) {
            continue;
        }

        double distance = (light.position - hit.point).Length();

        //w_i
        Vec3 unitLightDirection = (light.position - hit.point).Normalize();

        Vec3 Irradiance = light.intensity  / (distance * distance);

        double cosTheta = std::max(0.0, Vec3::Dot(unitLightDirection, hit.normal));

        //diffuse term
        Vec3 L_d = (mat.diffuseReflectance * Irradiance) * cosTheta;


        //w_o
        Vec3 unitViewDirection = -(ray.direction).Normalize();

        //Normalized half vector
        Vec3 h = (unitLightDirection + unitViewDirection).Normalize();

        Vec3 L_s;
        //Check if light arrives from behind
        if (cosTheta <= 0.0) {
            //Specular reflection is zero
            L_s = Vec3::Zero();
        }
        else {
            double cosalfa = std::max(0.0, Vec3::Dot(hit.normal, h));

            L_s = mat.specularReflectance * Irradiance * std::pow(cosalfa, mat.phongExponent);
        }



        totalColor = totalColor + (L_d + L_s);

    }

    return totalColor;

}


Vec3 computeColor(const Ray &ray, const Scene &scene) {

    if (ray.depth > scene.maxRecursionDepth) {
        return Vec3::Zero();
    }



    HitRecord record;
    if (findClosestHit(ray, scene, record)) {
        Vec3 color = applyShading(ray, record, scene); // ambient + diffuse + specular
        return color;
    }

    //No hit

    //if primary ray
    if (ray.depth == 0) {

        return scene.backgroundColor;
    }

    //reflected ray
    else if (ray.depth > 0) {
        return Vec3::Zero();
    }

    return Vec3::Zero();
}