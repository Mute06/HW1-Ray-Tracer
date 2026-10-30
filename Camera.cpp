#include "Camera.h"

void Camera::ComputeBasis() {
    v = Up;
    w = (-1.0 * Gaze).Normalize();
    u = Vec3::Cross(v,w);

    Vec3 middle = Position - (w * NearPlaneDistance);
    q = middle + NearPlaneLeft * u + NearPlaneTop  * v;

    Pixel_Width = (NearPlaneRight - NearPlaneLeft) / ImageWidth;
    Pixel_Height = (NearPlaneTop - NearPlaneBottom) / ImageHeight;
}

Ray Camera::GenerateRay(int i, int j) const {

    double S_U = (i + 0.5) * Pixel_Width;
    double S_V = (j + 0.5) * Pixel_Height;

    Vec3 S = q + S_U * u - S_V * v;
    Ray ray(Position, S - Position, 0, 1.0, 1e30);
    return ray;
}
