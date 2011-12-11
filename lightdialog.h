#ifndef LIGHTDIALOG_H
#define LIGHTDIALOG_H

#include <QDialog>
#include "ui_lightdialog.h"
#include "light.h"
#include "types.h"
#include "renderer.h"

#define LIGHT_MAX_X 100
#define LIGHT_MAX_Y 100
#define LIGHT_MAX_Z 100


class LightDialog : public QDialog, public Ui_lightDialog {
    Q_OBJECT

    public:
        LightDialog(Renderer* renderer, QWidget *parent);

    signals:
        void lightUpdated();

    public slots:
        void exec();
        void reject();

    protected slots:
        void lightChanged(int i);
        void saveLight();

    private:
        void connectControls();
        void disconnectControls();

        Light oldLights[NUM_LIGHTS];
        Renderer *renderer;
        int currLight;
};

#endif // LIGHTDIALOG_H
