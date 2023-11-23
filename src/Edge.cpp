#include "Edge.h"

Edge::Edge(size_t v0, size_t v1): v0(v0), v1(v1) {
}

bool Edge::operator==(Edge const& rhs) const {
    return (v0 == rhs.v0 && v1 == rhs.v1) || (v0 == rhs.v1 && v1 == rhs.v0);
}