#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>

#include "glwidget.h"
#include "lightdialog.h"
#include "cameradialog.h"

#include "openglrenderer.h"
#include "scene.h"
#include "mesh.h"

#define WIDTH 600
#define HEIGHT 600

class SubdivideAction : public QAction {
    Q_OBJECT

public:
    SubdivideAction(uint steps, const QString &text, QObject *parent = 0)
        : QAction(text, parent), m_steps(steps)
    {
        connect(this, SIGNAL(triggered()), this, SLOT(emitTriggered()));
    }

signals:
    void triggered(uint steps);

protected slots:
    void emitTriggered() { emit triggered(m_steps); }

protected:
    uint m_steps;
};

/*Main window of program*/
class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        MainWindow(QWidget* parent = 0);
        ~MainWindow();

    protected slots:
        void open();
        void editLights();
        void editCamera();
        void showAxis();
        void renderDefault();
        void renderWireframe();
        void renderPhong();
        void showInfo();
        void toggleFullscreen();
        void subdivide(uint steps);

    private:
        void createMenus();

        GLWidget* glWidget;         //drawing panel
        LightDialog *lightDialog;
        CameraDialog *cameraDialog;

        OpenGLRenderer *openGLRenderer;
        Scene *scene;
        Mesh *mesh;
};

#endif // MAINWINDOW_H
