#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QTime>
#include <QUuid>
#include <QTimer>

TemplatePrepare gcapp;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    winName = "image";

    scale = 1;
    m_IsTemplateReady = false;

    x = 1;
    y = 1;
    width = 1;
    height = 1;

#if defined (Q_OS_WIN32)
    m_Picture = imread("D:\\temp\\QTCTD\\1.bmp", 1);
    vstr.append("D:\\temp\\QTCTD\\1.bmp");
    vstr.append("D:\\temp\\QTCTD\\3.bmp");
    vstr.append("D:\\temp\\QTCTD\\11.bmp");
    vstr.append("D:\\temp\\QTCTD\\21.bmp");
    vstr.append("D:\\temp\\QTCTD\\31.bmp");
    vstr.append("D:\\temp\\QTCTD\\34.bmp");
    current_file = 0;
#endif
#if defined (Q_OS_LINUX)
    m_Picture = imread("/home/qian/workspace/matlab/ctd1/2.bmp", 1);
    QString path = "/home/qian/workspace/QTproject/QTCTD/pic/";
    for(int i = 0;i<7;i++)
    {
        QString ii = QString::number(i+1);
        for(int j = 0; j<4;j++)
        {
            QString jj = "a.bmp";
            if(j == 0) jj = "a.bmp";
            if(j == 1) jj = "b.bmp";
            if(j == 2) jj = "c.bmp";
            if(j == 3) jj = "d.bmp";
            vstr.append(path+ii+jj);
        }
    }
    current_file = 0;
#endif
    Mat show = m_Picture.clone();
    QImage image = MatToQImage(show);
    ui->label_picture->setPixmap(QPixmap::fromImage(image.scaled(ui->label_picture->size())));

    //暂时注释掉
    connect(&gcapp,SIGNAL(template_ready_new()),this,SLOT(on_template_ready_new()));

    //on_btn_template_confirm_clicked();


    QString pythonpath = "/home/qian/workspace/QTproject/QTCTD/python";
    if (!pythonInit(pythonpath.toStdString().c_str()))
        qDebug() << "python init failed" << endl;
    pFunc[0] = pythonLoadModuleAndFunction("pythonsvmtest", "judge1");
    pFunc[1] = pythonLoadModuleAndFunction("pythonsvmtest", "judge2");
    pFunc[2] = pythonLoadModuleAndFunction("pythonsvmtest", "judge3");
    pFunc[3] = pythonLoadModuleAndFunction("pythonsvmtest", "judge4");
    pFunc[4] = pythonLoadModuleAndFunction("pythonsvmtest", "judge5");
    pFunc[5] = pythonLoadModuleAndFunction("pythonsvmtest", "judge6");
    pFunc[6] = pythonLoadModuleAndFunction("pythonsvmtest", "judge7");

    qDebug() << "python function loaded" << endl;

    timerAutoPlay = new QTimer();
    timerAutoPlay->setInterval(2000);
    connect(timerAutoPlay,SIGNAL(timeout()),this,SLOT(autoPlay()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::autoPlay()
{
    on_btn_read_next_clicked();
    on_btn_detect_clicked();
}

void MainWindow::updateMiniPic(QString number)
{
    QString currenttemplate = number;
    //如果时-1，则从文件读取最新的，否则，显示对应的图
    if(number.compare("-1") == 0)
    {
        ifstream in;
        in.open("./template/1.txt"); //打开文件
        if (!in.eof())
        {
            int current;
            in >> current;
            currenttemplate = QString::number(current);
        }
        in.close();//关闭文件
    }

    QString path = "./template/" + currenttemplate;
    readTxt(path);

    //在右上绘制截取后的图
    QString picpath = path+"/1.bmp";
    Rect rect(x/scale,y/scale,width/scale,height/scale);
    Mat templatepic = imread(picpath.toStdString().c_str());
    Mat roi = templatepic(rect);

    QImage image = MatToQImage(roi);
    ui->label_area->setPixmap(QPixmap::fromImage(image.scaled(ui->label_area->size(),Qt::KeepAspectRatio)));

    //在roi上绘制轮廓，画到右下
    cv::Size size(roi.size().width*scale,roi.size().height*scale);
    Mat roicontour = Mat::zeros(size, CV_8UC3);
    for (int controlnum = 0; controlnum < index2; controlnum++)
    {
        int x = object2[controlnum][0];
        int y = object2[controlnum][1];
        circle(roicontour, Point(x, y), 2, Scalar(0, 0, 255), -1);
    }

    for(int rectnum = 0; rectnum < vrect.size();rectnum++)
    {
        rectangle(roicontour,vrect[rectnum],Scalar(0, 255, 0), 2);
    }

    image = MatToQImage(roicontour);
    ui->label_contour->setPixmap(QPixmap::fromImage(image.scaled(ui->label_contour->size(),Qt::KeepAspectRatio)));

}

void MainWindow::readTxt(QString path)
{
    QString path1 = path + "/1.txt";
    ifstream in;
    in.open(path1.toStdString().c_str()); //打开文件
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
    QString path2 = path + "/2.txt";
    in.open(path2.toStdString().c_str());//打开文件

    in >> x >> y >> width>> height;
    vrect.clear();
    while (!in.eof())
    {
        int x1,y1,width1,height1;
        in >> x1 >> y1 >> width1>> height1;
        Rect rect(x1,y1,width1,height1);
        vrect.append(rect);
    }
    vrect.pop_back();
    in.close();//关闭文件
}

void MainWindow::on_template_ready_new()
{
    updateMiniPic("-1");
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
//    //打开摄像头
//    QMessageBox msgBox;
//    msgBox.setText("摄像头不存在!");
//    msgBox.exec();

    //开启自动播放
    if(timerAutoPlay->isActive())
        timerAutoPlay->stop();
    else
        timerAutoPlay->start();
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
    //org = cv::Mat(dsize, CV_32S);
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
    on_template_ready_new();
    m_IsTemplateReady = true;
}

void MainWindow::on_btn_detect_clicked()
{
    //QString strshow;
    if(m_IsTemplateReady == false)
    {
        QMessageBox msgBox;
        msgBox.setText("没有模板!");
        msgBox.exec();
        return;
    }

    //进行图像处理
    Size dsize = Size(m_Picture.cols*scale, m_Picture.rows*scale);

    cv::resize(m_Picture, m_Picture_org, dsize);

    Mat bgr, hsv, dst;
    m_Picture_org.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
    cvtColor(bgr, hsv, CV_BGR2Lab);

    Vec3d low, high;
    //1
    low[0] = -8;
    low[1] = 35;
    low[2] = 28;
    high[0] = 91;
    high[1] = 65;
    high[2] = 58;

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
                dst.at<Vec3f>(r, c) = ff;
            }
        }
    }

    Mat save, gray_all;
    dst.convertTo(save, CV_32FC3, 255, 0);

    save.convertTo(gray_all, CV_8UC3, 255.0);

