#include "mainwindow.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

#include "lightdialog.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), scene(0), mesh(0) {
    //initialize main window attributes
    resize(WIDTH,HEIGHT);
    setWindowTitle("viewer");

    openGLRenderer = new OpenGLRenderer();
    scene = new Scene();

    //create glWidget
    glWidget = new GLWidget(openGLRenderer, this);
    glWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setCentralWidget(glWidget);

    lightDialog = new LightDialog(openGLRenderer, this);
    connect(lightDialog, SIGNAL(rejected()), glWidget, SLOT(repaint()));
    connect(lightDialog, SIGNAL(accepted()), glWidget, SLOT(repaint()));
    connect(lightDialog, SIGNAL(lightUpdated()), glWidget, SLOT(repaint()));

    cameraDialog = new CameraDialog(openGLRenderer, this);
    connect(cameraDialog, SIGNAL(rejected()), glWidget, SLOT(repaint()));
    connect(cameraDialog, SIGNAL(accepted()), glWidget, SLOT(repaint()));
    connect(cameraDialog, SIGNAL(cameraUpdated()), glWidget, SLOT(repaint()));

    createMenus();
}

MainWindow::~MainWindow() {
    delete scene;

    if (mesh) delete mesh;
}

void MainWindow::createMenus() {
    //file menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
        //open action
        QAction *openAct = new QAction("&Open OBJ", this);
        openAct->setStatusTip("Open OBJ file");
        openAct->setShortcut(QKeySequence("Ctrl+O"));
        this->connect(openAct, SIGNAL(triggered()), SLOT(open()));
        fileMenu->addAction(openAct);

        fileMenu->addSeparator();

        //exit action
        QAction *exitAct = new QAction("&Exit", this);
        exitAct->setStatusTip("Exit program");
        exitAct->setShortcut(QKeySequence("Ctrl+Q"));
        this->connect(exitAct, SIGNAL(triggered()), SLOT(close()));
        fileMenu->addAction(exitAct);


    QMenu *editMenu = menuBar()->addMenu("&Edit");
        //edit camera action
        QAction *editCameraAct = new QAction("&Camera", this);
        editCameraAct->setStatusTip("Edit Camera");
        editCameraAct->setShortcut(QKeySequence("Ctrl+C"));
        this->connect(editCameraAct, SIGNAL(triggered()), SLOT(editCamera()));
        editMenu->addAction(editCameraAct);

        //edit light action
        QAction *editLightAct = new QAction("&Light Sources", this);
        editLightAct->setStatusTip("Edit Light Sources");
        editLightAct->setShortcut(QKeySequence("Ctrl+L"));
        this->connect(editLightAct, SIGNAL(triggered()), SLOT(editLights()));
        editMenu->addAction(editLightAct);


    QMenu *viewMenu = menuBar()->addMenu("&Show");
        //view coordinates action
        QAction *viewInfoAct = new QAction("&Info", this);
        viewInfoAct->setStatusTip("Show Scene Information");
        viewInfoAct->setShortcut(QKeySequence("Shift+Ctrl+I"));
        this->connect(viewInfoAct, SIGNAL(triggered()), SLOT(showInfo()));
        viewMenu->addAction(viewInfoAct);

        //view axis action
        QAction *viewAxisAct = new QAction("&Axis", this);
        viewAxisAct->setStatusTip("Show Axis");
        viewAxisAct->setShortcut(QKeySequence("Shift+Ctrl+A"));
        this->connect(viewAxisAct, SIGNAL(triggered()), SLOT(showAxis()));
        viewMenu->addAction(viewAxisAct);

        //view full screen
        QAction *fullScreenAct = new QAction("&Fullscreen", this);
        fullScreenAct->setStatusTip("Fullscreen mode");
        fullScreenAct->setShortcut(QKeySequence("Ctrl+F"));
        this->connect(fullScreenAct, SIGNAL(triggered()), SLOT(toggleFullscreen()));
        viewMenu->addAction(fullScreenAct);

    QMenu *renderMenu = menuBar()->addMenu("&Render");
        //render default action
        QAction *renderDefaultAct = new QAction("&Default", this);
        renderDefaultAct->setStatusTip("Default");
        renderDefaultAct->setShortcut(QKeySequence("Shift+Ctrl+D"));
        this->connect(renderDefaultAct, SIGNAL(triggered()), SLOT(renderDefault()));
        renderMenu->addAction(renderDefaultAct);

        //render wireframe action
        QAction *renderWireAct = new QAction("&Wireframe", this);
        renderWireAct->setStatusTip("Wireframe");
        renderWireAct->setShortcut(QKeySequence("Shift+Ctrl+W"));
        this->connect(renderWireAct, SIGNAL(triggered()), SLOT(renderWireframe()));
        renderMenu->addAction(renderWireAct);

        //render phong action
        QAction *renderPhongAct = new QAction("&Phong", this);
        renderPhongAct->setStatusTip("Phong");
        renderPhongAct->setShortcut(QKeySequence("Shift+Ctrl+P"));
        this->connect(renderPhongAct, SIGNAL(triggered()), SLOT(renderPhong()));
        renderMenu->addAction(renderPhongAct);

    //subdivision steps
    QMenu *subdivideMenu = menuBar()->addMenu("&Subdivide");
    for (uint i = 0; i < 5; i++) {
        QAction *subdivideAct = new SubdivideAction(i, QString("%1").arg(i));
        this->connect(subdivideAct, SIGNAL(triggered(uint)), SLOT(subdivide(uint)));
        subdivideMenu->addAction(subdivideAct);
    }
}


void MainWindow::open() {
    //get file name from file dialog
    QString fileName = QFileDialog::getOpenFileName(this, "Load OBJ File", "", "OBJ Files (*.obj);;All files (*.*)");
    if (fileName == "") return;

    Mesh *newMesh = Mesh::fromObjFile(fileName);

    if (newMesh) {
        if (mesh) delete mesh;
        mesh = newMesh;

        mesh->unitize();
        scene->setMesh(mesh);
        glWidget->getRenderer()->setScene(scene);
    } else {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("Error opening file. Please ensure that the file is a quad mesh.");
        msgBox.exec();
    }

    /*GLMmodel* model = glmReadOBJ((char*)fileName.toAscii().constData());

    glmFacetNormals(model);
    if (!model->normals)
        glmVertexNormals(model,90);
    glmUnitize(model);

    Scene *scene = new Scene();
    scene->model = model;
    glWidget->getRenderer()->setScene(scene);*/

    //glWidget->model = model;
    glWidget->repaint();
}

void MainWindow::editLights() {
    lightDialog->exec();
}

void MainWindow::editCamera() {
    cameraDialog->exec();
}

void MainWindow::showAxis() {
    glWidget->setShowAxis( !glWidget->getShowAxis() );
    glWidget->repaint();
}

void MainWindow::renderDefault() {
    glWidget->setRenderMode(RENDER_MODE_DEFAULT);
    glWidget->repaint();
}

void MainWindow::renderWireframe() {
    glWidget->setRenderMode(RENDER_MODE_WIREFRAME);
    glWidget->repaint();
}

void MainWindow::renderPhong() {
    glWidget->setRenderMode(RENDER_MODE_PHONG);
    glWidget->repaint();
}

void MainWindow::showInfo() {
    glWidget->setShowInfo( !glWidget->getShowInfo() );
    glWidget->repaint();
}

void MainWindow::toggleFullscreen() {
    if (!isFullScreen())
        showFullScreen();
    else
        showNormal();
}

void MainWindow::subdivide(uint steps) {
    if (scene)
        scene->subdivide(steps);
    glWidget->repaint();
}
