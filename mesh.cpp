#include "mesh.h"
#include <QFile>

int uintCompare (const void *a, const void *b) {
    uint v1 = *(uint*)a;
    uint v2 = *(uint*)b;
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}

Mesh::Mesh()
    : m_vertexBuffer(0), m_normalBuffer(0), m_cached(false), m_numVertices(0)
{
}

Mesh::~Mesh() {
    if (m_vertexBuffer)
        free(m_vertexBuffer);

    if (m_normalBuffer)
        free(m_normalBuffer);
}

void Mesh::unitize() {
    //find bounding box of mesh
    Vector3f minPos, maxPos;
    for (uint i = 0; i < m_vertices.size(); i++) {
        Vector3f &pos = m_vertices[i].pos;
        for (uint j = 0; j < 3; j++) {
            minPos[j] = min(minPos[j],pos[j]);
            maxPos[j] = max(maxPos[j],pos[j]);
        }
    }

    //calculate scaling factor and offset from origin
    float scale = min(maxPos[0] - minPos[0], maxPos[1] - minPos[1]);
    scale = min(scale, maxPos[2] - minPos[2]);
    Vector3f center = (minPos + maxPos)/2.0;

    //scale and translate all vertices in mesh
    for (uint i = 0; i < m_vertices.size(); i++) {
       Vector3f &pos = m_vertices[i].pos;
       for (uint j = 0; j < 3; j++) {
           pos[j] = pos[j] - center[j]; //translate so center is at origin
           pos[j] = pos[j] / scale;     //scale to unit bounding box
       }
    }
}

/* parse a line in the form of "<type> <x> <y> <z>" to a coordinate*/
bool parseCoordinate(QString line, Vector3f& coord) {
    line = line.simplified();

    QStringList tokens = line.split(" ", QString::SkipEmptyParts);
    if (tokens.size() != 4)
        return false;

    QStringListIterator it(tokens);
    it.next();
    for (uint i = 0; i < 3; i++) {
        bool ok = false;
        coord[i] = it.next().toFloat(&ok);
        if (!ok) return false;
    }

    return true;
}

/* parse a line in the form of "<f> <v1/t1/n1> <v2/t2/n2> <v3/t3/n3> <v4/t4/n4>"
   into a face. The texture and normal indices are optional */
bool parseFace(QString line, uint *vertices, uint *normals, bool &hasNormals) {
    line = line.simplified();

    //ensure that the face is a quad
    QStringList tokens = line.split(" ", QString::SkipEmptyParts);
    if (tokens.size() != 5)
        return false;

    hasNormals = false;
    QStringListIterator it(tokens);
    it.next();
    for (uint i = 0; i < 4; i++) {
        //parse the token into its vertex, texture, and normal indices
        QString vertexStr = it.next();
        QStringList vertexTokens = vertexStr.split("/");

        //check if data is properly formatted
        bool validVertex = vertexTokens.size() > 0 && vertexTokens.size() <= 3;
        bool validNormals = !hasNormals || vertexTokens.size() == 3; //has normals --> 3 vertex tokens
        if (!validVertex || !validNormals)
            return false;

        //add vertex index
        bool ok = false;
        QStringListIterator vertexIt(vertexTokens);
        vertices[i] = vertexIt.next().toInt(&ok) - 1;

        //since we ignore texture indices, we can skip the second token
        if (vertexTokens.size() == 3) {
            hasNormals = true;
            vertexIt.next();
            normals[i] = vertexIt.next().toInt(&ok) - 1;
        }

        if (!ok) return false;
    }

    return true;
}

Mesh *Mesh::fromObjFile(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    Mesh *M = new Mesh();
    vector<Vector3f> normals;

    //first pass to add all vertices and normals to mesh
    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();

        if (lineBytes.at(0) == 'v' && lineBytes.at(1) == 'n') {
            // parse normal if line starts with 'vn'
            Vector3f normal;

            if (!parseCoordinate(QString(lineBytes), normal)) {
                delete M;
                return false;
            }

            normals.push_back(normal);
        } else if (lineBytes.at(0) == 'v' && lineBytes.at(1) == ' ') {
            // parse vertex if line starts with just 'v'
            Vertex V;
            if (!parseCoordinate(QString(lineBytes), V.pos)) {
                delete M;
                return false;
            }
            M->m_vertices.push_back(V);
        }
    }

    //second pass to add faces to the mesh
    file.reset();
    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();
        if (lineBytes.at(0) == 'f') {
            uint V[4];
            uint N[4];
            bool hasNormals;

            if (!parseFace(QString(lineBytes), V, N, hasNormals)) {
                delete M;
                return false;
            }

            if (hasNormals) {
                //use normals if they are provided
                Vector3f faceNormals[4];
                for (uint i = 0; i < 4; i++) {
                    if (N[i] < normals.size())
                        faceNormals[i] = normals[N[i]];
                }
                M->addFace(V[0],V[1],V[2],V[3], faceNormals);
            } else {
                //interpolate normals if they are not provided
                M->addFace(V[0],V[1],V[2],V[3]);
            }
        }
    }

    return M;
}

