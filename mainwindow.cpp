#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "toDIgit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Coordinate ripper");
    this->setWindowIcon(this->reaper_ico);
    ui->coordTableWidget->verticalHeader()->setVisible(true);
    ui->coordTableWidget->setShowGrid(true);
    ui->graphLabel->setMinimumSize(this->graph_size.default_x, this->graph_size.default_y);
    this->clearGraphic();
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

void MainWindow::clearGraphic()
{

    this->graph = QImage(QSize(this->graph_size.default_x, this->graph_size.default_y), QImage::Format_RGB32);
    this->graph.fill(Qt::white);
    this->drawAxes();
    ui->graphLabel->setPixmap(QPixmap::fromImage(this->graph));
}

void MainWindow::drawPointonGraph(const double x, const double y, const QPen& paintpen)
{
    QPainter painter(&this->graph);
    QPoint p1;
    p1.setX(this->graph_size.default_x / 2 + x);
    p1.setY(this->graph_size.default_y / 2 - y);
    painter.setPen(paintpen);
    painter.drawPoint(p1);
    ui->graphLabel->setPixmap(QPixmap::fromImage(this->graph));
}

void MainWindow::drawAxes()
{
    QPainter painter(&this->graph);
    QPen paintpen(Qt::black);
    paintpen.setWidth(this->graph_size.axes_pen_width);
    QPoint x1{0, (this->graph_size.default_y / 2)};
    QPoint x2{this->graph_size.default_x, (this->graph_size.default_y / 2)};
    QLine x_axe{x1, x2};
    QPoint y1{(this->graph_size.default_x / 2), 0};
    QPoint y2{(this->graph_size.default_x / 2), this->graph_size.default_y};
    QLine y_axe{y1, y2};
    painter.setPen(paintpen);
    painter.drawLine(x_axe);
    painter.drawLine(y_axe);
}

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
    this->clearData();
    QString coord_filepath = QFileDialog::getOpenFileName(this, tr("Open File"), "./", "TXT Files (*.txt);; BIN Files (*.bin);; All files (*.*)");
    if(coord_filepath.toStdString().find(".bin") != std::string::npos) {
        this->on_loadFormBin(coord_filepath);

    } else {
        this->on_loadFormTxt(coord_filepath);
    }
    this->switchParamToDefault();
    this->resizeGraphic();
    this->clearGraphic();
    for(size_t ptr = 0; ptr < this->points.size(); ptr++){
        QTableWidgetItem * item_x = new QTableWidgetItem(QString::number(this->points[ptr].x_));
        QTableWidgetItem * item_y = new QTableWidgetItem( QString::number(this->points[ptr].y_));
        ui->coordTableWidget->insertRow(ptr);
        ui->coordTableWidget->setItem(ptr,0,item_x);
        ui->coordTableWidget->setItem(ptr,1,item_y);
        QPen paintpen(Qt::red);
        paintpen.setWidth(this->graph_size.point_pen_width);
        this->drawPointonGraph(this->points[ptr].x_, this->points[ptr].y_, paintpen);
    }
    ui->coordTableWidget->show();
}

void MainWindow::on_loadFormBin(const QString& filepath)
{
    std::ifstream coord_file(filepath.toStdString(), std::ios::in);
    if(!coord_file) {
        QMessageBox::information(this, tr("Error"),  "No data to open");
        return;
    }
    PointInfo binLoadedInfo;
    std::string tmp_info;
    std::getline(coord_file, tmp_info);
    std::getline(coord_file, tmp_info);
    std::getline(coord_file, tmp_info);
    auto delim = tmp_info.find(':');
    delim = tmp_info.find(':', delim + 1);
    binLoadedInfo.size_ =  digcnv::toDigit(tmp_info.substr(delim + 1, 1)).AsInt();
    binLoadedInfo.pair_size_ = digcnv::toDigit(tmp_info.substr(delim + 1, tmp_info.rfind(':') - delim - 1)).AsInt();
    binLoadedInfo.data_hash_ = tmp_info.substr(tmp_info.rfind(':') + 1);
    std::getline(coord_file, tmp_info);
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
    coord_file.close();
    this->fillPointsInfo();
    if(binLoadedInfo.data_hash_ != this->info.data_hash_) {
         std::cerr << "Load from file error" << std::endl;
         QMessageBox::information(this, tr("Error"),  "Data corrupted");
         return;
    } else if(binLoadedInfo.data_hash_ == this->info.data_hash_) {
        QMessageBox::information(this, tr("Information"),  "Data restored complete");
    }
}

void MainWindow::on_loadFormTxt(const QString& filepath)
{
    std::ifstream coord_file(filepath.toStdString(), std::ios::in);
    if(!coord_file) {
        QMessageBox::information(this, tr("Error"),  "No data to open");
        return;
    }
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
    coord_file.close();
    this->fillPointsInfo();
}

