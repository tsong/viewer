#include "mesh.h"
#include <QtGlobal>

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

Mesh Mesh::cube() {
    float V[][3] = {{-1,-1,1},
                    {-1,1,1},
                    {1,1,1},
                    {1,-1,1},
                    {-1,-1,-1},
                    {-1,1,-1},
                    {1,1,-1},
                    {1,-1,-1}};

    uint F[][4] = { {1,4,3,2},
                    {1,5,8,4},
                    {2,6,5,1},
                    {3,7,6,2},
                    {4,8,7,3},
                    {6,7,8,5}};

    Mesh M;
    for (uint i = 0; i < 8; i++) {
        Vertex v;
        v.pos = Vector3f(V[i][0]/2.0,V[i][1]/2.0,V[i][2]/2.0);
        M.m_vertices.push_back(v);
    }

    for (uint i = 0; i < 6; i++) {
        M.addFace(F[i][0]-1,F[i][1]-1,F[i][2]-1,F[i][3]-1);
    }

    return M;
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

    //qDebug() << minPos.x << minPos.y << minPos.z;
    //qDebug() << maxPos.x << maxPos.y << maxPos.z;

    //Vector3f scale;
    //for (uint i = 0; i < 3; i++) scale[i] = 1.0/(maxPos[i] - minPos[i]);
    //qDebug() << scale.x << scale.y << scale.z;

    float scale = min(maxPos[0] - minPos[0], maxPos[1] - minPos[1]);
    scale = min(scale, maxPos[2] - minPos[2]);

    Vector3f center = (minPos + maxPos)/2.0;

   for (uint i = 0; i < m_vertices.size(); i++) {
       Vector3f &pos = m_vertices[i].pos;
       for (uint j = 0; j < 3; j++) {
           pos[j] = pos[j] - center[j]; //translate so center is at origin
           pos[j] = pos[j] / scale;     //scale to unit bounding box
       }
    }
}

bool parseCoordinate(QString line, Vector3f& coord) {
    line = line.simplified();

    QStringList tokens = line.split(" ", QString::SkipEmptyParts);
    Q_ASSERT(tokens.size() == 4);

    QStringListIterator it(tokens);
    it.next();
    for (uint i = 0; i < 3; i++) {
        bool ok = false;
        coord[i] = it.next().toFloat(&ok);
        if (!ok) return false;
    }

    return true;
}

bool parseFace(QString line, uint *vertices, uint *normals, bool &hasNormals) {
    line = line.simplified();

    QStringList tokens = line.split(" ", QString::SkipEmptyParts);
    //qDebug() << tokens;
    //Q_ASSERT(tokens.size() == 5);
    if (tokens.size() != 5)
        return false;

    hasNormals = false;
    QStringListIterator it(tokens);
    it.next();
    for (uint i = 0; i < 4; i++) {
        QString vertexStr = it.next();
        QStringList vertexTokens = vertexStr.split("/");
        //Q_ASSERT(vertexTokens.size() > 0 && vertexTokens.size() <= 3);
        ///Q_ASSERT(!hasNormals || vertexTokens.size() == 3); //has normals --> 3 vertex tokens

        bool validVertex = vertexTokens.size() > 0 && vertexTokens.size() <= 3;
        bool validNormals = !hasNormals || vertexTokens.size() == 3; //has normals --> 3 vertex tokens
        if (!validVertex || !validNormals)
            return false;

        bool ok = false;
        QStringListIterator vertexIt(vertexTokens);
        vertices[i] = vertexIt.next().toInt(&ok) - 1;

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

    //first pass to add all vertices and normals
    while (!file.atEnd()) {
        QByteArray lineBytes = file.readLine();

        if (lineBytes.at(0) == 'v' && lineBytes.at(1) == 'n') {
            Vector3f normal;

            if (!parseCoordinate(QString(lineBytes), normal)) {
                delete M;
                return false;
            }

            normals.push_back(normal);
        } else if (lineBytes.at(0) == 'v' && lineBytes.at(1) == ' ') {
            Vertex V;
            if (!parseCoordinate(QString(lineBytes), V.pos)) {
                delete M;
                return false;
            }
            M->m_vertices.push_back(V);
        }
    }

    //second pass to add faces
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
                Vector3f faceNormals[4];
                for (uint i = 0; i < 4; i++) {
                    if (N[i] < normals.size())
                        faceNormals[i] = normals[N[i]];
                }
                M->addFace(V[0],V[1],V[2],V[3], faceNormals);
            } else {
                M->addFace(V[0],V[1],V[2],V[3]);
            }
        }
    }

    return M;
}

