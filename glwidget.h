#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QGLShaderProgram>

#include "camera.h"
#include "types.h"
#include "light.h"

#include "renderer.h"

typedef enum RenderMode {
    RENDER_MODE_DEFAULT,
    RENDER_MODE_WIREFRAME,
    RENDER_MODE_PHONG
} RenderMode;


/*Widget to display graphics and animation*/
class GLWidget : public QGLWidget {
    Q_OBJECT

    public:
        GLWidget(Renderer *renderer = 0, QWidget* parent = 0);

        /*OpenGL*/
        void paintGL();
        void resizeGL(int width, int height);
        void initializeGL();

        /*Mouse input*/
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);

        void setRenderer(Renderer *renderer);
        Renderer *getRenderer();

        void setShowAxis(bool showAxis);
        void setShowInfo(bool showInfo);

        bool getShowAxis();
        bool getShowInfo();

        void setRenderMode(RenderMode renderMode);

    private:
        Renderer *m_renderer;
        RenderMode m_renderMode;

        bool m_moveCamera;
        bool m_zoomCamera;
        QPoint m_lastPos;

        bool m_showAxis;
        bool m_showInfo;

        QGLShaderProgram *m_phongShaders;
};

#endif // GLWIDGET_H
