//
// Created by Karthik Iyer on 22/10/23.
//

#include "Triangle.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

Triangle::Triangle(size_t v0, size_t v1, size_t v2): v0(v0), v1(v1), v2(v2) {
}

Circumcircle Triangle::getCircumcircle(std::vector<glm::vec2>& verts) const {
    glm::vec2 A = glm::vec2(verts[v0]);
    glm::vec2 B = glm::vec2(verts[v1]);
    glm::vec2 C = glm::vec2(verts[v2]);

    float D = 2 * (A.x*(B.y - C.y) + B.x*(C.y - A.y) + C.x*(A.y - B.y));
    float Dinv = 1.0f/D;
    float cx = Dinv * ((A.x*A.x + A.y*A.y)*(B.y - C.y) + (B.x*B.x + B.y*B.y)*(C.y - A.y) + (C.x*C.x + C.y*C.y)*(A.y - B.y));
    float cy = Dinv * ((A.x*A.x + A.y*A.y)*(C.x - B.x) + (B.x*B.x + B.y*B.y)*(A.x - C.x) + (C.x*C.x + C.y*C.y)*(B.x - A.x));
    glm::vec2 c(cx, cy);
    float r = glm::length(c - A);

    return Circumcircle(c, r);
}

std::vector<Edge>& Triangle::getEdges() {
    return edges;
}

void Triangle::generateEdgesWithCircumcenter(std::vector<glm::vec2>& verts) {
    edges.clear();
    edges.emplace_back(v0, v1);
    edges.emplace_back(v1, v2);
    edges.emplace_back(v2, v0);
    Circumcircle circumcircle = this->getCircumcircle(verts);
    edges[0].c = circumcircle.circumcenter;
    edges[1].c = circumcircle.circumcenter;
    edges[2].c = circumcircle.circumcenter;
}

bool Triangle::inCircumcircle(size_t v, std::vector<glm::vec2>& verts) const {
    glm::vec2 vert = verts[v];
    Circumcircle c = getCircumcircle(verts);
    float dx = c.circumcenter.x - vert.x;
    float dy = c.circumcenter.y - vert.y;
    return sqrt(dx*dx + dy*dy) - c.radius <= 1e-5;
}

std::vector<glm::vec2> Triangle::getSupertriangle(std::vector<glm::vec2>& vertices) {
    float minx = FLT_MAX;
    float miny = FLT_MAX;
    float maxx = FLT_MIN;
    float maxy = FLT_MIN;

    for (glm::vec2 v: vertices) {
        minx = std::min(minx, v.x);
        miny = std::min(miny, v.y);
        maxx = std::max(maxx, v.x);
        maxy = std::max(maxy, v.y);
    }

    float dx = (maxx - minx) * 10;
    float dy = (maxy - miny) * 10;

    glm::vec2 P0(minx, miny);
    glm::vec2 P1(maxx, miny);
    glm::vec2 P2(maxx, maxy);
    glm::vec2 P3(minx, maxy);

    glm::vec2 d0 = P2 - P0;

    glm::vec2 d1 = P3 - P1;

    P0 -= d0;
    P1 -= d1;
    P2 += d0;
    P3 += d1;

    glm::vec2 v0 = 0.5f*(P3+P2) + 2.f*(P2-P1);
    glm::vec2 v1 = P1 + 2.f*(P1-P0);
    glm::vec2 v2 = P0 - 2.f*(P1-P0);
//    glm::mat3 I(1.0f);
//    glm::mat3 t = glm::rotate(I, glm::radians(10.0f));
//    v0 = glm::vec3(v0, 1) * t;
//    v1 = glm::vec3(v1, 1) * t;
//    v2 = glm::vec3(v2, 1) * t;

//    v0 *= 5;
//    v1 *= 5;
//    v2 *= 5;

//    glm::vec2 v0(minx - dx, miny - dy*3);
//    glm::vec2 v1(minx - dx, maxy + dy);
//    glm::vec2 v2(maxx + dx*3, maxy + dy);
    std::vector<glm::vec2> st(3);
    st[0] = v0;
    st[1] = v1;
    st[2] = v2;
    return st;
}
