#include <iostream>
#include <opencv2/opencv.hpp>
#include <numpy/ndarrayobject.h>
#if defined (Q_OS_WIN32)
#include <Python.h>
#endif
#if defined (Q_OS_LINUX)
#include <python3.6m/Python.h>
#endif
using namespace std;
using namespace cv;

void outfilefun();

PyObject *MatToArray(Mat src);

bool pythonInit(string path);
PyObject *pythonLoadModuleAndFunction(string moduleName, string functionName);
int pythonCallFunction(PyObject *pFunc, PyObject *PythonArray);
