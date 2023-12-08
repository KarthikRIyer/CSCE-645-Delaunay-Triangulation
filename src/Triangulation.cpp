#include "Triangulation.h"
#include "Edge.h"
#include "GLSL.h"
#include "Program.h"
#include <algorithm>
#include <iostream>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

Triangulation::Triangulation() {
    vertices.clear();
    tris.clear();
    voronoiVertices.clear();
    this->voronoiEdges.clear();
    posBuf.clear();
    colBuf.clear();
}

void Triangulation::reset() {
    vertices.clear();
    tris.clear();
    voronoiVertices.clear();
    this->voronoiEdges.clear();
    posBuf.clear();
    colBuf.clear();
}

void Triangulation::init() {
// Send the position array to the GPU
    glGenBuffers(1, &posBufID);
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

    glGenBuffers(1, &colBufID);
    glBindBuffer(GL_ARRAY_BUFFER, colBufID);
    glBufferData(GL_ARRAY_BUFFER, colBuf.size()*sizeof(float), &colBuf[0], GL_STATIC_DRAW);

    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLSL::checkError(GET_FILE_LINE);
}

void Triangulation::triangulate(std::vector<glm::vec2> pts) {
    tris.clear();
    vertices.clear();
    vertices = pts;
    // create super triangle
    std::vector<glm::vec2> stVerts = Triangle::getSupertriangle(pts);
    vertices.insert(vertices.end(), stVerts.begin(), stVerts.end());
    
    Triangle st(vertices.size() - 1, vertices.size() - 2, vertices.size() - 3);
    tris.push_back(st);

    // triangulate
    for (size_t i = 0; i < vertices.size() - 3; i++) {
        addVert(i);
    }

    // remove triangles that share edges with super triangle
    tris.erase(
            std::remove_if(tris.begin(), tris.end(),
                           [&](const Triangle& t) {
                                return vertices[t.v0] == vertices[st.v0] || vertices[t.v0] == vertices[st.v1] || vertices[t.v0] == vertices[st.v2] ||
                                        vertices[t.v1] == vertices[st.v0] || vertices[t.v1] == vertices[st.v1] || vertices[t.v1] == vertices[st.v2] ||
                                        vertices[t.v2] == vertices[st.v0] || vertices[t.v2] == vertices[st.v1] || vertices[t.v2] == vertices[st.v2];
                            }),
               tris.end());

    vertices.resize(vertices.size() - 3);
    posBuf.clear();
    for (int i = 0; i < tris.size(); i++) {
        glm::vec2 v0 = vertices[tris[i].v0];
        glm::vec2 v1 = vertices[tris[i].v1];
        glm::vec2 v2 = vertices[tris[i].v2];
        posBuf.push_back(v0.x);
        posBuf.push_back(v0.y);
        posBuf.push_back(0);
        posBuf.push_back(v1.x);
        posBuf.push_back(v1.y);
        posBuf.push_back(0);
        posBuf.push_back(v2.x);
        posBuf.push_back(v2.y);
        posBuf.push_back(0);
        float r = 0.0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0)));
        float g = 0.0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0)));
        float b = 0.0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(1.0)));
        colBuf.push_back(r);
        colBuf.push_back(g);
        colBuf.push_back(b);
        colBuf.push_back(r);
        colBuf.push_back(g);
        colBuf.push_back(b);
        colBuf.push_back(r);
        colBuf.push_back(g);
        colBuf.push_back(b);
    }
    glBindBuffer(GL_ARRAY_BUFFER, posBufID);
    glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colBufID);
    glBufferData(GL_ARRAY_BUFFER, colBuf.size()*sizeof(float), &colBuf[0], GL_STATIC_DRAW);
    // Unbind the arrays
    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void Triangulation::addVert(size_t v) {
    std::vector<Edge> edges;
    std::vector<Edge> uniqueEdges;

    std::vector<Triangle> newTris;
    for (Triangle t: tris) {
        if (t.inCircumcircle(v, vertices)) {
            edges.push_back(Edge(t.v0, t.v1));
            edges.push_back(Edge(t.v1, t.v2));
            edges.push_back(Edge(t.v2, t.v0));
        } else {
            newTris.push_back(t);
        }
    }
    tris = newTris;

    // get unique edges
    for (int i = 0; i < edges.size(); i++) {
        bool isUnique = true;
        for (int j = 0; j < edges.size(); j++) {
            if (i != j && edges[i] == edges[j]) {
                isUnique = false;
                break;
            }
        }
        if (isUnique) uniqueEdges.push_back(edges[i]);
    }

    // create new triangles from the unique edges and new vert
    for (Edge e: uniqueEdges) {
        tris.push_back(Triangle(e.v0, e.v1, v));
    }
}

void Triangulation::draw(std::shared_ptr<Program> progTri, std::shared_ptr<Program> progPoint, std::shared_ptr<MatrixStack> P, std::shared_ptr<MatrixStack> MV) {
    if (!tris.empty()) {
        progTri->bind();
        glUniformMatrix4fv(progTri->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        glUniformMatrix4fv(progTri->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
        int h_pos = progTri->getAttribute("aPos");
        glEnableVertexAttribArray(h_pos);
        glBindBuffer(GL_ARRAY_BUFFER, posBufID);
        glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        int h_col = progTri->getAttribute("col");
        glEnableVertexAttribArray(h_col);
        glBindBuffer(GL_ARRAY_BUFFER, colBufID);
        glVertexAttribPointer(h_col, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

        int count = (int)posBuf.size()/3; // number of indices to be rendered
        glDrawArrays(GL_TRIANGLES, 0, count);

        glDisableVertexAttribArray(h_pos);
        glDisableVertexAttribArray(h_col);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        GLSL::checkError(GET_FILE_LINE);
        progTri->unbind();

        progPoint->bind();
        glUniformMatrix4fv(progPoint->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        glUniformMatrix4fv(progPoint->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));

        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glColor3d(0.2, 0.2, 0.2);
        for (auto it = voronoiEdges.begin(); it != voronoiEdges.end(); it++) {
            std::vector<glm::vec2> e = it->second;
            if (e.size() < 2) continue;
            glVertex2d(e[0].x, e[0].y);
            glVertex2d(e[1].x, e[1].y);
        }
        glEnd();
        progPoint->unbind();
    }
}

void Triangulation::voronoi(std::vector<glm::vec2> pts) {
    auto start = std::chrono::high_resolution_clock::now();
    this->triangulate(pts);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout<<"Time triangulation: "<<duration.count()<<" microseconds\n";

    start = std::chrono::high_resolution_clock::now();
    this->voronoiEdges.clear();
    this->voronoiVertices.clear();
    for (Triangle &tri: tris) {
        tri.generateEdgesWithCircumcenter(vertices);
    }

    for (Triangle &tri: tris) {
        std::vector<Edge> edges = tri.getEdges();
        for (Edge e: edges) {
            voronoiEdges[e].push_back(e.c);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout<<"Time voronoi: "<<duration.count()<<" microseconds\n";
}