//    namedWindow("img");  //CV_WINDOW_NORMAL
//    imshow("img", gray_all);

    cvtColor(gray_all, gray_all, CV_BGR2GRAY);

    GaussianBlur(gray_all, gray_all, Size(3, 3), 0);
    threshold(gray_all, gray_all, 100, 255, THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(gray_all, gray_all, element);
    erode(gray_all, gray_all, element);

//    namedWindow("img");  //CV_WINDOW_NORMAL
//    imshow("img", gray_all);
//    waitKey(1);

    vector<vector<cv::Point> > contours_all;
    vector<cv::Vec4i> hierarchy_all;

    findContours(gray_all, contours_all, hierarchy_all, CV_RETR_TREE, cv::CHAIN_APPROX_NONE, cv::Point(0, 0));

    //找最大的轮廓
    int maxindex = 0;
    int maxsize = 0;
    for (int i = 0; i < contours_all.size(); i++)
    {
        if (contours_all[i].size() > maxsize)  // && index1 > 0.9
        {
            maxsize = contours_all[i].size();
            maxindex = i;
        }
    }

//    Mat temp = Mat::zeros(gray_all.size(), CV_8UC3);
//    drawContours(temp, contours_all, maxindex, CV_RGB(255, 0, 0), 1);

//    namedWindow("img");  //CV_WINDOW_NORMAL
//    imshow("img", temp);
//    waitKey(1);

    index0 = 0;    //原始图片点数量
    for (int i = 0; i < contours_all[maxindex].size(); i+=2)
    {
        origin[index0][0] = contours_all[maxindex][i].x;
        origin[index0][1] = contours_all[maxindex][i].y;
        origin[index0++][2] = 0;
    }

    // 读取文件，判断有多少个模板
//    int templatecount;
//    ifstream in;
//    in.open("./template/1.txt"); //打开文件
//    if (!in.eof())
//        in >> templatecount;
//    in.close();//关闭文件

    QFile *tempFile = new QFile();
    tempFile->setFileName("./template/1.txt");
    if(!tempFile->open(QIODevice::ReadOnly|QIODevice::Text))
        qDebug() << "file open failed" << endl;
    QTextStream ts(tempFile);
    QString str_get;
    while(!ts.atEnd())
        str_get = ts.readAll();
    tempFile->close();
    int templatecount = str_get.toInt();


    //需要判断那个模板匹配的最好
    float T[2] = { 0,0 };
    float E = 0;
    float last_E = 0;
    int iteration = 15;
    float T_Intermediate[50][2];
    float e_Intermediate[50];

    int besttemplate = 0;
    int bestiter = 0;
    for(int count = 1; count <= templatecount; count++)
    {
        QString path = "./template/" + QString::number(count);

        readTxt(path);

        //数据准备完毕，下面进行匹配及结果展示
        //2.4 初始化
        T[0] = 0; T[1] = 0;
        E = 0;
        last_E = 0;
        for (int i = 0; i < 50; i++)
        {
            e_Intermediate[i] = 0;
            for (int j = 0; j < 2; j++)
                T_Intermediate[i][j] = 0;
        }

        //2.5 迭代,需要判断那个模板匹配的最好
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
//            if (delta < 0.001)
//                break;
            if(e_Intermediate[iter] < 1 && delta < 0.0001)
                break;
            last_E = E;
            qDebug() << "iter:" << iter << endl;
            qDebug() << e_Intermediate[iter] << endl;
        }
        if(iter < iteration)
        {
            //记录当前模板
            besttemplate = count;
            bestiter = iter;
            qDebug() << "besttemplate:" << besttemplate << endl;
            qDebug() << "bestiter:" << bestiter << endl;
            break;
        }
    }

    //更新右边两个小图
    if(besttemplate == 0)
        return;
    updateMiniPic(QString::number(besttemplate));

    //2.6 计算最终的R和T
