#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    width_ = ui->originImage->width();
    height_ = ui->originImage->height();
    origin_scene_ = new QGraphicsScene;
    filter_scene_ = new QGraphicsScene;

    connect(ui->cov_type, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(SetKernel()));
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_filedialog_clicked()
{
    image_name_ = QFileDialog::getOpenFileName(
       this,
       "Open Document",
       QDir::currentPath(),
       "Document files (*.png *.bmp *.jpg *.tif *.GIF);;All files(*.*)");
    if (!image_name_.isNull()) { //用户选择了文件
       // 处理文件
       QMessageBox::information(this, "Document", "Has image", QMessageBox::Ok | QMessageBox::Cancel);
    } else // 用户取消选择
       QMessageBox::information(this, "Document", "No image", QMessageBox::Ok | QMessageBox::Cancel);

    ui->lineEdit->setText(image_name_);

    origin_image_ = cv::imread(image_name_.toStdString());
    if(origin_image_.empty()){
        QMessageBox::information(this, "Document", "can not load image", QMessageBox::Ok | QMessageBox::Cancel);
        return ;
    }

    cv::resize(origin_image_,origin_image_,cv::Size(width_,height_),0,0,CV_INTER_LINEAR);

    QImage *temp_image;
    cv::Mat bgr_image;
    if(origin_image_.channels()==4)
        temp_image = new QImage((const unsigned char*)(origin_image_.data), origin_image_.cols,origin_image_.rows, origin_image_.cols*origin_image_.channels(), QImage::Format_ARGB32);
    else if(origin_image_.channels() == 3)
    {

        cvtColor(origin_image_, bgr_image, CV_BGR2RGB);
        temp_image = new QImage((const unsigned char*)(bgr_image.data), bgr_image.cols,bgr_image.rows, bgr_image.cols*bgr_image.channels(), QImage::Format_RGB888);
    }
    else
        temp_image = new QImage((const unsigned char*)(origin_image_.data), origin_image_.cols,origin_image_.rows, origin_image_.cols*origin_image_.channels(), QImage::Format_Indexed8);

    if(temp_image->isNull())
        QMessageBox::information(this, "Document", "can not convert to QImage", QMessageBox::Ok | QMessageBox::Cancel);
    origin_scene_->clear();
    origin_scene_->addPixmap(QPixmap::fromImage(*temp_image));
    ui->originImage->setScene(origin_scene_);
//    ui->originImage->resize(origin_scene_->width(),origin_scene_->height());
    ui->originImage->show();


}


void MainWindow::on_useImage_clicked()
{
//    origin_scene_= ui->filterImage->scene();
    if(filter_image_.empty())
    {
        QMessageBox::information(this, "Document", "no filter image", QMessageBox::Ok | QMessageBox::Cancel);
        return;
    }
    filter_image_.copyTo(origin_image_);

    QImage *temp_image;
    cv::Mat bgr_image;
    if(origin_image_.channels()==4)
        temp_image = new QImage((const unsigned char*)(origin_image_.data), origin_image_.cols,origin_image_.rows, origin_image_.cols*origin_image_.channels(), QImage::Format_ARGB32);
    else if(origin_image_.channels() == 3)
    {

        cvtColor(origin_image_, bgr_image, CV_BGR2RGB);
        temp_image = new QImage((const unsigned char*)(bgr_image.data), bgr_image.cols,bgr_image.rows, bgr_image.cols*bgr_image.channels(), QImage::Format_RGB888);
    }
    else
        temp_image = new QImage((const unsigned char*)(origin_image_.data), origin_image_.cols,origin_image_.rows, origin_image_.cols*origin_image_.channels(), QImage::Format_Indexed8);

    if(temp_image->isNull())
        QMessageBox::information(this, "Document", "can not convert to QImage", QMessageBox::Ok | QMessageBox::Cancel);


    origin_scene_->clear();
    origin_scene_->addPixmap(QPixmap::fromImage(*temp_image));
    ui->originImage->setScene(origin_scene_);
    ui->originImage->show();



}

void MainWindow::on_apply_clicked()
{
    if(origin_image_.empty())
    {
        QMessageBox::information(this, "Document", "load image", QMessageBox::Ok | QMessageBox::Cancel);
        ui->lineEdit->setFocus();
        return;
    }


    CalConv(filter_image_);

    if(ui->nomal->isChecked())
    {
        isNomal_ = true;
        normalize(filter_image_,filter_image_,0,1,NORM_MINMAX,-1,Mat());
    }
    else
        isNomal_ = false;


    QImage *temp_image;
    cv::Mat bgr_image;
    if(filter_image_.channels()==4)
        temp_image = new QImage((const unsigned char*)(filter_image_.data), filter_image_.cols,filter_image_.rows, filter_image_.cols*filter_image_.channels(), QImage::Format_ARGB32);
    else if(filter_image_.channels() == 3)
    {
        cvtColor(filter_image_, bgr_image, CV_BGR2RGB);
        temp_image = new QImage((const unsigned char*)(bgr_image.data), bgr_image.cols,bgr_image.rows, bgr_image.cols*bgr_image.channels(), QImage::Format_RGB888);
    }
    else
        temp_image = new QImage((const unsigned char*)(filter_image_.data), filter_image_.cols,filter_image_.rows, filter_image_.cols*filter_image_.channels(), QImage::Format_Indexed8);


    if(temp_image->isNull())
        QMessageBox::information(this, "Document", "can not convert to QImage", QMessageBox::Ok | QMessageBox::Cancel);

    filter_scene_->clear();
    filter_scene_->addPixmap(QPixmap::fromImage(*temp_image));
    ui->filterImage->setScene(filter_scene_);
    ui->filterImage->show();

}

