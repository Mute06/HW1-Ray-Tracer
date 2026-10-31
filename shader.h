
#ifndef HW1_RAY_TRACER_SHADER_H
#define HW1_RAY_TRACER_SHADER_H

#include "math.h"
#include "intersect.h"

Vec3 applyShading(const Ray& ray, const HitRecord& hit, const Scene& scene);
Vec3 computeColor(const Ray& ray, const Scene& scene);

#endif //HW1_RAY_TRACER_SHADER_H
