#ifndef DELAUNAY_TRIANGULATION_H
#define DELAUNAY_TRIANGULATION_H

#include <vector>
#include <glm/vec2.hpp>
#include <GL/glew.h>
#include "Triangle.h"
#include "MatrixStack.h"

class Program;

class Triangulation {
private:
    void addVert(size_t v);
public:
    Triangulation();
    void triangulate(std::vector<glm::vec2> pts);
    void voronoi(std::vector<glm::vec2> pts);
    void reset();
    void init();
//    void saveObj(const std::string& filename);
    void draw(std::shared_ptr<Program> prog, std::shared_ptr<Program> progPoint, std::shared_ptr<MatrixStack> P, std::shared_ptr<MatrixStack> MV);

    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> voronoiVertices;
    std::unordered_map<Edge, std::vector<glm::vec2>> voronoiEdges;
    std::vector<float> posBuf;
    std::vector<float> colBuf;
    std::vector<Triangle> tris;
    GLuint posBufID;
    GLuint colBufID;
};


#endif //DELAUNAY_TRIANGULATION_H
