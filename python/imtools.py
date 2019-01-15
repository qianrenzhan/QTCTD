import os

import matplotlib.pyplot as plt
import numpy as np
from PIL import Image
from scipy.ndimage import filters


def get_imlist(path):
    """ Returns a list of filenames for
    all jpg images in a directory. """
    return [
        os.path.join(path, f) for f in os.listdir(path) if f.endswith('.bmp')
    ]


def get_imlist2(path):
    """ Returns a list of filenames for
    all jpg images in a directory. """
    return [
        os.path.join(path, f) for f in os.listdir(path) if f.endswith('.ppm')
    ]


def imresize(im, sz):
    """ Resize an image array using PIL. """
    pil_im = Image.fromarray(np.uint8(im))
    return np.array(pil_im.resize(sz))


def histeq(im, nbr_bins=256):
    """ Histogram equalization of a grayscale image. """
    # get image histogram
    imhist, bins = np.histogram(im.flatten(), nbr_bins, normed=True)
    cdf = imhist.cumsum()  # cumulative distribution function
    cdf = 255 * cdf / cdf[-1]  # normalize
    # use linear interpolation of cdf to find new pixel values
    im2 = np.interp(im.flatten(), bins[:-1], cdf)
    return im2.reshape(im.shape), cdf


def compute_average(imlist):
    """ Compute the average of a list of images. """
    # open first image and make into array of type float
    averageim = np.array(Image.open(imlist[0]), 'f')
    for imname in imlist[1:]:
        try:
            averageim += np.array(Image.open(imname))
        except:
            print(imname + '...skipped')
    averageim /= len(imlist)
    # return average as uint8
    return np.array(averageim, 'uint8')


def plot_2D_boundary(plot_range, points, decisionfcn, labels, values=[0]):
    """ Plot_range is (xmin,xmax,ymin,ymax), points is a list
        of class points, decisionfcn is a funtion to evaluate,
        labels is a list of labels that decisionfcn returns for each class,
        values is a list of decision contours to show. """

    clist = ['b', 'r', 'g', 'k', 'm', 'y']  # colors for the classes

    # evaluate on a grid and plot contour of decision function
    x = np.arange(plot_range[0], plot_range[1], .1)
    y = np.arange(plot_range[2], plot_range[3], .1)
    xx, yy = np.meshgrid(x, y)
    xxx, yyy = xx.flatten(), yy.flatten()  # lists of x,y in grid
    zz = np.array(decisionfcn(xxx, yyy))
    zz = zz.reshape(xx.shape)

    # plot contour(s) at values
    plt.contour(xx, yy, zz, values)

    # for each class, plot the points with ’*’ for correct, ’o’ for incorrect
    for i in range(len(points)):
        d = decisionfcn(points[i][:, 0], points[i][:, 1])
        correct_ndx = labels[i] == d
        incorrect_ndx = labels[i] != d
        plt.plot(
            points[i][correct_ndx, 0],
            points[i][correct_ndx, 1],
            '*',
            color=clist[i])
        plt.plot(
            points[i][incorrect_ndx, 0],
            points[i][incorrect_ndx, 1],
            'o',
            color=clist[i])
    plt.axis('equal')
    plt.show()
