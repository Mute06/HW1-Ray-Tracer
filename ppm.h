#ifndef PPM_H
#define PPM_H

#include "math.h"
#include <string>
#include <vector>

void writePPM(const std::string& filename, int width, int height, const std::vector<Vec3>& pixels);

#endif // PPM_H