//
// Created by Karthik Iyer on 22/10/23.
//

#ifndef DELAUNAY_TRIANGLE_H
#define DELAUNAY_TRIANGLE_H

#include <vector>
#include <glm/vec2.hpp>
#include "Circumcircle.h"
#include "Edge.h"

class Triangle {

public:
    Triangle(size_t v0, size_t v1, size_t v2);
    Circumcircle getCircumcircle(std::vector<glm::vec2>& verts) const;
    bool inCircumcircle(size_t v, std::vector<glm::vec2>& verts) const;
    void generateEdgesWithCircumcenter(std::vector<glm::vec2>& verts);
    static std::vector<glm::vec2> getSupertriangle(std::vector<glm::vec2>& vertices);
    std::vector<Edge>& getEdges();

    size_t v0;
    size_t v1;
    size_t v2;
    std::vector<Edge> edges;
};


#endif //DELAUNAY_TRIANGLE_H
