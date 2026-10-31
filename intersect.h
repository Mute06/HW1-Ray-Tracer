//
// Created by umut on 10/30/26.
//

#ifndef HW1_RAY_TRACER_INTERSECT_H
#define HW1_RAY_TRACER_INTERSECT_H
#include "math.h"
#include "scene.h"

struct HitRecord {
  bool hit = false;
  double t = 0.0;

  Vec3 point;
  Vec3 normal;
  int materialId;
};

//Intersection Functions
bool intersectSphere(const Ray& ray, const Sphere& sphere, const Scene& scene, HitRecord& record);
bool intersectPlane(const Ray& ray, const Plane& plane, const Scene& scene, HitRecord& record);
bool intersectTriangle(const Ray &ray, const Triangle &triangle, const Scene &scene, HitRecord &record)
bool intersectMesh(const Ray &ray, const Mesh &mesh, const Scene &scene, HitRecord &record);
bool findClosestHit(const Ray& ray, const Scene& scene, HitRecord& bestHit);

bool isInShadow(const Vec3& hitPoint, const Vec3& surfaceNormal, const PointLight& light, const Scene& scene);


#endif //HW1_RAY_TRACER_INTERSECT_H
