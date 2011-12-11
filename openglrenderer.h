#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <GL/gl.h>

#include "renderer.h"

#define MAX_GL_LIGHTS 8

class OpenGLRenderer : public Renderer {
    public:
        OpenGLRenderer();
        ~OpenGLRenderer();

        void init(int width, int height);
        void resize(int width, int height);
        void render();

        void setLights(Light *lights, int n);
        void setLight(int i, Light light);
        void setCamera(Camera camera);
        void setScene(Scene *scene);

        Camera getCamera();
        int getNumLights();
        Light getLight(int i);

    private:
        Light lights[MAX_GL_LIGHTS];
        Camera camera;

        Scene *scene;
};

#endif // OPENGLRENDERER_H
