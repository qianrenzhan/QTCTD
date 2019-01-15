import pickle
import os
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

import imtools
import pca
from svmutil import *

# import cv2


def compute_feature(im):
    """ Returns a feature vector for an
        ocr image patch. """

    # resize and remove border
    norm_im = imtools.imresize(im, (30, 30))
    # norm_im = norm_im[3:-3, 3:-3]

    return norm_im.flatten()


def load_data(path):
    """ 加载数据 """

    imlist = imtools.get_imlist(path)

    labels = [int(imfile.split('/')[-1][-5]) for imfile in imlist]

    # create features from the images
    features = []
    for imname in imlist:
        im = np.array(Image.open(imname).convert('L'))
        features.append(compute_feature(im))
    return np.array(features), labels


def judge7(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD77/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD77/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def judge6(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD66/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD66/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def judge5(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD55/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD55/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def judge4(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD44/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD44/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def judge3(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD33/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD33/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def judge2(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD22/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD22/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def judge1(img):
    features = []
    im = np.array(Image.fromarray(img).convert('L'))
    features.append(compute_feature(im))
    labels = [0]

    svm_para_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD11/pca_para.pkl'

    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    features = np.array([np.dot(V, f - m) for f in features])

    svm_model_path = '/home/qian/workspace/QTproject/QTCTD/python/CTD11/svm.model'

    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    res = svm_predict(labels, features, m)

    res_im = np.array(res[0])
    print(res_im[0])
    return res_im[0]


def test():
    print("start...")
    test_path_list = [
        '/home/qian/workspace/vscode/c++python/build/CTD44/test',
        '/home/qian/workspace/vscode/c++python/build/CTD33/test',
        '/home/qian/workspace/vscode/c++python/build/CTD11/test'
    ]

    n = 0
    print("loading image features...")
    test_features, test_labels = load_data(test_path_list[n])

    print("loading SVM para...")
    svm_para_path = '/home/qian/workspace/vscode/c++python/python/pca_para.pkl'
    with open(svm_para_path, 'rb') as f:
        V = pickle.load(f)
        m = pickle.load(f)
    test_features = np.array([np.dot(V, f - m) for f in test_features])

    print("loading model...")
    svm_model_path = '/home/qian/workspace/vscode/c++python/python/svm.model'
    m = svm_load_model(svm_model_path)

    # how does it perform on the test set?
    print("predict...")
    res = svm_predict(test_labels, test_features, m)

    res_im = np.array(res[0])  # .reshape(9, 9)

    print('Result:')
    print(res_im)

    # 绘制所有图
    imlist = imtools.get_imlist(test_path_list[n])
    plt.figure("随机图片" + str(n))
    for i in range(48):
        plt.subplot(4, 12, i + 1)
        plt.imshow(Image.open(imlist[i % 24]))
        plt.axis("off")

    # 绘制识别出的图
    plt.figure("分类结果" + str(n))
    for i in range(24):
        if res_im[i] == 1:
            plt.subplot(4, 12, i + 1)
            plt.imshow(Image.open(imlist[i]))
            plt.axis("off")

    for i in range(24):
        if res_im[i] == 0:
            plt.subplot(4, 12, i + 1 + 24)
            plt.imshow(Image.open(imlist[i]))
            plt.axis("off")

    plt.show()

    return res_im[0]
