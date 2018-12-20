#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "templateprepare.h"

TemplatePrepare gcapp;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    winName = "image";

    scale = 0.5;
    m_IsTemplateReady = false;

    m_Picture = imread("D:/temp/31.jpg", 1);
    Mat show = m_Picture.clone();
    QImage image = MatToQImage(show);
    ui->label_picture->setPixmap(QPixmap::fromImage(image.scaled(ui->label_picture->size())));

    connect(&gcapp,SIGNAL(template_ready(int, int, int, int)),this,SLOT(on_template_ready(int, int, int, int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_template_ready(int x,int y,int width,int height)
{
    //读取1.txt和2.txt
    Mat test = m_Picture;

    ifstream in;
    in.open("1.txt"); //打开文件
    index1 = 0;
    while (!in.eof())
    {
        in >> object1[index1][0] >> object1[index1][1];
        object1[index1++][2] = 0;
    }
    in.close();//关闭文件

    index2 = 0;       //控制点的数量
    int max = 0;
    for (int i = 0; i < index1 - 1; i++)
    {
        if (object1[i][1] > max)
            max = object1[i][1];
    }
    for (int i = 0; i < index1 - 1; i++)
    {
        if (object1[i][1] < max*0.95)
        {
            controldata1[index2][0] = object1[i][0];
            controldata1[index2][1] = object1[i][1];
            controldata1[index2][2] = object1[i][2];

            object2[index2][0] = object1[i][0];
            object2[index2][1] = object1[i][1];
            object2[index2++][2] = object1[i][2];
        }
    }

    //读取待检区域txt
   in.open("2.txt");//打开文件
   vrect.clear();
    while (!in.eof())
    {
        int x1,y1,width1,height1;
        in >> x1 >> y1 >> width1>> height1;
        Rect rect(x1,y1,width1,height1);
        vrect.append(rect);
    }
    in.close();//关闭文件

    //在右上绘制截取后的图
    Rect rect(x/scale,y/scale,width/scale,height/scale);
    Mat roi = m_Picture(rect);
    QImage image = MatToQImage(roi);
    ui->label_area->setPixmap(QPixmap::fromImage(image.scaled(ui->label_area->size(),Qt::KeepAspectRatio)));

    //在roi上绘制轮廓，画到右下
    cv::Size size(roi.size().width/2,roi.size().height/2);
    Mat roicontour = Mat::zeros(size, CV_8UC3);
    for (int controlnum = 0; controlnum < index2; controlnum++)
    {
        int x = object2[controlnum][0];
        int y = object2[controlnum][1];
        circle(roicontour, Point(x, y), 4, Scalar(0, 0, 255), -1);
    }

    for(int rectnum = 0; rectnum < vrect.size();rectnum++)
    {
        rectangle(roicontour,vrect[rectnum],Scalar(0, 255, 0), -1);
    }

    image = MatToQImage(roicontour);
    ui->label_contour->setPixmap(QPixmap::fromImage(image.scaled(ui->label_contour->size(),Qt::KeepAspectRatio)));
}

QImage MainWindow::MatToQImage(cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat MainWindow::QImageToMat(QImage image)
{
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), (size_t)image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        break;
    }
    return mat;
}

void MainWindow::on_btn_open_clicked()
{
    //打开摄像头
    QMessageBox msgBox;
    msgBox.setText("摄像头不存在!");
    msgBox.exec();
}

void MainWindow::on_btn_read_clicked()
{
    QString fileName;

    fileName = QFileDialog::getOpenFileName(this,
          tr("Open Image"), NULL, tr("Image Files (*.png *.jpg *.bmp)"));

    m_Picture = imread(fileName.toStdString(), 1);
    Mat show = m_Picture.clone();
    QImage image = MatToQImage(show);
    ui->label_picture->setPixmap(QPixmap::fromImage(image.scaled(ui->label_picture->size())));
}

void MainWindow::on_btn_template_prepare_clicked()
{
    cv::Size dsize = cv::Size(m_Picture.cols*scale, m_Picture.rows*scale);
    org = cv::Mat(dsize, CV_32S);
    cv::resize(m_Picture, org, dsize);

    cvNamedWindow(winName.c_str(), CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback(winName.c_str(), this->onMouse, (void*)&org);    //选框
    gcapp.setImageAndWinName(org, winName);
    gcapp.showImage(false);
}

void MainWindow::onMouse(int event, int x, int y, int flags, void *param)
{
    gcapp.mouseClick(event, x, y, flags, param);
}

void MainWindow::on_btn_template_confirm_clicked()
{
    m_IsTemplateReady = true;
}

void MainWindow::on_btn_detect_clicked()
{

    if(m_IsTemplateReady == false)
    {
        QMessageBox msgBox;
        msgBox.setText("没有模板!");
        msgBox.exec();
        return;
    }

    //进行图像处理
    Size dsize = Size(m_Picture.cols*scale, m_Picture.rows*scale);

    Mat org = Mat(dsize, CV_32S);
    cv::resize(m_Picture, org, dsize);

    Mat bgr, hsv, dst;
    org.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
    cvtColor(bgr, hsv, CV_BGR2Lab);

    Vec3d low, high;
    low[0] = -61;
    low[1] = -8;
    low[2] = -20;
    high[0] = 138;
    high[1] = 51;
    high[2] = 39;

    Mat mask;
    inRange(hsv, Scalar(low[0], low[1], low[2]), Scalar(high[0], high[1], high[2]), mask);
    dst = Mat::zeros(hsv.size(), CV_32FC3);
    Vec3f ff = Vec3f(255, 255, 255);
    Vec3f f0 = Vec3f(0, 0, 0);
    for (int r = 0; r < bgr.rows; r++)
    {
        for (int c = 0; c < bgr.cols; c++)
        {
            if (mask.at<uchar>(r, c) == 255)
            {
                dst.at<Vec3f>(r, c) = f0;
            }
            else
            {
                dst.at<Vec3f>(r, c) = ff;// bgr.at<Vec3f>(r, c);
            }
        }
    }

    Mat save, gray_all;
    dst.convertTo(save, CV_32FC3, 255, 0);

    save.convertTo(gray_all, CV_8UC3, 255.0);

    //        namedWindow("img");  //CV_WINDOW_NORMAL
    //        imshow("img", gray_all);

    cvtColor(gray_all, gray_all, CV_BGR2GRAY);

    GaussianBlur(gray_all, gray_all, Size(3, 3), 0);
    threshold(gray_all, gray_all, 100, 255, THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(gray_all, gray_all, element);
    erode(gray_all, gray_all, element);

    vector<vector<cv::Point> > contours_all;
    vector<cv::Vec4i> hierarchy_all;

    findContours(gray_all, contours_all, hierarchy_all, CV_RETR_TREE, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    //找最大的轮廓
    int maxindex = 0;
    int maxsize = 0;
    for (int i = 0; i < contours_all.size(); i++)
    {
        //RotatedRect rotaterect1 = minAreaRect(contours_all[i]);
        //float width1 = rotaterect1.size.width;
        //float height1 = rotaterect1.size.height;
        //float rectarea1 = width1 * height1;
        //float realarea1 = contourArea(contours_all[i]);
        //float index1 = realarea1 / rectarea1;

        if (contours_all[i].size() > maxsize)  // && index1 > 0.9
        {
            maxsize = contours_all[i].size();
            maxindex = i;
        }
    }

    index0 = 0;    //原始图片点数量
    for (int i = 0; i < contours_all[maxindex].size(); i+=2)
    {
        origin[index0][0] = contours_all[maxindex][i].x;
        origin[index0][1] = contours_all[maxindex][i].y;
        origin[index0++][2] = 0;
    }

    for (int i = 0; i < index2; i++)   //控制点1初始化
    {
        controldata1[i][0] = object2[i][0];
        controldata1[i][1] = object2[i][1];
        controldata1[i][2] = object2[i][2];
    }

    //数据准备完毕，下面进行匹配及结果展示

    //2.4 初始化
    float T[2] = { 0,0 };
    float E = 0;
    float last_E = 0;
    int iteration = 15;
    float T_Intermediate[15][2];
    for (int i = 0; i < 15; i++)
        for (int j = 0; j < 2; j++)
            T_Intermediate[i][j] = 0;
    //float delta_Intermediate[15];
    float e_Intermediate[15];
    //int indexcontrol[100];   //index2

    //2.5 迭代
    int iter = 0;
    for (iter = 0; iter < iteration; iter++)
    {
        //2.5.1 寻找控制点的对应点
        for (int controlnum = 0; controlnum < index2; controlnum++)
        {
            int minidis = 100000;
            int minindex = 0;
            for (int originnum = 0; originnum < index0; originnum++)
            {
                //计算距离
                int dis = sqrt((controldata1[controlnum][0] - origin[originnum][0])*(controldata1[controlnum][0] - origin[originnum][0])
                    + (controldata1[controlnum][1] - origin[originnum][1])*(controldata1[controlnum][1] - origin[originnum][1]));
                if (dis < minidis)
                {
                    minidis = dis;
                    minindex = originnum;
                }
            }
            controldata2[controlnum][0] = origin[minindex][0];
            controldata2[controlnum][1] = origin[minindex][1];
            controldata2[controlnum][2] = origin[minindex][2];
        }
        //2.5.2 求解旋转和平移矩阵
        //controldata1 controldata2 的中心
        float x1sum = 0, y1sum = 0;
        float x2sum = 0, y2sum = 0;
        for (int i = 0; i < index2; i++)  //index2 控制点的数量
        {
            x1sum += controldata1[i][0];
            y1sum += controldata1[i][1];
            x2sum += controldata2[i][0];
            y2sum += controldata2[i][1];
        }
        x1sum /= index2;
        y1sum /= index2;
        x2sum /= index2;
        y2sum /= index2;

        T[0] = x2sum - x1sum;
        T[1] = y2sum - y1sum;
        T_Intermediate[iter][0] = T[0];
        T_Intermediate[iter][1] = T[1];

        //2.5.3 利用求解得到的RT计算变换之后的点
        for (int controlnum = 0; controlnum < index2; controlnum++)
        {
            controldata1[controlnum][0] += T[0];
            controldata1[controlnum][1] += T[1];
        }
        //误差
        float dissum = 0;
        for (int controlnum = 0; controlnum < index2; controlnum++)
        {
            dissum += ((controldata1[controlnum][0] - controldata2[controlnum][0])*(controldata1[controlnum][0] - controldata2[controlnum][0])
                + (controldata1[controlnum][1] - controldata2[controlnum][1])*(controldata1[controlnum][1] - controldata2[controlnum][1]));
        }
        E = sqrt(dissum);
        e_Intermediate[iter] = E / index2;
        float delta = abs(E - last_E) / index2;
        if (delta < 0.0001)
            break;
        last_E = E;
        //cout << "迭代了" << iter << "代" << endl;
        cout << e_Intermediate[iter] << endl;
    }

    //2.6 计算最终的R和T
    cout << "计算最终的R和T" << endl;
    float temp_T[2] = {0,0};
    for (int i = 0; i < iter; i++)
    {
        temp_T[0] += T_Intermediate[i][0];
        temp_T[1] += T_Intermediate[i][1];
    }

    cout << temp_T[0] << " " << temp_T[1] << endl;

    //2.7 得到匹配关系，把模板轮廓和检测区域根据R和T映射到原始图片，进行显示，主要是展示效果
    for (int controlnum = 0; controlnum < index2; controlnum++)
    {
        int x = object2[controlnum][0] + temp_T[0];
        int y = object2[controlnum][1] + temp_T[1];
        circle(org, Point(x, y), 2, Scalar(0, 0, 255), -1);
    }

    QImage image = MatToQImage(org);
    ui->label_picture->setPixmap(QPixmap::fromImage(image.scaled(ui->label_picture->size())));

}
