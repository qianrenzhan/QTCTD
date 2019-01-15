#ifndef TEMPLATEPREPARE_H
#define TEMPLATEPREPARE_H

#include <QObject>
#include <QDir>
#include <QDebug>

#include<opencv2/opencv.hpp>

//#include <windows.h>
#include <iostream>
//#include <tchar.h>
#include <fstream>
#include <string>

using namespace std;
using namespace cv;

const int CONTOUR_KEY = CV_EVENT_FLAG_CTRLKEY;  //Ctrl键
const int AREA_KEY = CV_EVENT_FLAG_SHIFTKEY; //Shift键

const Scalar RED = Scalar(0, 0, 255);
const Scalar PINK = Scalar(230, 130, 255);
const Scalar BLUE = Scalar(255, 0, 0);
const Scalar LIGHTBLUE = Scalar(255, 255, 160);
const Scalar GREEN = Scalar(0, 255, 0);

class TemplatePrepare:public QObject
{
    Q_OBJECT
public:

    enum { NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
    enum { CONTOURMODE = 0, AREAMODE = 1, NONE = 2};
    static const int radius = 2;
    static const int thickness = -1;

    void reset();
    void setImageAndWinName(const Mat& _image, const string& _winName);
    void showImage(bool update);
    void mouseClick(int event, int x, int y, int flags, void* param);
    QString fileOperation();

signals:
    void template_ready_new();

private:

    const string* winName;
    const Mat* image;
    Mat currentimage;
    Mat originimage;
    int mode;

    Rect rect;                 //外围轮廓
    vector<Rect> vrectobject;  //内部待检区域
    Rect rectdraw;             //更新绘图使用的矩形
    ofstream fout;
};

#endif // TEMPLATEPREPARE_H
