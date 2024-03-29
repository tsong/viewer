#ifndef MESH_H
#define MESH_H

#include "types.h"
#include <QGLWidget>

#include <map>
#include <vector>

#define USE_OBJ_NORMALS 0   //uses normals in OBJ file

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

//compares two unsigned integers for sorting
int uintCompare (const void *a, const void *b);

struct Edge {
    uint vertices[2];
    uint faces[2];
    uint numFaces;

    Edge() : numFaces(0) {}

    //less than operator for map
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

    //less than comparison for map
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

    static Mesh *fromObjFile(QString filename);

    const float *getVertexBuffer(uint &numVertices);
    const float *getNormalBuffer(uint &numVertices);
    void glDraw();

    // scales mesh down to a unit bounding box
    void unitize();

    // returns the mesh after one step of Catmull-Clark subdivision
    Mesh subdivide();

protected:
    // adds a face of 4 new vertices
    void addFace(Vector3f v1, Vector3f v2, Vector3f v3, Vector3f v4, const Vector3f *normals = 0);

    // adds a face of 4 existing vertices
    void addFace(uint v1, uint v2, uint v3, uint v4, const Vector3f *normals = 0);

    // returns the index of vertex in position p, and adds it to mesh if it does not exist
    uint indexOf(Vector3f p);

    // returns the index of an edge (2 vertices in mesh), and adds it to mesh if it does not exist
    uint indexOf(uint v1, uint v2);

    // initializes and fills the vertex and normal buffers
    void createBuffers();

    // calculates face, egde, and vertex points
    void calculatePoints();

protected:
    //vertex and normal data
    float *m_vertexBuffer;
    float *m_normalBuffer;
    bool m_cached;
    uint m_numVertices;

    //geometric primitives of mesh
    vector<Vertex> m_vertices;
    vector<Edge> m_edges;
    vector<Face> m_faces;

    //vertices computed in subdivision
    vector<Vector3f> m_facePoints;
    vector<Vector3f> m_edgePoints;
    vector<Vector3f> m_vertexPoints;

    //normals computed in subdivision
    vector<Vector3f> m_facePointNormals;
    vector<Vector3f> m_edgePointNormals;
    vector<Vector3f> m_vertexPointNormals;

    //lookup maps of geometric primitives to their index
    map<Vector3f, uint> m_pointIdxMap;
    map<Edge, uint> m_edgeIdxMap;
    map<Face, uint> m_faceIdxMap;
};

#endif // MESH_H