Mesh Mesh::subdivide() {
    //calculate new points in subdivided mesh
    calculatePoints();

    Mesh M;

    //each face will become 4 faces in the new mesh
    for (uint i = 0; i < m_faces.size(); i++) {
        Face &f = m_faces[i];
        Vector3f V[4];
        Vector3f N[4];

        //calculate the vertices of the new faces
        V[0] = m_facePoints[i];
        N[0] = m_facePointNormals[i];
        for (uint j = 0; j < 4; j++) {
            V[1] = m_edgePoints[f.edges[j]];
            V[2] = m_vertexPoints[f.vertices[(j+1)%4]];
            V[3] = m_edgePoints[f.edges[(j+1)%4]];

            N[1] = m_edgePointNormals[f.edges[j]];
            N[2] = m_vertexPointNormals[f.vertices[(j+1)%4]];
            N[3] = m_edgePointNormals[f.edges[(j+1)%4]];

            M.addFace(V[0],V[1],V[2],V[3],N);
        }
    }

    return M;
}

void Mesh::calculatePoints() {
    //face points
    for (uint i = 0; i < m_faces.size(); i++) {
        //interpolate face point coordinate from all vertices of face
        const uint *V = m_faces[i].vertices;
        Vector3f p(0,0,0);
        for (uint j = 0; j < 4; j++) p = p + m_vertices[V[j]].pos;
        m_facePoints.push_back(p / 4.0);

        //interpolate face point normal from normals of all vertices of face
        Vector3f n;
        for (uint j = 0; j < 4; j++) n = n + m_faces[j].normals[j];
        m_facePointNormals.push_back(n / 4.0);
    }

    //edge points
    for (uint i = 0; i < m_edges.size(); i++) {
        const uint *V = m_edges[i].vertices;
        const uint *F = m_edges[i].faces;

        //interpolate edge point coordinate from adjacent face points and two vertices
        Vector3f p(0,0,0);
        for (uint j = 0; j < 2; j++) p = p + m_vertices[V[j]].pos;
        for (uint j = 0; j < m_edges[i].numFaces; j++) p = p + m_facePoints[F[j]];
        m_edgePoints.push_back(p / (2 + m_edges[i].numFaces));

        //interpolate edge point normal from normals of adjacent face points and two vertices
        Vector3f n;
        for (uint j = 0; j < 2; j++) n = n + m_vertices[V[j]].normal;
        for (uint j = 0; j < m_edges[i].numFaces; j++) p = p + m_facePointNormals[F[j]];
        m_edgePointNormals.push_back(n / (2 + m_edges[i].numFaces));
    }

    //vertex points
    for (uint i = 0; i < m_vertices.size(); i++) {
        vector<uint> &E = m_vertices[i].edges;
        vector<uint> &F = m_vertices[i].faces;
        uint n = E.size();

        //extrodinary point
        if (E.size() != F.size()) {
            m_vertexPoints.push_back(m_vertices[i].pos);
            continue;
        }
        //Q_ASSERT(F.size() == n);

        Vector3f p = m_vertices[i].pos;

        // f = average face point of all adjacent faces
        Vector3f f(0,0,0);
        for (uint j = 0; j < n; j++) f = f + m_facePoints[F[j]];
        f = f / n;

        //r = average coordinate of all adjacent edge midpoints
        Vector3f r(0,0,0);
        for (uint j = 0; j < n; j++) {
            uint v1 = m_edges[E[j]].vertices[0];
            uint v2 = m_edges[E[j]].vertices[1];
            r = r + (m_vertices[v1].pos + m_vertices[v2].pos)/2.0;
        }
        r = r / n;

        m_vertexPoints.push_back((f + r*2 + p*(n-3))/n);
        m_vertexPointNormals.push_back(m_vertices[i].normal);
    }
}


void Mesh::addFace(Vector3f v1, Vector3f v2, Vector3f v3, Vector3f v4, const Vector3f *normals) {
    addFace(indexOf(v1), indexOf(v2), indexOf(v3), indexOf(v4), normals);
}

