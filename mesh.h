#ifndef MESH_H
#define MESH_H

#include "types.h"
#include <QGLWidget>
#include <QFile>

#include <map>
#include <vector>

#define USE_OBJ_NORMALS 0

using namespace std;

struct Vertex;
struct Edge;
struct Face;

typedef struct Vertex Vertex;
typedef struct Edge Edge;
typedef struct Face Face;

struct Vertex {
    Vector3f pos;
    vector<uint> edges;
    vector<uint> faces;
    Vector3f normal;
};

int uintCompare (const void *a, const void *b);

struct Edge {
    uint vertices[2];
    uint faces[2];
    uint numFaces;

    Edge() : numFaces(0) {}

    bool operator<(const Edge &e) const {
        uint V1[2] = {vertices[0], vertices[1]};
        uint V2[2] = {e.vertices[0], e.vertices[1]};

        qsort(V1, 2, sizeof(uint), uintCompare);
        qsort(V2, 2, sizeof(uint), uintCompare);

        for (uint i = 0; i < 2; i++) {
            if (V1[i] == V2[i]) continue;
            return V1[i] < V2[i];
        }

        return false;
    }
};

struct Face {
    uint vertices[4];
    uint edges[4];
    Vector3f normals[4];

    bool operator<(const Face &f) const {
        uint V1[4] = {vertices[0], vertices[1], vertices[2], vertices[3]};
        uint V2[4] = {f.vertices[0], f.vertices[1], f.vertices[2], f.vertices[3]};

        qsort(V1, 4, sizeof(uint), uintCompare);
        qsort(V2, 4, sizeof(uint), uintCompare);

        for (uint i = 0; i < 4; i++) {
            if (V1[i] == V2[i]) continue;
            return V1[i] < V2[i];
        }

        return false;
    }
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    static Mesh cube();
    static Mesh *fromObjFile(QString filename);

    void unitize();

    const float *getVertexBuffer(uint &numVertices);
    const float *getNormalBuffer(uint &numVertices);
    void glDraw();

    Mesh subdivide();

protected:
    void addFace(Vector3f v1, Vector3f v2, Vector3f v3, Vector3f v4, const Vector3f *normals = 0);
    void addFace(uint v1, uint v2, uint v3, uint v4, const Vector3f *normals = 0);

    uint indexOf(Vector3f p);
    uint indexOf(uint v1, uint v2);

    void createBuffers();
    void calculatePoints();

protected:
    float *m_vertexBuffer;
    float *m_normalBuffer;
    bool m_cached;
    uint m_numVertices;

    vector<Vertex> m_vertices;
    vector<Edge> m_edges;
    vector<Face> m_faces;

    vector<Vector3f> m_facePoints;
    vector<Vector3f> m_edgePoints;
    vector<Vector3f> m_vertexPoints;

    vector<Vector3f> m_facePointNormals;
    vector<Vector3f> m_edgePointNormals;
    vector<Vector3f> m_vertexPointNormals;

    map<Vector3f, uint> m_pointIdxMap;
    map<Edge, uint> m_edgeIdxMap;
    map<Face, uint> m_faceIdxMap;
};

#endif // MESH_H
