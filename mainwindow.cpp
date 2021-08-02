#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toDIgit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->coordTableWidget->verticalHeader()->setVisible(true);
    ui->coordTableWidget->setShowGrid(true);

    this->resetGraph();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent (QCloseEvent *event)
{
     QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Coordinate ripper",
                                                                   tr("Are you sure?\n"),
                                                                   QMessageBox::No | QMessageBox::Yes,
                                                                   QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        event->accept();
    }
}

//--------------- VOT TUT TOZHE KOSTIL
void MainWindow::resetGraph()
{
    QtCharts::QChart * graph = new QtCharts::QChart();
    graph->legend()->hide();
    QtCharts::QValueAxis *axisX = new QtCharts::QValueAxis();
    QtCharts::QValueAxis *axisY = new QtCharts::QValueAxis();
    graph->addAxis(axisX, Qt::AlignLeft);
    graph->addAxis(axisY, Qt::AlignBottom);

    this->graph = new QtCharts::QLineSeries(graph);
    graph->addSeries(this->graph);
    this->graph->attachAxis(axisX);
    this->graph->attachAxis(axisY);
    axisX->setRange(-50.0,50.0); //!
    axisY->setRange(-50.0,50.0); //!
    this->graph->setUseOpenGL(true);
    QtCharts::QChartView* graphic_ = new QtCharts::QChartView(graph);
    this->axesPrint();
    ui->graphLayout->addWidget(graphic_);
}

void MainWindow::axesPrint()
{
    for(int ptr = 0; ptr < 50; ptr ++){ //!
        this->graph->append(0.0,ptr);
    }
    for(int ptr = 0; ptr < 51; ptr ++){ //!
        this->graph->append(ptr,0.0);
    }

    for(int ptr = 0; ptr > -50; ptr --){ //!
        this->graph->append(ptr,0.0);
    }
    for(int ptr = 0; ptr > -50; ptr --){ //!
        this->graph->append(0.0,ptr);
    }
}
//---------------------

void MainWindow::on_exitPushButton_clicked()
{
   this->close();
}

std::pair<std::string,std::string> MainWindow::splitIntoWord(const std::string& words) {
    const char* forbidden {";"};
    const auto idx_strt(words.find_first_not_of(forbidden));
    const auto idx_end(words.find_first_of(forbidden));
    std::string x = words.substr(idx_strt, idx_end - idx_strt);
    std::string y = words.substr(idx_end - idx_strt + 1);
    return { x, y };
}

void MainWindow::on_loadPushButton_clicked()
{
    QString coord_filepath = QFileDialog::getOpenFileName(this, tr("Open File"), "./", "TXT Files (*.txt);; All files (*.*)");
    std::ifstream coord_file(coord_filepath.toStdString(), std::ios::in);
    std::vector<std::pair<std::string,std::string>> parsed_str_coords;
    while(!coord_file.eof()){
        std::string tmp_coords_pair;
        std::getline(coord_file, tmp_coords_pair);
        if(!tmp_coords_pair.empty()) {
            try {
                std::pair<std::string,std::string> pair_std_coords = this->splitIntoWord(tmp_coords_pair);
                this->points.push_back({ digcnv::toDigit(pair_std_coords.first).AsDouble(), digcnv::toDigit(pair_std_coords.second).AsDouble() });
            } catch(...){
                std::cerr << "Load from file error" << std::endl;
            }
        }
    }
    for(size_t ptr = 0; ptr < this->points.size(); ptr++){
        QTableWidgetItem * item_x = new QTableWidgetItem(QString::number(this->points[ptr].x_));
        QTableWidgetItem * item_y = new QTableWidgetItem( QString::number(this->points[ptr].y_));
        ui->coordTableWidget->insertRow(ptr);
        ui->coordTableWidget->setItem(ptr,0,item_x);
        ui->coordTableWidget->setItem(ptr,1,item_y);
    }

    ui->coordTableWidget->show();

    coord_file.close();
}


void MainWindow::on_switchPushButton_clicked()
{
    for(auto& pr : this->points) {
        std::swap(pr.x_, pr.y_);
    }

    ui->coordTableWidget->clear();
    for(size_t ptr = 0; ptr < this->points.size(); ptr++){
        QTableWidgetItem * item_x = new QTableWidgetItem(QString::number(this->points[ptr].x_));
        QTableWidgetItem * item_y = new QTableWidgetItem( QString::number(this->points[ptr].y_));
        ui->coordTableWidget->setItem(ptr,0,item_x);
        ui->coordTableWidget->setItem(ptr,1,item_y);
    }

    ui->coordTableWidget->show();
}
