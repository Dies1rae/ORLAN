#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#pragma once

#include "common.h"

#include <QMainWindow>
#include <QtWidgets>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QPixmap>
#include <vector>
#include <fstream>
#include <utility>
#include <algorithm>
#include <math.h>

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
    void on_savePushButton_clicked();
    void on_clearDataPushButton_clicked();
private:
    void on_loadFormBin(const QString& filepath);
    void on_loadFormTxt(const QString& filepath);
    void closeEvent(QCloseEvent *event);
    std::pair<std::string,std::string> splitIntoWord(const std::string& words);
    std::string calculatePointHsh();
    void fillPointsInfo();
    void drawPointonGraph(const double x, const double y, const QPen& paintpen);
    void drawAxes();
    void clearData();
    void clearGraphic();
    void resizeGraphic();
    void switchParamToDefault();
private:
    Ui::MainWindow *ui;
    const QIcon reaper_ico {"./ripper_ico.ico"};
    std::vector<Point> points;
    PointInfo info;
    QImage graph;
    graphSizes graph_size;
};
#endif // MAINWINDOW_H
