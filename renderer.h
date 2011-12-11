#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"
#include "light.h"
#include "camera.h"

#include "scene.h"

class Renderer {
    public:
        virtual ~Renderer() {}

        virtual void init(int width, int height) = 0;
        virtual void resize(int width, int height) = 0;
        virtual void render() = 0;

        virtual void setLights(Light *lights, int n) = 0;
        virtual void setLight(int i, Light light) = 0;
        virtual void setCamera(Camera camera) = 0;

        virtual void setScene(Scene *scene) = 0;
       /* virtual void setShowAxis(bool showAxis) = 0;
        virtual void setShowInfo(bool showInfo) = 0;*/

        virtual Camera getCamera() = 0;
        /*virtual bool getShowAxis() = 0;
        virtual bool getShowInfo() = 0;*/

        virtual int getNumLights() = 0;
        virtual Light getLight(int i) = 0;
};


#endif // RENDERER_H
