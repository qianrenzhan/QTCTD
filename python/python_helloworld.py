import pickle
import os
import matplotlib.pyplot as plt
import numpy as np
from PIL import Image

import imtools
import pca
from svmutil import *

def print_hello():
    print("hello world")


def add_test(a, b):
    return a + b, a * b
