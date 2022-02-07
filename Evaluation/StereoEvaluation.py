import subprocess as sp
from threading import Timer
from shutil import copyfile
import os
import PoseEvaluation as pe
import matplotlib.pyplot as plt
import numpy as np
import glob
import win32gui
import win32con
from pathlib import Path

# Settings
dataset = 'fountain_dense'
dataset_name = 'fountain'
imageDst = 2
loadAnglesFromFile = False
onlyShowHistogram = False
# Settings

def clearDirectory(path):
    for fileName in os.listdir(path):
        filePath = os.path.join(path, fileName)
        os.remove(filePath)

def saveAngles(name, angles):
    Path('data/' + dataset + '/r' + str(imageDst)).mkdir(parents=True, exist_ok=True)
    with open('data/' + dataset + '/r' + str(imageDst) + '/angles_' + name + '.txt', 'w') as file:
        for angle in angles:
            file.write(str(angle) + '\n')

def loadAngles(name):
    angles = []
    with open('data/' + dataset + '/r' + str(imageDst) + '/angles_' + name + '.txt', 'r') as file:
        for angle in file:
            angles.append(float(angle))
    return angles

def plotHistogram(name, angles, threshold, cumulative=False, onlyShow=False):
    plt.rc('axes', labelsize=24)
    plt.rc('xtick', labelsize=14)
    plt.rc('ytick', labelsize=14)
    bins = np.arange(0, threshold + 1, 1)
    plt.hist(angles, bins=bins, density=False, cumulative=cumulative, color='royalblue')
    #plt.title(dataset_name + ' - ' + name)
    plt.xlabel('Winkeldifferenz in °')
    plt.ylabel('Bildpaare')
    plt.xlim(0, threshold)
    plt.ylim(0, len(angles))
    plt.tight_layout()
    if onlyShow:
        plt.show()
    else:
        plt.savefig('data/' + dataset + '/r' + str(imageDst) + '/' + name + '.png')

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

nbImages = len(glob.glob(imageSource + '*.jpg'))

indexImg1 = 0
indexImg2 = 1

os.chdir(workingDirectory)

anglesDFM = []
anglesAliceVision = []
anglesOpenCV = []

if loadAnglesFromFile:
    anglesDFM = loadAngles('dfm')
    anglesAliceVision = loadAngles('aliceVision')
    anglesOpenCV = loadAngles('openCV')
else:
    for indexImg1 in range(0, nbImages - 1):
        maxIndex = indexImg1 + imageDst + 1
        maxIndex = maxIndex if maxIndex < nbImages else nbImages
        for indexImg2 in range(indexImg1 + 1, maxIndex):
            clearDirectory(visualize)
            clearDirectory(cameras)

            # Copy images
            copyfile(imageSource + str(indexImg1) + '.jpg', visualize + '0.jpg')
            copyfile(imageSource + str(indexImg2) + '.jpg', visualize + '1.jpg')

            # Copy camera data
            copyfile(cameraSource + str(indexImg1).rjust(4, '0') + '.png.camera', cameras + str(indexImg1) + '.png.camera')
            copyfile(cameraSource + str(indexImg2).rjust(4, '0') + '.png.camera', cameras + str(indexImg2) + '.png.camera')

            sp.call([runCommonTasks])

            try:
                t = Timer(3, killKeypointWindow)
                t.start()
                sp.run([runDFMTasks], timeout=5*60)
            except sp.TimeoutExpired:
                print('DFM timed out!')
                continue

            sp.call([runAliceVisionTasks])
            sp.call([runOpenCVTasks])

            angles = pe.calculateAngles('cameras/', 'Cache/dfm/StructureFromMotion/poses.txt')
            anglesDFM.extend(angles)
            angles = pe.calculateAngles('cameras/', 'Cache/aliceVision/StructureFromMotion/poses.txt')
            anglesAliceVision.extend(angles)
            angles = pe.calculateAngles('cameras/', 'Cache/openCV/StructureFromMotion/poses.txt')
            anglesOpenCV.extend(angles)
    saveAngles('dfm', anglesDFM)
    saveAngles('aliceVision', anglesAliceVision)
    saveAngles('openCV', anglesOpenCV)

maaDFM = pe.calculateMAA(anglesDFM)
avgDFM = pe.calculateAverage(anglesDFM)
maaAliceVision = pe.calculateMAA(anglesAliceVision)
avgAliceVision = pe.calculateAverage(anglesAliceVision)
maaOpenCV = pe.calculateMAA(anglesOpenCV)
avgOpenCV = pe.calculateAverage(anglesOpenCV)

print("DFM avg: " + str(avgDFM))
print("AliceVision avg: " + str(avgAliceVision))
print("OpenCV avg: " + str(avgOpenCV))

plotHistogram('DFM', anglesDFM, 10, True, onlyShowHistogram)
plotHistogram('AliceVision', anglesAliceVision, 10, True, onlyShowHistogram)
plotHistogram('OpenCV', anglesOpenCV, 10, True, onlyShowHistogram)
