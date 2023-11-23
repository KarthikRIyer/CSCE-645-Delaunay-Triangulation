//
// Created by Karthik Iyer on 23/10/23.
//

#ifndef DELAUNAY_CIRCUMCIRCLE_H
#define DELAUNAY_CIRCUMCIRCLE_H

#include <glm/vec2.hpp>

class Circumcircle {
public:
    Circumcircle(glm::vec2 circumcenter, float radius);
    glm::vec2 circumcenter;
    float radius;
};


#endif //DELAUNAY_CIRCUMCIRCLE_H
