#include "shader.h"

Vec3 applyShading(const Ray &ray, const HitRecord &hit, const Scene &scene) {

    //Ambient lighting
    Material mat = scene.materials[hit.materialId];
    Vec3 L_a = mat.ambientReflectance * scene.ambientLight;

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

        //Check if light arrives from behind
        

    }

}
