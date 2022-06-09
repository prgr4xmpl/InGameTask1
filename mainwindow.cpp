#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    ui->eqautorSlider->setTickPosition();
    ui->equatorSlider->setMaximum(90);
    ui->equatorSlider->setValue(8);

//    ui->greenwichSlider->setTickPosition();
    ui->greenwichSlider->setMaximum(90);
    ui->greenwichSlider->setValue(23);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::equatorSliderMoved(int pos) {
    qDebug() << "MainWindow::equatorSliderMoved" << pos;
    ui->moonAndEarthWidget->equatorAngle = (float)pos;
}

void MainWindow::greenwichSliderMoved(int pos) {
    qDebug() << "MainWindow::greenwichSliderMoved" << pos;
    ui->moonAndEarthWidget->greenwichAngle = (float)pos;
}

void MainWindow::moonOrbiteSliderMoved(int pos) {
    qDebug() << "MainWindow::moonOrbiteSliderMoved" << pos;
    ui->moonAndEarthWidget->moonSliderCoefficient = 1.0 + (float)pos / 100.0 * 2.0;
}
