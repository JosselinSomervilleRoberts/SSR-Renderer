#include "LightSourceDir.h"


LightSourceDir::LightSourceDir(glm::vec3 direction_, glm::vec3 color_, float intensity_) :
    color(color_), direction(direction_), intensity(intensity_)
{}

LightSourceDir::LightSourceDir() : LightSourceDir(glm::vec3(1., 0., 0.), glm::vec3(0., 1., 0.1), 10.) {}