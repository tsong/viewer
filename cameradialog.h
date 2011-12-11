#ifndef CAMERADIALOG_H
#define CAMERADIALOG_H

#include <QDialog>
#include "ui_cameradialog.h"
#include "renderer.h"

#define ORIGIN_MAX_X 3
#define ORIGIN_MAX_Y 3
#define ORIGIN_MAX_Z 3

class CameraDialog : public QDialog, public Ui_cameraDialog {
    Q_OBJECT

    public:
        CameraDialog(Renderer *renderer, QWidget *parent);

    signals:
        void cameraUpdated();

    public slots:
        void exec();
        void reject();

    protected slots:
        void updateCamera();
        void boundsChanged();

    private:
        void connectControls();
        void disconnectControls();

        Renderer *renderer;
        Camera camera;
        Camera oldCamera;
};

#endif // CAMERADIALOG_H
