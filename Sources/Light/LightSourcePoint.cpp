#include "LightSourcePoint.h"

LightSourcePoint::LightSourcePoint(glm::vec3 position_, glm::vec3 color_, float intensity_) :
    color(color_), position(position_), intensity(intensity_)
{
    this->a_c = 0.;
    this->a_l = 0.;
    this->a_q = 1.;
}

LightSourcePoint::LightSourcePoint() : LightSourcePoint(glm::vec3(20., 10., 150.), glm::vec3(0., 1., 0.1), 100000.) {}