import numpy as np
import quaternion
from colmap.scripts.python.read_write_model import read_model, qvec2rotmat, rotmat2qvec
from colmap.scripts.python.read_dense import read_array
from imageio import imread
import matplotlib.pyplot as plt
import h5py
import deepdish as dd
from time import time
import os
import glob

class Pose:
    rotation = []
    translation = []

    def __init__(self, rot, trans):
        self.rotation = np.quaternion(rot[0], rot[1], rot[2], rot[3])
        self.translation = np.array(trans)

class Scene:
    def __init__(self):
        self.poses = {}

    def addPose(self, image, pose ):
        self.poses[image] = pose

    def readAlembic(self, path):
        alembic = []
        with open(path, "r") as file:
            alembic = file.readlines()

        i = 0
        while i < len(alembic):
            # Name
            name = alembic[i][:-1]
            i = i + 1
            # Rotation
            qElements = alembic[i].split()
            q = []
            q.append(float(qElements[0]))
            q.append(float(qElements[1]))
            q.append(float(qElements[2]))
            q.append(float(qElements[3]))
            i = i + 1
            # Translation
            tElements = alembic[i].split()
            t = []
            t.append(float(tElements[0]))
            t.append(float(tElements[1]))
            t.append(float(tElements[2]))
            i = i + 1

            self.addPose(name, Pose(q, t))

    def readBin(self, path):
        _, images, _ = read_model(path=path, ext='.bin')
        for image in images.values():
           self.addPose(image.name, Pose(image.qvec))

    def readCalibration(self, path):
        for filename in glob.glob(os.path.join(path, '*.h5')):
            config = h5py.File(filename, 'r')
            name = os.path.basename(filename)[12:-3]
            q = [config['q'][0], config['q'][1], config['q'][2], config['q'][3]]
            t = [config['T'][0], config['T'][1], config['T'][2]]
            self.addPose(name, Pose(q, t))

    def readGL3D(self, path):
        cameras = []
        with open(path, "r") as file:
            cameras = file.readlines()
        
        # One line of data per image:
        # IMAGE_ID, FX, FY, PX, PY, SKEW, TRANSLATION VECTOR (3x1), ROTATION MATRIX (3x3), RADIAL DISTORTION (3x1), IMAGE SIZE (2x1).
        for line in cameras:
            data = line.split()
            id = data[0].zfill(8)

            translation = np.empty(3)
            translation[0] = float(data[6])
            translation[1] = float(data[7])
            translation[2] = float(data[8])

            rotation = np.empty((3, 3))
            rotation[0][0] = float(data[9])
            rotation[1][0] = float(data[10])
            rotation[2][0] = float(data[11])
            rotation[0][1] = float(data[12])
            rotation[1][1] = float(data[13])
            rotation[2][1] = float(data[14])
            rotation[0][2] = float(data[15])
            rotation[1][2] = float(data[16])
            rotation[2][2] = float(data[17])

            pose = Pose(rotmat2qvec(rotation), translation)
            self.addPose(id, pose)

