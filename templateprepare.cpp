#include "templateprepare.h"

void TemplatePrepare::reset()
{
    rectdraw = Rect(0,0,1,1);
    rect = Rect(0,0,1,1);
    vrectobject.clear();
    mode = NONE;
}


void TemplatePrepare::setImageAndWinName(const Mat& _image, const string& _winName)
{
    if (_image.empty() || _winName.empty())
        return;
    image = &_image;
    winName = &_winName;
    currentimage = _image;
    originimage = _image.clone();
    reset();
}


void TemplatePrepare::showImage(bool update)
{
    if (image->empty() || winName->empty())
        return;

    Mat res;
    currentimage.copyTo(res);    //currentimage.    image->

    if(mode == CONTOURMODE)
        cv::rectangle(res, Point(rectdraw.x, rectdraw.y),Point(rectdraw.x + rectdraw.width, rectdraw.y + rectdraw.height), GREEN, 2);
    else
        cv::rectangle(res, Point(rectdraw.x, rectdraw.y),Point(rectdraw.x + rectdraw.width, rectdraw.y + rectdraw.height), RED, 2);

    cv::imshow(*winName, res);
    if(update)
    {
        res.copyTo(currentimage);
        mode = NONE;
    }
}


void TemplatePrepare::mouseClick(int event, int x, int y, int flags, void*)
{
    switch (event)
    {
    case CV_EVENT_LBUTTONDOWN:
    {
        bool isc = (flags & CONTOUR_KEY) != 0;
        bool isa = (flags & AREA_KEY) != 0;

        if (!isc && isa)
        {
            mode = AREAMODE;
            rectdraw = Rect(x, y, 1, 1);
        }
        if (isc && !isa)
        {
            mode = CONTOURMODE;
            rectdraw = Rect(x, y, 1, 1);
        }
    }
        break;


    case CV_EVENT_RBUTTONDOWN:
    {
        //保存结果到：轮廓到txt，区域到另一个txt。
        /*
         * 1.根据contour截取区域，进行颜色分割，得到模板轮廓，并存为txt
         * 2.保存待检区域矩形4点到txt
        */
        Mat process;
        originimage.copyTo(process);

//        namedWindow("img");  //CV_WINDOW_NORMAL
//        imshow("img", process);

        Mat org = process(rect);

//        namedWindow("org");  //CV_WINDOW_NORMAL
//        imshow("org", org);

        Mat bgr, hsv, dst;
        org.convertTo(bgr, CV_32FC3, 1.0 / 255, 0);
        //cvtColor(bgr, hsv, CV_BGR2HSV);
        cvtColor(bgr, hsv, CV_BGR2Lab);

        Vec3d low, high;
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
                    dst.at<Vec3f>(r, c) = ff;// bgr.at<Vec3f>(r, c);
                }
            }
        }

        Mat save,gray_all;
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
            RotatedRect rotaterect1 = minAreaRect(contours_all[i]);
            float width1 = rotaterect1.size.width;
            float height1 = rotaterect1.size.height;
            float rectarea1 = width1 * height1;
            float realarea1 = contourArea(contours_all[i]);
            float index1 = realarea1 / rectarea1;

            if (contours_all[i].size() > maxsize)  // && index1 > 0.9
            {
                maxsize = contours_all[i].size();
                maxindex = i;
            }
        }

        Mat temp = Mat::zeros(gray_all.size(), CV_8UC3);
        drawContours(temp, contours_all, maxindex, CV_RGB(255, 0, 0), 1); //rng.uniform(0,255)  ，CV_FILLED

//        namedWindow("temp");
//        imshow("temp", temp);

        //保存轮廓到文件
        fout.open("1.txt");
        for (int i = 0; i < contours_all[maxindex].size(); i+= 20)
        {
            fout << contours_all[maxindex][i].x << " " << contours_all[maxindex][i].y << endl;
        }
        fout.close();

        fout.open("2.txt");
        fout << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << endl;
        for(int i = 0;i<vrectobject.size();i++)
        {
            fout << vrectobject[i].x - rect.x << " ";
            fout << vrectobject[i].y - rect.y << " ";
            fout << vrectobject[i].width << " ";
            fout << vrectobject[i].height << endl;
        }
        fout.close();

        destroyWindow(*winName);

        emit template_ready_new();
    }
        break;


    case CV_EVENT_LBUTTONUP:
        if(mode == CONTOURMODE)
        {
            rect = rectdraw;
            showImage(true);
        }
        else if(mode == AREAMODE)
        {
            vrectobject.push_back(rectdraw);
            showImage(true);
        }
        break;

    case CV_EVENT_MOUSEMOVE:
        if(mode != NONE)
        {
            rectdraw = Rect(Point(rectdraw.x, rectdraw.y), Point(x, y));
            showImage(false);
        }
        break;
    }
}
