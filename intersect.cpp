
#include "intersect.h"

bool intersectSphere(const Ray &ray, const Sphere &sphere, const Scene &scene, HitRecord &record) {


    //delta_P: p = o - c
    Vec3 delta_p = ray.origin - sphere.center;

    // A = d . d
    double A = Vec3::Dot(ray.direction, ray.direction);

    //B = 2h
    //h = dir . p
    double h = Vec3::Dot(ray.direction, delta_p);

    //C = (p . p) - R^2
    double C = Vec3::Dot(delta_p, delta_p) - sphere.radius * sphere.radius;


    double discriminant = h * h - A * C;
    if (discriminant < 0) {
        return false; // Ray misses
    }

    double sqrtD = std::sqrt(discriminant);

    double t1 = (-h - sqrtD) / A;
    double t2 = (-h + sqrtD) / A;

    //T selection
    double tCandidate = -1.0;
    if (t1 >= ray.tMin && t1 <= ray.tMax) {
        tCandidate = t1;
    }
    else if (t2 >= ray.tMin && t2 <= ray.tMax) {
        tCandidate = t2;
    }
    else {
        return false;
    }

    record.t = tCandidate;
    record.point = ray.PointAt(tCandidate);
    record.normal = (record.point - sphere.center) / sphere.radius; // n = (p - c) / |p-c|
    record.materialId = sphere.materialId;
    record.hit = true;
    return true;
}

bool intersectPlane(const Ray &ray, const Plane &plane, const Scene &scene, HitRecord &record) {

    double denominator = Vec3::Dot(ray.direction, plane.normal);

    if (std::abs(denominator) < 1e-6) {
        return false; // Ray is parallel to plane
    }

    double t = Vec3::Dot((plane.point - ray.origin), plane.normal);

    if (t < ray.tMin) {
        return false; //Behind camera
    }

    if (t > ray.tMax) {
        return false; // More far away than another detected object or out of range
    }

    record.hit = true;
    record.t = t;
    record.point = ray.PointAt(t);
    record.normal = (denominator < 0.0) ? plane.normal : (plane.normal * -1.0); //Normal points towards incoming ray
    record.materialId = plane.materialId;
    return true;
}

bool findClosestHit(const Ray &ray, const Scene &scene, HitRecord &bestHit) {

    // Local copy of ray
    Ray currentRay = ray;
    bestHit.hit = false;
    bool hitAnything = false;
    double minT = 1e30;
    HitRecord tempRecord;

    //Check Planes
    for (const Plane& plane : scene.planes) {
        if (intersectPlane(ray, plane, scene, tempRecord)) {
            hitAnything = true;
            currentRay.tMax = tempRecord.t;
            bestHit = tempRecord;
        }
    }

    //Check Spheres
    for (const Sphere& sphere : scene.spheres) {
        if (intersectSphere(ray, sphere, scene, tempRecord)) {
            hitAnything = true;
            currentRay.tMax = tempRecord.t;
            bestHit = tempRecord;
        }
    }


    bestHit.hit = hitAnything;
    return hitAnything;
}