Mesh Mesh::subdivide() {
    calculatePoints();

    Mesh M;
    for (uint i = 0; i < m_faces.size(); i++) {
        Face &f = m_faces[i];
        Vector3f V[4];
        Vector3f N[4];

        V[0] = m_facePoints[i];
        N[0] = m_facePointNormals[i];
        //for (uint j = 0; j < 4; j++) normals[0] = normals[0] + f.normals[j]/4.0;
        for (uint j = 0; j < 4; j++) {
            V[1] = m_edgePoints[f.edges[j]];
            V[2] = m_vertexPoints[f.vertices[(j+1)%4]];
            V[3] = m_edgePoints[f.edges[(j+1)%4]];

            N[1] = m_edgePointNormals[f.edges[j]];
            N[2] = m_vertexPointNormals[f.vertices[(j+1)%4]];
            N[3] = m_edgePointNormals[f.edges[(j+1)%4]];

            //normals[1] = interpolateNormal()

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
        /*if (F.size() != n) {
            qDebug() << F.size() << E.size();
            //print edges
            qDebug() << "ERROR:";
            for (uint j = 0; j < E.size(); j++) {
                qDebug() << "    Edge:" << m_edges[E[j]].vertices[0] << m_edges[E[j]].vertices[1];
            }

            //print faces
            for (uint j = 0; j < F.size(); j++) {
                Face &f = m_faces[F[j]];
                qDebug() << "    Face:" << f.vertices[0] << f.vertices[1] << f.vertices[2] << f.vertices[3];
            }
        }*/
        //Q_ASSERT(F.size() == n);

        Vector3f p = m_vertices[i].pos;
        Vector3f f(0,0,0); //average of adjacent face points
        Vector3f r(0,0,0); //average of adjacent edge midpoints

        for (uint j = 0; j < n; j++) f = f + m_facePoints[F[j]];
        f = f / n;

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

    for (uint i = 0; i < 4; i++) {
        f.vertices[i] = V[i];
        f.edges[i] = indexOf(V[i],V[(i+1)%4]);

        Edge &e = m_edges[f.edges[i]];
        /*if (e.numFaces >= 2) {
            Face &f1 = m_faces[e.faces[0]];
            Face &f2 = m_faces[e.faces[1]];

            qDebug() << "Bad Edge:" << e.vertices[0] << e.vertices[1];
            qDebug() << "    F1:" << f1.vertices[0] << f1.vertices[1] << f.vertices[2] << f.vertices[3];
            qDebug() << "    F2:" << f.vertices[0] << f.vertices[1] << f.vertices[2] << f.vertices[3];
        }*/

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


    /*for (uint i = 0; i < 4; i++) {
        Vertex &v = m_vertices[f.vertices[i]];
        if (v.normal.magnitude() == 0)
            v.normal = f.normals[i];
        else
            v.normal = v.normal*0.5 + f.normals[i]*0.5;

    }*/

    m_faces.push_back(f);
}

uint Mesh::indexOf(Vector3f p) {
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
    uint numVertices;
    const float *vertexBuffer = getVertexBuffer(numVertices);
    const float *normalBuffer = getNormalBuffer(numVertices);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
        if (vertexBuffer) glVertexPointer(3, GL_FLOAT, 0, vertexBuffer);
        if (normalBuffer) glNormalPointer(GL_FLOAT, 0, normalBuffer);
        glDrawArrays(GL_QUADS, 0, numVertices);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);


    /*glBegin(GL_QUADS);
    for (uint i = 0; i < m_faces.size(); i++) {
        Face &f = m_faces[i];

        for (uint j = 0; j < 4; j++) {
            Vertex &v = m_vertices[f.vertices[j]];
            Vector3f &p = v.pos;

            glNormal3f(f.normals[j].x, f.normals[j].y, f.normals[j].z);
            //glNormal3f(n.x, n.y, n.z);
            //glNormal3f(v.normal.x, v.normal.y, v.normal.z);
            glVertex3f(p[0],p[1],p[2]);
        }
    }
    glEnd();*/

    /*glDisable(GL_LIGHTING);
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    for (uint i = 0; i < m_faces.size(); i++) {
        Face &f = m_faces[i];

        Vector3f v1 = m_vertices[f.vertices[0]].pos - m_facePoints[i];
        Vector3f v2 = m_vertices[f.vertices[3]].pos - m_facePoints[i];
        Vector3f n = v1.cross(v2);
        n = n / n.magnitude() * -1;

        //glVertex3f(m_facePoints[i][0],m_facePoints[i][1],m_facePoints[i][2]);
        //glVertex3f(n.x, n.y, n.z);

        for (uint j = 0; j < 4; j++) {
            Vector3f &p = m_vertices[f.vertices[i]].pos;
            //Vector3f delta = p - m_facePoints[i];
            //Vector3f norm = n + delta;
            Vector3f norm = p + n;

            glVertex3f(p[0],p[1],p[2]);
            //glVertex3f(n.x, n.y, n.z);
            glVertex3f(norm.x,norm.y,norm.z);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);*/

    /*calculatePoints();

    glPointSize(5);
    glBegin(GL_POINTS);
    glColor3f(1,0,0);
    for (uint i = 0; i < m_facePoints.size(); i++) {
        Vector3f &p = m_facePoints[i];
        glVertex3f(p[0],p[1],p[2]);
    }

    glColor3f(0,1,0);
    for (uint i = 0; i < m_edgePoints.size(); i++) {
        Vector3f &p = m_edgePoints[i];
        glVertex3f(p[0],p[1],p[2]);
    }

    glColor3f(0,0,1);
    for (uint i = 0; i < m_vertexPoints.size(); i++) {
        Vector3f &p = m_vertexPoints[i];
        glVertex3f(p[0],p[1],p[2]);
    }
    glEnd();

    glColor3f(0,0,1);
    glBegin(GL_QUADS);
    for (uint i = 0; i < m_faces.size(); i++) {
        Face &f = m_faces[i];
        Vector3f V[4];

        V[0] = m_facePoints[i];
        for (uint j = 0; j < 4; j++) {
            V[1] = m_edgePoints[f.edges[j]];
            V[2] = m_vertexPoints[f.vertices[(j+1)%4]];
            V[3] = m_edgePoints[f.edges[(j+1)%4]];

            for (uint k = 0; k < 4; k++) {
                glVertex3f(V[k][0], V[k][1], V[k][2]);
            }
        }
    }
    glEnd();*/
}