void MainWindow::SetKernel()
{
    int _flag = ui->cov_type->currentIndex();

    ui->spinBox_1->setValue(kernel_[_flag][0]);  ui->spinBox_4->setValue(kernel_[_flag][1]);  ui->spinBox_7->setValue(kernel_[_flag][2]);
    ui->spinBox_2->setValue(kernel_[_flag][3]);  ui->spinBox_5->setValue(kernel_[_flag][4]);  ui->spinBox_8->setValue(kernel_[_flag][5]);
    ui->spinBox_3->setValue(kernel_[_flag][6]);  ui->spinBox_6->setValue(kernel_[_flag][7]);  ui->spinBox_9->setValue(kernel_[_flag][8]);


}


#include<iostream>
using namespace std;
void MainWindow::CalConv(cv::Mat &_image)
{
    Mat gray;
    if(origin_image_.channels() != 1)
        cvtColor(origin_image_,gray,CV_BGR2GRAY);
    else
        origin_image_.copyTo(gray);
    _image.create(gray.size(),gray.type());

    kernel_1x1_ = ui->spinBox_1->value();    kernel_1x2_ = ui->spinBox_4->value();    kernel_1x3_ = ui->spinBox_7->value();

    kernel_2x1_ = ui->spinBox_2->value();    kernel_2x2_ = ui->spinBox_5->value();    kernel_2x3_ = ui->spinBox_8->value();

    kernel_3x1_ = ui->spinBox_3->value();    kernel_3x2_ = ui->spinBox_6->value();    kernel_3x3_ = ui->spinBox_9->value();

//    cout<<kernel_1x1_<<" "<<kernel_1x2_<<" "<<kernel_1x3_<<endl;
//    cout<<kernel_2x1_<<" "<<kernel_2x2_<<" "<<kernel_2x3_<<endl;
//    cout<<kernel_3x1_<<" "<<kernel_3x2_<<" "<<kernel_3x3_<<endl;
    int channels = origin_image_.channels();
    for(size_t i = 1;i<gray.rows-1;i++)
    {
        uchar *pre_pts = gray.ptr<uchar>(i-1);
        uchar *cur_pts = gray.ptr<uchar>(i);
        uchar *nxt_pts = gray.ptr<uchar>(i+1);
        uchar *out_pts = _image.ptr<uchar>(i);
        for(size_t j = 1;j<(gray.cols-1);j++)
        {

//            _image.at<cv::Vec3b>(i,j)[0] = kernel_1x1_*origin_image_.at<cv::Vec3b>(i-1,j-1)[0] + kernel_1x2_*origin_image_.at<cv::Vec3b>(i-1,j)[0] + kernel_1x3_*origin_image_.at<cv::Vec3b>(i-1,j+1)[0] +
//                                    kernel_2x1_*origin_image_.at<cv::Vec3b>(i,j-1)[0] + kernel_2x2_*origin_image_.at<cv::Vec3b>(i,j)[0] + kernel_2x1_*origin_image_.at<cv::Vec3b>(i,j+1)[0] +
//                                    kernel_3x1_*origin_image_.at<cv::Vec3b>(i+1,j-1)[0] + kernel_3x2_*origin_image_.at<cv::Vec3b>(i+1,j)[0] + kernel_3x1_*origin_image_.at<cv::Vec3b>(i+1,j+1)[0];
            *out_pts++ = kernel_1x1_*pre_pts[j+0] + kernel_1x2_*pre_pts[j+1] + kernel_1x3_*pre_pts[j+2] +
                                    kernel_2x1_*cur_pts[j+0] + kernel_2x2_*cur_pts[j+1] + kernel_2x1_*cur_pts[j+2] +
                                    kernel_3x1_*nxt_pts[j+0] + kernel_3x2_*nxt_pts[j+1] + kernel_3x1_*nxt_pts[j+2];

        }

    }


    _image.row(0).setTo(Scalar(0));
    _image.row(_image.rows - 1).setTo(Scalar(0));
    _image.col(0).setTo(Scalar(0));
    _image.col(_image.cols - 1).setTo(Scalar(0));
}
