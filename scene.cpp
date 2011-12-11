#include "scene.h"

Scene::Scene(): m_mesh(0), m_subdivisionSteps(0) {}

void Scene::setMesh(Mesh *mesh) {
    m_mesh = mesh;
    m_subdividedMesh = *m_mesh;
}

Mesh *Scene::getMesh() {
    return m_mesh;
}

void Scene::glDraw() {
    if (m_mesh) {
        if (m_subdivisionSteps > 0)
            m_subdividedMesh.glDraw();
        else
            m_mesh->glDraw();
    }
}

void Scene::subdivide(uint steps) {
    if (m_subdivisionSteps == steps) return;
    if (m_subdivisionSteps > steps) {
        m_subdivisionSteps = 0;
        m_subdividedMesh = *m_mesh;
    }

    for (; m_subdivisionSteps < steps; m_subdivisionSteps++) {
        m_subdividedMesh = m_subdividedMesh.subdivide();
    }
}
