#include "imagedialog.h"
#include <QPixmap>
#include <QScreen>
#include <QApplication>

ImageDialog::ImageDialog(QWidget *parent) : QDialog(parent) {

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *imageLayout = new QHBoxLayout();
    label1 = new QLabel(this);
    label2 = new QLabel(this);
    buttonLayout = new QHBoxLayout();
    button1 = new QPushButton("Button 1", this);
    button2 = new QPushButton("Button 2", this);
    connect(button1, &QPushButton::clicked, this, &ImageDialog::onButtonClickedyes);
    connect(button2, &QPushButton::clicked, this, &ImageDialog::onButtonClickedno);


    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        int halfScreenWidth = screen->size().width() / 2;
        label1->setMinimumWidth(halfScreenWidth);
        label2->setMinimumWidth(halfScreenWidth);

        label1->setMinimumHeight(1000);
        label2->setMinimumHeight(1000);
    }

    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);


    imageLayout->addWidget(label1);
    imageLayout->addWidget(label2);

    mainLayout->addLayout(imageLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

ImageDialog::~ImageDialog() {

}

void ImageDialog::setImagePaths(const QString &path1, const QString &path2) {

    QPixmap pixmap1(path1);
    QPixmap pixmap2(path2);
    label1->setPixmap(pixmap1.scaled(label1->size(), Qt::KeepAspectRatio));
    label2->setPixmap(pixmap2.scaled(label2->size(), Qt::KeepAspectRatio));
}

void ImageDialog::onButtonClickedyes() {
    this->close();
}

void ImageDialog::onButtonClickedno() {
    this->close();
}
