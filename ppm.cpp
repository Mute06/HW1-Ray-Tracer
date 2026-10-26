#include "ppm.h"
#include <fstream>
#include <cmath>
#include <algorithm>
#include <stdexcept>

void writePPM(const std::string& filename, int width, int height, const std::vector<Vec3>& pixels) {
    std::ofstream out(filename);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot open output PPM file: " + filename);
    }

    out << "P3\n" << width << " " << height << "\n255\n";

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            const Vec3& c = pixels[j * width + i];
            int r = static_cast<int>(std::round(std::clamp(c.x, 0.0, 255.0)));
            int g = static_cast<int>(std::round(std::clamp(c.y, 0.0, 255.0)));
            int b = static_cast<int>(std::round(std::clamp(c.z, 0.0, 255.0)));
            out << r << " " << g << " " << b << (i + 1 == width ? "" : " ");
        }
        out << "\n";
    }
}