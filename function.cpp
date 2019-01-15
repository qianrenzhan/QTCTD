#include "function.h"

void outfilefun()
{
    cout << "out file function" << endl;
}

PyObject *MatToArray(Mat src)
{
    import_array();
    clock_t start, finish;
    start = clock();

    cv::Mat img = src;
    //imread("1.bmp"); // CV_LOAD_IMAGE_COLOR

    PyObject *PythonArray = PyTuple_New(1);

    auto sz = img.size();
    int x = sz.width;
    int y = sz.height;
    int z = img.channels();
    uchar *CArrays = new uchar[x * y * z];
    int iChannels = img.channels();
    int iRows = img.rows;
    int iCols = img.cols * iChannels;

    // if (img.isContinuous())
    // {
    //     iCols *= iRows;
    //     iRows = 1;
    // }

    uchar *p;
    int id = -1;
    for (int i = 0; i < iRows; i++)
    {
        // get the pointer to the ith row
        p = img.ptr<uchar>(i);
        // operates on each pixel
        for (int j = 0; j < iCols; j++)
        {
            CArrays[++id] = p[j]; //连续空间
        }
    }

    npy_intp Dims[3] = {y, x, z};                                               //注意这个维度数据！
    PyObject *PyArray = PyArray_SimpleNewFromData(3, Dims, NPY_UBYTE, CArrays); //这有什么问题
    PyTuple_SetItem(PythonArray, 0, PyArray);

    finish = clock();
    //cout << "\n赋值为" << (double)(finish - start) / CLOCKS_PER_SEC << "秒！" << endl;

    return PythonArray;
}

bool pythonInit(string path)
{
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        cout << "Py_Initialize failed." << endl;
        return false;
    }

    // 将Python工作路径切换到待调用模块所在目录，一定要保证路径名的正确性
    //string path = "/home/qian/workspace/vscode/c++python/python";
    string chdir_cmd = string("sys.path.append(\"") + path + "\")";
    const char *cstr_cmd = chdir_cmd.c_str();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(cstr_cmd);
    return true;
}

PyObject *pythonLoadModuleAndFunction(string moduleName, string functionName)
{
    //加载模块
    PyObject *pModule = PyImport_ImportModule(moduleName.c_str());
    if (!pModule) // 加载模块失败
    {
        cout << "[ERROR] Python get module failed." << endl;
        return pModule;
    }
    cout << "[INFO] Python get module succeed." << endl;

    //加载函数
    PyObject *pFunc = PyObject_GetAttrString(pModule, functionName.c_str()); //print_hello
    if (!pFunc || !PyCallable_Check(pFunc))                                  // 验证是否加载成功
    {
        cout << "[ERROR] Can't find function" << endl;
        return pFunc;
    }
    cout << "[INFO] Get function succeed 123." << endl;

    return pFunc;
}

int pythonCallFunction(PyObject *pFunc, PyObject *PythonArray)
{
    PyObject *pReturn = PyObject_CallObject(pFunc, PythonArray);
    long res = PyLong_AsLong(pReturn);
    return res;
}