void Mesh::addFace(uint v1, uint v2, uint v3, uint v4, const Vector3f *normals) {
    Face f;
    uint V[] = {v1,v2,v3,v4};
    uint idx = m_faces.size();

    //deal with each vertex of the face
    for (uint i = 0; i < 4; i++) {
        f.vertices[i] = V[i];
        f.edges[i] = indexOf(V[i],V[(i+1)%4]);

        Edge &e = m_edges[f.edges[i]];
        Q_ASSERT(e.numFaces < 2);
        e.faces[e.numFaces++] = idx;
        m_vertices[V[i]].faces.push_back(idx);
    }

    //deal with face normals
    if (USE_OBJ_NORMALS && normals) {
       //if vertex normals of face exist, just use those
        for (uint i = 0; i < 4; i++) {
            f.normals[i] = normals[i];

            //update cumulative moving average of vertex normal
            Vector3f &vertexNormal = m_vertices[f.vertices[i]].normal;
            float k = m_vertices[f.vertices[i]].faces.size();
            Q_ASSERT(k > 0);
            vertexNormal = (vertexNormal*(k-1) + normals[i])/k;
            vertexNormal = vertexNormal / vertexNormal.magnitude();
        }
    } else {
        //calculate normals if they do not exist
        Vector3f a = m_vertices[f.vertices[0]].pos - m_vertices[f.vertices[1]].pos;
        Vector3f b = m_vertices[f.vertices[1]].pos - m_vertices[f.vertices[2]].pos;
        Vector3f n = a.cross(b);
        n = n / n.magnitude();
        for (uint i = 0; i < 4; i++) {
            f.normals[i] = n;

            //update cumulative moving average of vertex normal
            Vector3f &vertexNormal = m_vertices[f.vertices[i]].normal;
            float k = m_vertices[f.vertices[i]].faces.size();
            Q_ASSERT(k > 0);
            vertexNormal = (vertexNormal*(k-1) + n)/k;
            vertexNormal = vertexNormal / vertexNormal.magnitude();
        }
    }

    m_faces.push_back(f);
}

uint Mesh::indexOf(Vector3f p) {
    //add new vertex with position p to the mesh if it does not exist
    if (m_pointIdxMap.count(p) == 0) {
        Vertex v;
        v.pos = p;
        m_vertices.push_back(v);
        m_pointIdxMap[p] = m_vertices.size() - 1;
    }

    return m_pointIdxMap[p];
}

uint Mesh::indexOf(uint v1, uint v2) {
    Edge e;
    e.numFaces = 0;
    e.vertices[0] = v1;
    e.vertices[1] = v2;

    //add edge e to the mesh if it does not exist
    if (m_edgeIdxMap.count(e) == 0) {
        m_edges.push_back(e);
        uint idx = m_edges.size()-1;
        m_edgeIdxMap[e] = idx;
        m_vertices[v1].edges.push_back(idx);
        m_vertices[v2].edges.push_back(idx);
    }

    return m_edgeIdxMap[e];
}

const float *Mesh::getVertexBuffer(uint &numVertices) {
    if (!m_cached)
        createBuffers();

    numVertices = m_numVertices;
    return m_vertexBuffer;
}

const float *Mesh::getNormalBuffer(uint &numVertices) {
    if (!m_cached)
        createBuffers();

    numVertices = m_numVertices;
    return m_normalBuffer;
}


void Mesh::createBuffers() {
    //create and fill buffers with vertex and normal coordinates
    m_numVertices = m_faces.size()*4;
    m_vertexBuffer = (float*)malloc(3*sizeof(float)*m_numVertices);
    m_normalBuffer = (float*)malloc(3*sizeof(float)*m_numVertices);
    uint idx = 0;
    for (uint i = 0; i < m_faces.size(); i++) {
        Face &f = m_faces[i];
        for (uint j = 0; j < 4; j++) {
            Vector3f &v = m_vertices[f.vertices[j]].pos;
            Vector3f &n = f.normals[j];

            for (uint k = 0; k < 3; k++) {
                m_vertexBuffer[idx] = v[k];
                m_normalBuffer[idx] = n[k];
                idx++;
            }
        }
    }

    m_cached = true;
}

void Mesh::glDraw() {
    //get vertex and normal buffers
    uint numVertices;
    const float *vertexBuffer = getVertexBuffer(numVertices);
    const float *normalBuffer = getNormalBuffer(numVertices);

    //draw arrays using openGL
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
        if (vertexBuffer) glVertexPointer(3, GL_FLOAT, 0, vertexBuffer);
        if (normalBuffer) glNormalPointer(GL_FLOAT, 0, normalBuffer);
        glDrawArrays(GL_QUADS, 0, numVertices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