void MainWindow::on_switchPushButton_clicked()
{
    for(auto& pr : this->points) {
        std::swap(pr.x_, pr.y_);
    }
    this->switchParamToDefault();
    this->resizeGraphic();
    this->clearGraphic();
    ui->coordTableWidget->clear();
    for(size_t ptr = 0; ptr < this->points.size(); ptr++){
        QTableWidgetItem * item_x = new QTableWidgetItem(QString::number(this->points[ptr].x_));
        QTableWidgetItem * item_y = new QTableWidgetItem( QString::number(this->points[ptr].y_));
        ui->coordTableWidget->setItem(ptr,0,item_x);
        ui->coordTableWidget->setItem(ptr,1,item_y);
        QPen paintpen(Qt::red);
        paintpen.setWidth(this->graph_size.point_pen_width);
        this->drawPointonGraph(this->points[ptr].x_, this->points[ptr].y_, paintpen);
    }
    ui->coordTableWidget->show();
}

void MainWindow::fillPointsInfo()
{
    this->info.size_ = this->points.size();
    this->info.pair_size_ = this->points.size() / 2;
    this->info.data_hash_ = this->calculatePointHsh();
}

std::string MainWindow::calculatePointHsh()
{
    size_t hsh = 0;
    size_t deg = 0;
    for(const auto& p : this->points){
        deg++;
        hsh += std::pow(p.x_, deg) + std::pow(p.y_, deg + 1);
    }
    return std::to_string(hsh * this->info.hasher_salt_);
}

void MainWindow::on_savePushButton_clicked()
{
    if(!this->points.empty()) {
        QString dst_filepath = QFileDialog::getSaveFileName(this, tr("Save File"), "./", "BIN Files (*.bin);; All files (*.*)");
        std::ofstream dst_bin;
        dst_bin.open(dst_filepath.toStdString(), std::ofstream::out | std::ofstream::binary);
        if(!dst_bin) {
            std::cerr << "File save error" << std::endl;
            QMessageBox::information(this, tr("Error"),  "File save error");
        }
        this->fillPointsInfo();
        dst_bin << "info\n";
        dst_bin << "doubleXY\n";
        dst_bin << this->info.hasher_salt_ << ':';
        dst_bin << this->info.size_ << ':';
        dst_bin << this->info.pair_size_ << ':';
        dst_bin << this->info.data_hash_ << '\n';
        dst_bin << "data\n";
        for(const auto& pt : this->points) {
            dst_bin << pt.x_ << ';' << pt.y_ << '\n';
        }
        dst_bin.close();
    } else {
        QMessageBox::information(this, tr("Error"),  "No data to save");
    }
}

void MainWindow::clearData()
{
    ui->coordTableWidget->clear();
    ui->coordTableWidget->setRowCount(0);
    this->points.clear();
    this->clearGraphic();
}

void MainWindow::on_clearDataPushButton_clicked()
{
     this->clearData();
}

//This func is really bad
void MainWindow::resizeGraphic()
{
    int minX = INT_MAX;
    int maxX = 0;
    int minY = INT_MAX;
    int maxY = 0;
    for(size_t ptr = 0; ptr < this->points.size(); ptr++) {
        if(this->points[ptr].x_ > maxX) {
            maxX = this->points[ptr].x_;
        }
        if(this->points[ptr].x_ < minX) {
            minX = this->points[ptr].x_;
        }
        if(this->points[ptr].y_ > maxY) {
            maxY = this->points[ptr].y_;
        }
        if(this->points[ptr].y_ < minY) {
            minY = this->points[ptr].y_;
        }
    }

    if(maxX > this->graph_size.default_x / 2) {
        while(maxX > this->graph_size.default_x / 2) {
            this->graph_size.default_x += this->graph_size.default_x;
            this->graph_size.point_pen_width++;
            this->graph_size.axes_pen_width++;
        }
    }
    if(minX < -1 * (this->graph_size.default_x / 2)) {
        while(minX < -1 * (this->graph_size.default_x / 2)) {
            this->graph_size.default_x += graph_size.default_x;
            this->graph_size.point_pen_width++;
            this->graph_size.axes_pen_width++;
        }
    }
    if(maxY > this->graph_size.default_y / 2) {
        while(maxY > this->graph_size.default_y / 2) {
            this->graph_size.default_y += this->graph_size.default_y;
            this->graph_size.point_pen_width++;
            this->graph_size.axes_pen_width++;
        }
    }
    if(minY < -1 * (this->graph_size.default_y / 2)) {
        while(minY < -1 * (this->graph_size.default_y / 2)) {
            this->graph_size.default_y += this->graph_size.default_y;
            this->graph_size.point_pen_width++;
            this->graph_size.axes_pen_width++;
        }
    }
}

void MainWindow::switchParamToDefault()
{
    this->graph_size = {};
}
