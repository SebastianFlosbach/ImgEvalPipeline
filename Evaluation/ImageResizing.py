from PIL import Image
import glob

sourcePath = 'C:/Users/Administrator/Documents/Data/Fotos Affe/'
destinationPath = "C:/Users/Administrator/Desktop/ImgEvalPipeline/visualize/"

fileCounter = 0
fileList = glob.glob(sourcePath + '*.jpg') + glob.glob(sourcePath + '*.png')
for file in fileList:
    image = Image.open(file)
    aspectRatio = image.width / image.height
    image = image.resize(size=(1280, int(1280 / aspectRatio)), resample=Image.ANTIALIAS)
    image.save(destinationPath + str(fileCounter) + '.jpg')
    fileCounter += 1
    print('Saved ' + file + ' to ' + destinationPath + str(fileCounter) + '.jpg with resolution ' + str(image.width) + 'x' + str(image.height))
