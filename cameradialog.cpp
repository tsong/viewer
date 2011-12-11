#include "cameradialog.h"
#include <QDebug>

CameraDialog::CameraDialog(Renderer *renderer, QWidget *parent=0)
    : QDialog(parent)
{
    Ui_cameraDialog::setupUi(this);
    this->renderer = renderer;

    QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    /*ox->setValidator(doubleValidator);
    oy->setValidator(doubleValidator);
    oz->setValidator(doubleValidator);*/
    min->setValidator(doubleValidator);
    max->setValidator(doubleValidator);
}

void CameraDialog::connectControls() {
    connect(theta, SIGNAL(valueChanged(int)), SLOT(updateCamera()));
    connect(phi, SIGNAL(valueChanged(int)), SLOT(updateCamera()));
    connect(rho, SIGNAL(valueChanged(int)), SLOT(updateCamera()));

    connect(ox, SIGNAL(valueChanged(int)), SLOT(updateCamera()));
    connect(oy, SIGNAL(valueChanged(int)), SLOT(updateCamera()));
    connect(oz, SIGNAL(valueChanged(int)), SLOT(updateCamera()));

    connect(min, SIGNAL(textChanged(QString)), SLOT(boundsChanged()));
    connect(max, SIGNAL(textChanged(QString)), SLOT(boundsChanged()));
}

void CameraDialog::disconnectControls() {
    theta->disconnect();
    phi->disconnect();
    rho->disconnect();

    ox->disconnect();
    oy->disconnect();
    oz->disconnect();

    min->disconnect();
    max->disconnect();
}

void CameraDialog::boundsChanged() {
    camera.setRadialBounds( min->text().toFloat(), max->text().toFloat() );
    renderer->setCamera(camera);

    rho->setMinimum((int)camera.getMinRadial());
    rho->setMaximum((int)camera.getMaxRadial());

    min->setText(QString::number(camera.getMinRadial()));
    max->setText(QString::number(camera.getMaxRadial()));
}

void CameraDialog::updateCamera() {
    camera.setAzimuth( theta->value() );
    camera.setZenith( phi->value() );
    camera.setRadial( rho->value() );

    Vector3f o;
    o.x = ox->value() / 100.0 * ORIGIN_MAX_X;
    o.y = oy->value() / 100.0 * ORIGIN_MAX_Y;
    o.z = oz->value() / 100.0 * ORIGIN_MAX_Z;

    camera.setOrigin(o);

    renderer->setCamera(camera);
    emit cameraUpdated();
}

void CameraDialog::exec() {
    camera = renderer->getCamera();
    oldCamera = camera;

    disconnectControls();

    theta->setValue(camera.getAzimuth());
    phi->setValue(camera.getZenith());

    rho->setMinimum((int)camera.getMinRadial());
    rho->setMaximum((int)camera.getMaxRadial());
    rho->setValue(camera.getRadial());

    Vector3f o = camera.getOrigin();
    ox->setValue( (int)(o.x / ORIGIN_MAX_X * 100) );
    oy->setValue( (int)(o.y / ORIGIN_MAX_Y * 100) );
    oz->setValue( (int)(o.z / ORIGIN_MAX_Z * 100) );

    min->setText( QString::number(camera.getMinRadial()) );
    max->setText( QString::number(camera.getMaxRadial()) );

    connectControls();

    QDialog::exec();
}

void CameraDialog::reject() {
    renderer->setCamera(oldCamera);
    QDialog::reject();
}
