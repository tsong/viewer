#include "openglrenderer.h"
#include <QDebug>
#include <stdio.h>

OpenGLRenderer::OpenGLRenderer() {
    scene = 0;
    /*showAxis = false;
    showInfo = false;*/
}

OpenGLRenderer::~OpenGLRenderer() {}

void OpenGLRenderer::init(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    float w = width > height ? 2 : (float)width/height * 2;
    float h = height > width ? 2 : (float)height/width * 2;
    glFrustum(-w/2,w/2,-h/2,h/2,1,100);

    glMatrixMode(GL_MODELVIEW);
    glClearColor(0,0,0,0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    Light light(0.5f, 0.5f, 0.5f, 1.0f, 0.8f, 0.8f, 1.0, 1.0f, 1.0f, 0.6f, 0.6f, 1.0f, 10.0f, 10.0f, 10.0f);
    setLight(0,light);
}

void OpenGLRenderer::resize(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);

    float w = width > height ? 2 : (float)width/height * 2;
    float h = height > width ? 2 : (float)height/width * 2;
    glFrustum(-w/2,w/2,-h/2,h/2,1,100);

    glMatrixMode(GL_MODELVIEW);
    glClearColor(0,0,0,0);
}

void OpenGLRenderer::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glColor3f(1,1,1);

    //get camera
    Vector3f p = camera.toCartesian();
    Vector3f o = camera.getOrigin();

    gluLookAt(p.x, p.y, p.z, o.x, o.y, o.z, 0, 1, 0);

    if (scene) {
        scene->glDraw();
    }
}

void OpenGLRenderer::setLight(int i, Light light) {
    if (i >= MAX_GL_LIGHTS) return;

    GLenum lightTable[MAX_GL_LIGHTS] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};
    lights[i] = light;

    //get light colors
    Color a = lights[i].ambient;
    Color d = lights[i].diffuse;
    Color s = lights[i].specular;
    Vector3f p = lights[i].pos;

    //set up color and position buffers
    float ambient[4] = {a.r,a.g,a.b,a.a};
    float diffuse[4] = {d.r,d.g,d.b,d.a};
    float specular[4] = {s.r,s.g,s.b,s.a};
    float pos[4] = {p.x,p.y,p.z,p.w};

    glLightfv(lightTable[i], GL_AMBIENT, ambient);
    glLightfv(lightTable[i], GL_DIFFUSE, diffuse);
    glLightfv(lightTable[i], GL_SPECULAR, specular);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightfv(lightTable[i], GL_POSITION, pos);

    if (lights[i].isEnabled)
        glEnable(lightTable[i]);
    else
        glDisable(lightTable[i]);
}

void OpenGLRenderer::setLights(Light *lights, int n) {
    for (int i = 0; i < MAX(MAX_GL_LIGHTS,n); i++) setLight(i, lights[i]);
}

void OpenGLRenderer::setCamera(Camera camera) { this->camera = camera; }
void OpenGLRenderer::setScene(Scene *scene) { this->scene = scene; }

Camera OpenGLRenderer::getCamera() { return camera; }
int OpenGLRenderer::getNumLights() { return MAX_GL_LIGHTS; }
Light OpenGLRenderer::getLight(int i) { return lights[i]; }
