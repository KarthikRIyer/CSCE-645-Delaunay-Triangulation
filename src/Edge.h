//
// Created by Karthik Iyer on 23/10/23.
//

#ifndef DELAUNAY_EDGE_H
#define DELAUNAY_EDGE_H

#include <glm/vec2.hpp>
#include <functional>

class Edge {

public:
    Edge(size_t v0, size_t v1);
    size_t v0;
    size_t v1;
    glm::vec2 c;

    bool operator==(Edge const& rhs) const;
};

template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

template<> struct std::hash<Edge> {
    std::size_t operator()(const Edge& e) const {
        size_t a = e.v0;
        size_t b = e.v1;
        if (a > b) {
            size_t t = a;
            a = b;
            b = t;
        }
        size_t seed = 0;
        std::hash<size_t> hasher;
        hash_combine(seed, hasher(a));
        hash_combine(seed, hasher(b));
        return seed;
    }
};


#endif //DELAUNAY_EDGE_H
