#ifndef SCENE_H
#define SCENE_H

#include "mesh.h"

class Scene {
public:
    Scene();
    void setMesh(Mesh *mesh);
    Mesh *getMesh();
    void glDraw();
    //GLMmodel *model;

    void subdivide(uint steps);
protected:
    Mesh *m_mesh;
    Mesh m_subdividedMesh;
    uint m_subdivisionSteps;
};

#endif // SCENE_H
