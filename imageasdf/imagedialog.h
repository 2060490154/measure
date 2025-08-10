// ImageDialog.h
#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class ImageDialog : public QDialog {
    Q_OBJECT

public:
    explicit ImageDialog(QWidget *parent = nullptr);
    ~ImageDialog();
    void setImagePaths(const QString &path1, const QString &path2);

private slots:
    void onButtonClickedyes();
    void onButtonClickedno();
private:
    QLabel *label1, *label2;
    QPushButton *button1, *button2;
    QHBoxLayout *buttonLayout;
    QVBoxLayout *mainLayout;
};

#endif // IMAGEDIALOG_H
