#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<opencv2/opencv.hpp>
#include <fstream>
#include <string>

#include "templateprepare.h"
#include "function.h"

using namespace std;
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
    Mat QImageToMat(QImage image);
    QImage MatToQImage(cv::Mat& mat);
    void updateMiniPic(QString number);
    void readTxt(QString path);
    static void onMouse(int event, int x, int y, int flags, void *param);

private slots:
    void on_btn_open_clicked();

    void on_btn_read_clicked();

    void on_btn_template_prepare_clicked();

    void on_template_ready_new();

    void on_btn_template_confirm_clicked();
    
    void on_btn_detect_clicked();
    
    void on_btn_read_next_clicked();

    void autoPlay();

private:
    Ui::MainWindow *ui;

    Mat m_Picture;
    Mat m_Picture_org;
    Mat m_TemplateArea;
    Mat m_TemplateContour;

    //和目标制备有关
    Mat org;
    string winName;
    float scale;
    bool m_IsTemplateReady;

    int x,y,width,height;
    //原始轮廓
    int origin[2000][3];
    int controldata2[2000][3];
    int index0 = 0;              //原始图片点数量

    //模板
    int object1[1000][3];         //原始模板
    int object2[1000][3];		//去掉高度的模板
    int controldata1[1000][3];   //控制点，每次迭代
    int index1;
    int index2;                 //控制点的数量

    //待检区域
    QList<Rect> vrect;

    //filelist
    QVector<QString> vstr;
    int current_file;

    //Python
//    PyObject *pFunc1;
//    PyObject *pFunc2;
//    PyObject *pFunc3;
    PyObject *pFunc[7];

    //自动播放
    QTimer *timerAutoPlay;

};

#endif // MAINWINDOW_H
