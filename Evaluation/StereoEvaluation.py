import subprocess as sp
from threading import Timer
from shutil import copyfile, rmtree
import os
import PoseEvaluation as pe
import matplotlib.pyplot as plt
import numpy as np
import glob
import win32gui
import win32con

# Settings
dataset = 'castle_dense'
imageDst = 1
loadAnglesFromFile = False
# Settings

def clearDirectory(path):
    for fileName in os.listdir(path):
        filePath = os.path.join(path, fileName)
        os.remove(filePath)

def saveAngles(name, angles):
    with open('data/' + dataset + '/r' + str(imageDst) + '/angles_' + name + '.txt', 'w') as file:
        for angle in angles:
            file.write(str(angle) + '\n')

def loadAngles(name):
    angles = []
    with open('data/' + dataset + '/r' + str(imageDst) + '/angles_' + name + '.txt', 'r') as file:
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

if loadAnglesFromFile:
    anglesDFM = loadAngles('dfm')
    anglesAliceVision = loadAngles('aliceVision')
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

            #sp.call([runDFMTasks])
            sp.call([runAliceVisionTasks])


            angles = pe.calculateAngles('cameras/', 'Cache/dfm/StructureFromMotion/poses.txt')
            anglesDFM.extend(angles)
            angles = pe.calculateAngles('cameras/', 'Cache/aliceVision/StructureFromMotion/poses.txt')
            anglesAliceVision.extend(angles)
    saveAngles('dfm', anglesDFM)
    saveAngles('aliceVision', anglesAliceVision)

maaDFM = pe.calculateMAA(anglesDFM)
maaAliceVision = pe.calculateMAA(anglesAliceVision)

plotAngles(anglesDFM, 10, True)
plotAngles(anglesAliceVision, 10, True)
