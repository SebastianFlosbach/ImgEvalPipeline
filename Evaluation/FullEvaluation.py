import subprocess as sp
from threading import Timer
import os
import PoseEvaluation as pe
import matplotlib.pyplot as plt
import numpy as np
import glob
import win32gui
import win32con
from pathlib import Path
from distutils.dir_util import copy_tree

# Settings
dataset = 'herzjesu_dense'
loadAnglesFromFile = False
# Settings

def clearDirectory(path):
    for fileName in os.listdir(path):
        filePath = os.path.join(path, fileName)
        os.remove(filePath)

def saveAngles(name, angles):
    Path('data/' + dataset + '/full').mkdir(parents=True, exist_ok=True)
    with open('data/' + dataset + '/full' + '/angles_' + name + '.txt', 'w') as file:
        for angle in angles:
            file.write(str(angle) + '\n')

def loadAngles(name):
    angles = []
    with open('data/' + dataset + '/full' + '/angles_' + name + '.txt', 'r') as file:
        for angle in file:
            angles.append(float(angle))
    return angles

def plotAngles(angles, threshold, cumulative=False):
    bins = np.arange(0, threshold + 1, 1)
    plt.hist(np.clip(angles, bins[0], bins[-1]), bins=bins, density=False, cumulative=cumulative)
    plt.show()

def killKeypointWindow():
    handle = win32gui.FindWindow(None, 'Keypoints')
    if not (handle == 0):
        win32gui.PostMessage(handle,win32con.WM_CLOSE,0,0)

workingDirectory = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/'

runCommonTasks = workingDirectory + 'runCommonTasks.bat'
runDFMTasks = workingDirectory + 'runDFMTasks.bat'
runAliceVisionTasks = workingDirectory + 'runAliceVisionTasks.bat'
runOpenCVTasks = workingDirectory + 'runOpenCVTasks.bat'

cameraSource = workingDirectory + 'data/' + dataset + '/cameras/'
imageSource = workingDirectory + 'data/' + dataset + '/visualize/'
visualize = workingDirectory + 'visualize/'
cameras = workingDirectory + 'cameras/'

anglesDFM = []
anglesAliceVision = []
anglesOpenCV = []

os.chdir(workingDirectory)

if loadAnglesFromFile:
    anglesDFM = loadAngles('dfm')
    anglesAliceVision = loadAngles('aliceVision')
    anglesOpenCV = loadAngles('openCV')
else:
    clearDirectory(visualize)
    clearDirectory(cameras)
    copy_tree(imageSource, visualize)
    copy_tree(cameraSource, cameras)

    #sp.call([runCommonTasks])
    
    try:
        t = Timer(5, killKeypointWindow)
        t.start()
        sp.run([runDFMTasks], timeout=6*60*60)
    except sp.TimeoutExpired:
        print('DFM timed out!')
    sp.call([runAliceVisionTasks])
    sp.call([runOpenCVTasks])

    anglesDFM = pe.calculateAngles('cameras/', 'Cache/dfm/StructureFromMotion/poses.txt')
    anglesAliceVision = pe.calculateAngles('cameras/', 'Cache/aliceVision/StructureFromMotion/poses.txt')
    anglesOpenCV = pe.calculateAngles('cameras/', 'Cache/openCV/StructureFromMotion/poses.txt')

    saveAngles('dfm', anglesDFM)
    saveAngles('aliceVision', anglesAliceVision)
    saveAngles('openCV', anglesOpenCV)

maaDFM = pe.calculateMAA(anglesDFM)
maaAliceVision = pe.calculateMAA(anglesAliceVision)
maaOpenCV = pe.calculateMAA(anglesOpenCV)

avgDFM = pe.calculateAverage(anglesDFM)
avgAliceVision = pe.calculateAverage(anglesAliceVision)
avgOpenCV = pe.calculateAverage(anglesOpenCV)

print("DFM avg: " + str(avgDFM))
print("AliceVision avg: " + str(avgAliceVision))
print("OpenCV avg: " + str(avgOpenCV))
