
#include <QDebug>
#include <math.h>
#include "glwidget.h"

#include <GL/glut.h>

GLWidget::GLWidget(Renderer *renderer, QWidget* parent)
    : QGLWidget(parent), m_renderer(renderer), m_renderMode(RENDER_MODE_DEFAULT),
      m_moveCamera(false), m_zoomCamera(false), m_showAxis(false), m_showInfo(false),
      m_phongShaders(0)
{
}

void GLWidget::initializeGL() {
    if (m_renderer)
        m_renderer->init(width(), height());

    //create shaders for phong shading
    //makeCurrent();
    m_phongShaders = new QGLShaderProgram(context(), this);
    m_phongShaders->addShaderFromSourceFile(QGLShader::Vertex, "phong.vsh");
    m_phongShaders->addShaderFromSourceFile(QGLShader::Fragment, "phong.fsh");
    m_phongShaders->link();
}

void GLWidget::resizeGL(int width, int height) {
    if (m_renderer)
        m_renderer->resize(width, height);

    /*glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0,0,width,height);
    glOrtho(-1,1,-1,1,-1,1);
    //glFrustum(-2,2,-2,2,-2,2);
    glMatrixMode(GL_MODELVIEW);*/

}

void GLWidget::paintGL() {
    //set render mode options
    switch(m_renderMode) {
    case RENDER_MODE_WIREFRAME:
        glDisable(GL_LIGHTING);
        m_phongShaders->release();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case RENDER_MODE_PHONG:
        glEnable(GL_LIGHTING);
        m_phongShaders->bind();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        glEnable(GL_LIGHTING);
        m_phongShaders->release();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }

    if (m_renderer)
        m_renderer->render();

    /*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    //glRotatef(45, 1, 1, 1);

    //Camera camera = m_renderer->getCamera();
    //camera.setRadial(1);
    //Vector3f p = camera.toCartesian();
    //gluLookAt(p.x, p.y, p.z, 0, 0, 0, 0, 1, 0);
    Camera camera = m_renderer->getCamera();
    glRotatef(camera.getZenith(), 1, 0, 0);
    glRotatef(camera.getAzimuth(), 0, 1, 0);

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f(1,0,0);
    mesh.glDraw();*/

    m_phongShaders->release();

    //draw axis
    if (m_showAxis) {
        glDisable(GL_LIGHTING);

        glBegin(GL_LINES);
            glColor3f(1,0,0);
            glVertex3f(0,0,0);
            glVertex3f(100,0,0);

            glColor3f(0,1,0);
            glVertex3f(0,0,0);
            glVertex3f(0,100,0);

            glColor3f(0,0,1);
            glVertex3f(0,0,0);
            glVertex3f(0,0,100);
        glEnd();
        glEnable(GL_LIGHTING);
    }

    //show info
    if (m_showInfo) {
        glColor3f(1,1,1);
        Camera camera = m_renderer->getCamera();
        Vector3f p = camera.toCartesian();

        QString info = QString("Camera: (%1, %2, %3)").arg(p.x, 0, 'f', 2).arg(p.y, 0, 'f', 2).arg(p.z, 0, 'f', 2);
        renderText(5,13,info);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton)
        m_moveCamera = true;

    if (event->buttons() & Qt::RightButton)
        m_zoomCamera = true;


   /* if (event->buttons() & Qt::MiddleButton) {
        mesh = mesh.subdivide();
        repaint();
    }*/

    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!(m_moveCamera || m_zoomCamera)) return;

    Camera camera = m_renderer->getCamera();

    QPoint pos = event->pos();
    float dx = pos.x() - m_lastPos.x();
    float dy = pos.y() - m_lastPos.y();
    m_lastPos = pos;

    if (m_moveCamera) {
        camera.setZenith(camera.getZenith() + dy);
        camera.setAzimuth(camera.getAzimuth() + dx);
    }

    if (m_zoomCamera) {
        camera.setRadial(camera.getRadial() + dy*.1);
    }

    m_renderer->setCamera(camera);
    repaint();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *) {
    m_moveCamera = false;
    m_zoomCamera = false;
}

void GLWidget::wheelEvent(QWheelEvent *event) {
    Camera camera = m_renderer->getCamera();
    camera.setRadial(camera.getRadial() - event->delta() * 0.002);
    m_renderer->setCamera(camera);

    repaint();
}

void GLWidget::setRenderMode(RenderMode renderMode) {
    m_renderMode = renderMode;
}

void GLWidget::setRenderer(Renderer *renderer) { m_renderer = renderer; }
Renderer *GLWidget::getRenderer() { return m_renderer; }

void GLWidget::setShowAxis(bool showAxis) { m_showAxis = showAxis; }
void GLWidget::setShowInfo(bool showInfo) { m_showInfo = showInfo; }

bool GLWidget::getShowAxis() { return m_showAxis; }
bool GLWidget::getShowInfo() { return m_showInfo; }



