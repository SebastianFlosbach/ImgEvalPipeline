import subprocess as sp
from shutil import copyfile, rmtree
import os
import sys

workingDirectory = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/'

runCommonTasks = workingDirectory + 'runCommonTasks.bat'
runDFMTasks = workingDirectory + 'runDFMTasks.bat'
runAliceVisionTasks = workingDirectory + 'runAliceVisionTasks.bat'

cameraSource = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/data/fountain_dense/cameras/'
imageSource = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/data/fountain_dense/visualize/'
visualize = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/visualize/'
cameras = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/cameras/'

indexImg1 = 0
indexImg2 = 1

def clearDirectory(path):
    for fileName in os.listdir(path):
        filePath = os.path.join(path, fileName)
        os.remove(filePath)

os.chdir(workingDirectory)

for indexImg1 in range(0, 5):
    for indexImg2 in range(indexImg1 + 1, indexImg1 + 5):
        clearDirectory(visualize)
        clearDirectory(cameras)

        # Copy images
        copyfile(imageSource + str(indexImg1) + '.jpg', visualize + '0.jpg')
        copyfile(imageSource + str(indexImg2) + '.jpg', visualize + '1.jpg')

        # Copy camera data
        copyfile(cameraSource + str(indexImg1).rjust(4, '0') + '.png.camera', cameras + str(indexImg1) + '.png.camera')
        copyfile(cameraSource + str(indexImg2).rjust(4, '0') + '.png.camera', cameras + str(indexImg2) + '.png.camera')

        sp.call([runCommonTasks])
        sp.call([runDFMTasks])
        sp.call([runAliceVisionTasks])

        copyfile('Cache/dfm/StructureFromMotion/poses.txt', 'Evaluation/Poses/dfm/poses_' + str(indexImg1) + '_' + str(indexImg2) + '.txt')
        copyfile('Cache/aliceVision/StructureFromMotion/poses.txt', 'Evaluation/Poses/aliceVision/poses_' + str(indexImg1) + '_' + str(indexImg2) + '.txt')
