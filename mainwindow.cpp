#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    windowSortingSequence.show();
}


void MainWindow::on_pushButton_2_clicked()
{
    windowTestingSorts.show();
}


void MainWindow::on_pushButton_3_clicked()
{
    windowSortVisualization.show();
}

