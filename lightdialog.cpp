#include "lightdialog.h"
#include <QRegExpValidator>
#include <QDoubleValidator>
#include <QDebug>

LightDialog::LightDialog(Renderer *renderer, QWidget *parent) : QDialog(parent) {
    Ui_lightDialog::setupUi(this);

    lightSpinBox->setRange(0,NUM_LIGHTS-1);
    currLight = lightSpinBox->value();

    //QRegExpValidator *rgbValidator = new QRegExpValidator(QRegExp("^(-{0,1}[01]|-{0,1}[01]{0,1}\\.\\d{1,8})$"), this);

    /*ar->setValidator(rgbValidator);
    ag->setValidator(rgbValidator);
    ab->setValidator(rgbValidator);

    dr->setValidator(rgbValidator);
    dg->setValidator(rgbValidator);
    db->setValidator(rgbValidator);

    sr->setValidator(rgbValidator);
    sg->setValidator(rgbValidator);
    sb->setValidator(rgbValidator);*/

    /*QDoubleValidator *doubleValidator = new QDoubleValidator(this);
    px->setValidator(doubleValidator);
    py->setValidator(doubleValidator);
    pz->setValidator(doubleValidator);*/

    typeComboBox->addItem("Directional");
    typeComboBox->addItem("Positional");

    this->renderer = renderer;
    connectControls();
}

void LightDialog::connectControls() {
    connect(lightSpinBox, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(lightSpinBox, SIGNAL(valueChanged(int)), SLOT(lightChanged(int)));

    connect(ar, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(ag, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(ab, SIGNAL(valueChanged(int)), SLOT(saveLight()));

    connect(dr, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(dg, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(db, SIGNAL(valueChanged(int)), SLOT(saveLight()));

    connect(sr, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(sg, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(sb, SIGNAL(valueChanged(int)), SLOT(saveLight()));

    connect(px, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(py, SIGNAL(valueChanged(int)), SLOT(saveLight()));
    connect(pz, SIGNAL(valueChanged(int)), SLOT(saveLight()));

    connect(lightEnable, SIGNAL(clicked()), SLOT(saveLight()));
    connect(typeComboBox, SIGNAL(currentIndexChanged(int)), SLOT(saveLight()));
}

void LightDialog::disconnectControls() {
    lightSpinBox->disconnect();

    ar->disconnect();
    ag->disconnect();
    ab->disconnect();

    dr->disconnect();
    dg->disconnect();
    db->disconnect();

    sr->disconnect();
    sg->disconnect();
    sb->disconnect();

    px->disconnect();
    py->disconnect();
    pz->disconnect();

    lightEnable->disconnect();
    typeComboBox->disconnect();
}

void LightDialog::lightChanged(int i) {
    disconnectControls();

    currLight = i;
    lightSpinBox->setValue(currLight);

    //load new light
    Light light = renderer->getLight(i);
    lightEnable->setChecked(light.isEnabled);

    Color a = light.ambient;
    Color d = light.diffuse;
    Color s = light.specular;
    Vector3f p = light.pos;

    ar->setValue(int(a.r*100)); ag->setValue(int(a.g*100)); ab->setValue(int(a.b*100));
    dr->setValue(int(d.r*100)); dg->setValue(int(d.g*100)); db->setValue(int(d.b*100));
    sr->setValue(int(s.r*100)); sg->setValue(int(s.g*100)); sb->setValue(int(s.b*100));

    px->setValue( int(p.x / LIGHT_MAX_X * 100) );
    py->setValue( int(p.y / LIGHT_MAX_Y * 100) );
    pz->setValue( int(p.z / LIGHT_MAX_Z * 100) );
    //px->setText(QString::number(p.x)); py->setText(QString::number(p.y)); pz->setText(QString::number(p.z));

    typeComboBox->setCurrentIndex((int)p.w);

    connectControls();
}

void LightDialog::saveLight() {
    //int type = typeComboBox->currentIndex();

    Color a;
    a.r = ar->value() / 100.0;
    a.g = ag->value() / 100.0;
    a.b = ab->value() / 100.0;
    a.a = 1.0;

    Color d;
    d.r = dr->value() / 100.0;
    d.g = dg->value() / 100.0;
    d.b = db->value() / 100.0;
    d.a = 1.0;

    Color s;
    s.r = sr->value() / 100.0;
    s.g = sg->value() / 100.0;
    s.b = sb->value() / 100.0;
    s.a = 1.0;

    Vector3f p;
    p.x = px->value() / 100.0 * LIGHT_MAX_X;
    p.y = py->value() / 100.0 * LIGHT_MAX_Y;
    p.z = pz->value() / 100.0 * LIGHT_MAX_Z;

    Light light(a,d,s,p);
    light.isEnabled = lightEnable->isChecked();

    renderer->setLight(currLight, light);

    emit lightUpdated();
}

void LightDialog::exec() {
    for (int i = 0; i < renderer->getNumLights(); i++) {
        oldLights[i] = renderer->getLight(i);
    }

    lightChanged(currLight);
    QDialog::exec();
}

void LightDialog::reject() {
    renderer->setLights(oldLights,renderer->getNumLights());
    QDialog::reject();
}
