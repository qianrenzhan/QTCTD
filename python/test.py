import os

import numpy as np
from PIL import Image
import pickle

import imtools
import pca
from svmutil import *
import matplotlib.pyplot as plt


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
    # imlist = [
    #     os.path.join(path, f) for f in os.listdir(path) if f.endswith('.bmp')
    # ]

    labels = [int(imfile.split('/')[-1][-5]) for imfile in imlist]

    # create features from the images
    features = []
    for imname in imlist:
        im = np.array(Image.open(imname).convert('L'))
        features.append(compute_feature(im))
    return np.array(features), labels


train_path_list = ['./CTD44', './CTD33', './CTD11']
test_path_list = ['./CTD44/test', './CTD33/test', './CTD11/test']

# 44 33 11
for n in range(1):
    train_path = train_path_list[n]
    test_path = test_path_list[n]

    # TRAINING DATA
    features, labels = load_data(train_path)

    # TESTING DATA
    test_features, test_labels = load_data(test_path)

    V, S, m = pca.pca(features)
    with open('pca_para.pkl', 'wb') as f:
        pickle.dump(V, f)
        pickle.dump(m, f)
    # keep most important dimensions
    V = V[:10]  # 不能超过当前图片数量
    features = np.array([np.dot(V, f - m) for f in features])
    test_features = np.array([np.dot(V, f - m) for f in test_features])

    prob = svm_problem(labels, features)

    param = svm_parameter('-t 0')

    m = svm_train(prob, param)

    # with open('model.pkl', 'wb') as f:
    #     pickle.dump(m, f)

    # how did the training do?
    res = svm_predict(labels, features, m)

    svm_save_model("svm.model", m)

    # how does it perform on the test set?
    res = svm_predict(test_labels, test_features, m)

    res_im = np.array(res[0])  # .reshape(9, 9)

    print('Result:')
    print(res_im)

    # 绘制所有图
    imlist = imtools.get_imlist(test_path)
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
