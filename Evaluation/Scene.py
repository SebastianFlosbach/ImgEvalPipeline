import numpy as np
from colmap.scripts.python.read_write_model import read_model, qvec2rotmat
from colmap.scripts.python.read_dense import read_array
from imageio import imread
import matplotlib.pyplot as plt
import h5py
import deepdish as dd
from time import time
import os
import glob

class Scene:
    def __init__(self):
        self.poses = {}

    def addPose(self, image, quat ):
        self.poses[image] = quat

    def readAlembic(self, path):
        alembic = open(path, "r")
        name = ""
        readName = True
        for line in alembic:
            if readName:
                name = line[:-1]
                readName = False
            else:
                qelements = line.split()
                q = []
                q.append(float(qelements[0]))
                q.append(float(qelements[1]))
                q.append(float(qelements[2]))
                q.append(float(qelements[3]))
                readName = True
                self.addPose(name, q)

    def readBin(self, path):
        _, images, _ = read_model(path=path, ext='.bin')
        for image in images.values():
           self.addPose(image.name, image.qvec)

    def readCalibration(self, path):
        for filename in glob.glob(os.path.join(path, '*.h5')):
            config = h5py.File(filename, 'r')
            name = os.path.basename(filename)[12:-3]
            self.addPose(name, [config['q'][0], config['q'][1], config['q'][2], config['q'][3]])
