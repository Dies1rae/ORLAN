#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include "sPoint.h"

#include <QMainWindow>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QtChartsVersion>
#include <QtCharts>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <vector>
#include <string>
#include <fstream>
#include <utility>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_exitPushButton_clicked();
    void on_loadPushButton_clicked();

    void on_switchPushButton_clicked();

private:
    void MainWindow::resetGraph();
    void MainWindow::axesPrint();
    void MainWindow::closeEvent(QCloseEvent *event);
    std::pair<std::string,std::string> splitIntoWord(const std::string& words);
private:
    Ui::MainWindow *ui;
    QtCharts::QLineSeries *graph;
    std::vector<Point> points;
};
#endif // MAINWINDOW_H
