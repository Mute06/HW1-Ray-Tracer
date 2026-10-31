
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

    double t = Vec3::Dot((plane.point - ray.origin), plane.normal) / denominator;

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

bool intersectTriangle(const Ray &ray, const Triangle &triangle,
                       const Scene &scene, HitRecord &record) {

    const Vec3 &a = triangle.A;
    const Vec3 &b = triangle.B;
    const Vec3 &c = triangle.C;

    // Determinant of the matrix with columns u, v, w.
    auto determinant = [](const Vec3 &u, const Vec3 &v, const Vec3 &w) {
        return Vec3::Dot(u, Vec3::Cross(v, w));
    };

    // [a-b  a-c  d] * [beta gamma t]^T = a-o
    const Vec3 ab = a - b;
    const Vec3 ac = a - c;
    const Vec3 rhs = a - ray.origin;

    const Vec3 normal = Vec3::Cross(b - a, c - a);
    const double normalLength = normal.Length();
    const double directionLength = ray.direction.Length();

    // A zero-area triangle or a zero direction has no valid intersection.
    if (normalLength == 0.0 || directionLength == 0.0) {
        return false;
    }

    const double detA = determinant(ab, ac, ray.direction);
    const double EPSILON = 1e-8;

    // Parallel / nearly parallel ray.
    // Scale the tolerance because ray.direction is not necessarily unit length.
    if (std::abs(detA) <= EPSILON * normalLength * directionLength) {
        return false;
    }

    // Replace the first column to find beta.
    const double beta =
        determinant(rhs, ac, ray.direction) / detA;

    if (beta < 0.0) {
        return false;
    }

    // Replace the second column to find gamma.
    const double gamma =
        determinant(ab, rhs, ray.direction) / detA;

    if (gamma < 0.0 || beta + gamma > 1.0) {
        return false;
    }

    // Replace the third column to find t.
    const double t = determinant(ab, ac, rhs) / detA;

    if (t < ray.tMin || t > ray.tMax) {
        return false;
    }

    record.t = t;
    record.point = ray.PointAt(t);
    record.normal = normal / normalLength;
    record.materialId = triangle.materialId;
    record.hit = true;

    return true;
}

bool intersectMesh(const Ray &ray, const Mesh &mesh,
                   const Scene &scene, HitRecord &record) {

    Ray currentRay = ray;
    bool hitAnything = false;
    HitRecord tempRecord;

    for (const Triangle &face : mesh.faces) {

        if (intersectTriangle(currentRay, face, scene, tempRecord)) {

            hitAnything = true;

            // Only accept closer intersections from now on.
            currentRay.tMax = tempRecord.t;

            record = tempRecord;

            // All faces use the mesh's material.
            record.materialId = mesh.materialId;
        }
    }

    return hitAnything;
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
        if (intersectPlane(currentRay, plane, scene, tempRecord)) {
            hitAnything = true;
            currentRay.tMax = tempRecord.t;
            bestHit = tempRecord;
        }
    }

    //Check Spheres
    for (const Sphere& sphere : scene.spheres) {
        if (intersectSphere(currentRay, sphere, scene, tempRecord)) {
            hitAnything = true;
            currentRay.tMax = tempRecord.t;
            bestHit = tempRecord;
        }
    }

    // Check Triangles

    for (const Triangle &triangle : scene.triangles) {
        if (intersectTriangle(currentRay, triangle, scene, tempRecord)) {
        hitAnything = true;
        currentRay.tMax = tempRecord.t;
        bestHit = tempRecord;
    }
}

    // Check Meshes
    for (const Mesh &mesh : scene.meshes) {
        if (intersectMesh(currentRay, mesh, scene, tempRecord)) {
        hitAnything = true;

        currentRay.tMax = tempRecord.t;
        bestHit = tempRecord;
    }
}

    bestHit.hit = hitAnything;
    return hitAnything;
}

bool isInShadow(const Vec3 &hitPoint, const Vec3 &surfaceNormal, const PointLight &light, const Scene &scene) {
    Vec3 toLight = light.position - hitPoint;
    double distance = toLight.Length();
    Vec3 lightDirection = toLight / distance;

    Vec3 shadowRayOrigin = hitPoint + surfaceNormal * scene.shadowRayEpsilon;

    Ray shadowRay(shadowRayOrigin, lightDirection, 0, 0.0, distance);

    HitRecord shadowRecord;

    for (const Sphere& sphere : scene.spheres) {

        if (intersectSphere(shadowRay,sphere, scene, shadowRecord)) {
            return true;
        }

    }

    for (const Plane& plane : scene.planes) {
        if (intersectPlane(shadowRay, plane, scene, shadowRecord)) {
            return true;
        }
    }

    for (const Triangle& triangle : scene.triangles) {
        if (intersectTriangle(shadowRay, triangle, scene, shadowRecord)) {
            return true;
        }
    }

    for (const Mesh& mesh : scene.meshes) {
        for (const Triangle& face : mesh.faces) {
            if (intersectTriangle(shadowRay, face, scene, shadowRecord)) {
                return true;
            }
        }
    }

    return false;
}

