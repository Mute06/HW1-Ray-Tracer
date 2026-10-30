#ifndef HW1_RAY_TRACER_CAMERA_H
#define HW1_RAY_TRACER_CAMERA_H

#include "math.h"
#include "string"

class Camera {
public:

    int id;
    //e vector
    Vec3 Position;
    Vec3 Gaze;
    Vec3 Up;

    double NearPlaneLeft = 0.0, NearPlaneRight = 0.0, NearPlaneBottom = 0.0, NearPlaneTop = 0.0;
    double NearPlaneDistance = 0.0;

    int ImageWidth, ImageHeight;
    std::string ImageName;

    //Computed basis
    Vec3 u, v, w;
    Vec3 q; // Top left corner of image plane

    double Pixel_Width = 0.0, Pixel_Height = 0.0;

    Camera() = default;

    void ComputeBasis();
    Ray GenerateRay(int i, int j) const;


};


#endif //HW1_RAY_TRACER_CAMERA_H