//    cout << "计算最终的R和T:" << endl;
//    strshow = "计算最终的R和T:\n";
//    ui->textEdit->insertPlainText(strshow);
//    ui->textEdit->moveCursor(QTextCursor::End);

    float temp_T[2] = {0,0};
    for (int i = 0; i < bestiter; i++)
    {
        temp_T[0] += T_Intermediate[i][0];
        temp_T[1] += T_Intermediate[i][1];
    }
    qDebug() << "x:" << temp_T[0] << ",y:" << temp_T[1] << endl;

//    temp_T[0] -=2;
//    temp_T[1] -=2;
//    strshow = QString::number(temp_T[0]) + " " + QString::number(temp_T[1]) + "\n";
//    ui->textEdit->insertPlainText(strshow);
//    ui->textEdit->moveCursor(QTextCursor::End);

    //2.7 得到匹配关系，把模板轮廓和检测区域根据R和T映射到原始图片，进行显示，主要是展示效果
    for (int controlnum = 0; controlnum < index2 - 1; controlnum++)
    {
        int x1 = object2[controlnum][0] + temp_T[0];
        int y1 = object2[controlnum][1] + temp_T[1];
        int x2 = object2[controlnum+1][0] + temp_T[0];
        int y2 = object2[controlnum+1][1] + temp_T[1];
        circle(m_Picture_org, Point(x1, y1), 4, Scalar(255, 255, 255), -1);
        if(x2-x1<5)
            line(m_Picture_org,Point(x1,y1),Point(x2,y2),Scalar(0,0,0),2);
    }

    for(int rectnum = 0; rectnum < vrect.size(); rectnum++)
    {
        int x = vrect[rectnum].x + temp_T[0];
        int y = vrect[rectnum].y + temp_T[1];
        Rect rect(x, y, vrect[rectnum].width, vrect[rectnum].height);

        Mat temparea = m_Picture(rect);

        int res1 = 0;
        res1 = pythonCallFunction(pFunc[besttemplate-1], MatToArray(temparea));
//        if(besttemplate == 1)
//            res1 = pythonCallFunction(pFunc1, MatToArray(temparea));
//        else if(besttemplate == 2)
//            res1 = pythonCallFunction(pFunc2, MatToArray(temparea));
//        else if(besttemplate == 3)
//            res1 = pythonCallFunction(pFunc3, MatToArray(temparea));
        qDebug() << "res1:" << res1 << endl;

        if(res1 == 1)
            rectangle(m_Picture_org,rect,Scalar(0, 255, 0), 2);   //GREEN
        else
            rectangle(m_Picture_org,rect,Scalar(0, 0, 255), 2);   //RED

        //save small area
//        Mat write = m_Picture(rect);
//        QString filename = QTime::currentTime().toString()+QUuid::createUuid().toString() + ".bmp";
//        imwrite(filename.toStdString(),write);
    }

    qDebug() << m_Picture_org.cols << " " << m_Picture_org.rows << endl;
    QImage image = MatToQImage(m_Picture_org);
    ui->label_picture->setPixmap(QPixmap::fromImage(image.scaled(ui->label_picture->size())));

}

void MainWindow::on_btn_read_next_clicked()
{
    current_file = (++current_file)%(vstr.size());
    QString fileName = vstr.at(current_file);

    m_Picture = imread(fileName.toStdString(), 1);
    Mat show = m_Picture.clone();
    QImage image = MatToQImage(show);
    ui->label_picture->setPixmap(QPixmap::fromImage(image.scaled(ui->label_picture->size())));
}
