# -------------------------------------------------
# Project created by QtCreator 2010-11-18T19:18:27
# -------------------------------------------------
QT += opengl
TARGET = viewer
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    utils/glutils.cpp \
    utils/pointutils.cpp \
    glwidget.cpp \
    camera.cpp \
    lightdialog.cpp \
    light.cpp \
    openglrenderer.cpp \
    scene.cpp \
    cameradialog.cpp \
    mesh.cpp
HEADERS += mainwindow.h \
    glwidget.h \
    utils/matrix.h \
    utils/vector.h \
    utils/glutils.h \
    utils/pointutils.h \
    camera.h \
    lightdialog.h \
    light.h \
    types.h \
    openglrenderer.h \
    renderer.h \
    scene.h \
    cameradialog.h \
    mesh.h
FORMS += lightdialog.ui \
    cameradialog.ui

OTHER_FILES += \
    phong.vsh \
    phong.fsh
