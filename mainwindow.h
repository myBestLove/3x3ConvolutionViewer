#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>


using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QString image_name_;
    Mat origin_image_;
    Mat filter_image_;
    QGraphicsScene *origin_scene_;
    QGraphicsScene *filter_scene_;

    int width_;
    int height_;

    bool isNomal_ = true;
    int kernel_1x1_;
    int kernel_1x2_;
    int kernel_1x3_;
    int kernel_2x1_;
    int kernel_2x2_;
    int kernel_2x3_;
    int kernel_3x1_;
    int kernel_3x2_;
    int kernel_3x3_;
    int cov_type;

    int kernel_[11][9] = {{0,0,0,0,1,0,0,0,0},
                         {0,1,0,1,-4,1,0,1,0},
                         {0,0,0,-1,2,-1,0,0,0},
                         {0,-1,0,0,2,0,0,-1,0},
                         {-2,-1,0,-1,1,1,0,1,2},
                         {1,0,1,-1,0,1,-1,0,1},
                         {-1,-1,-1,0,0,0,1,1,1},
                         {0,-1,0,-1,5,-1,0,-1,0},
                         {1,1,1,1,1,1,1,1,1},
                         {1,0,-1,2,0,-2,1,0,-1},
                         {1,2,1,0,0,0,-1,-2,-1}};
private slots:

    void on_useImage_clicked();

    void on_filedialog_clicked();

    void on_apply_clicked();

    void SetKernel();
    void CalConv(Mat &_image);

    void on_cov_type_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
