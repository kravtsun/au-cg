#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "my_opengl_widget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->openGLWidget = new MyOpenGLWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}
