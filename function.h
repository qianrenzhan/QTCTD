#include <iostream>
#include <opencv2/opencv.hpp>
#include <numpy/ndarrayobject.h>
#include <python3.6m/Python.h>

using namespace std;
using namespace cv;

void outfilefun();

PyObject *MatToArray(Mat src);

bool pythonInit(string path);
PyObject *pythonLoadModuleAndFunction(string moduleName, string functionName);
int pythonCallFunction(PyObject *pFunc, PyObject *PythonArray);
