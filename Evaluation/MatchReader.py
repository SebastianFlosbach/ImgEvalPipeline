import cv2 as cv

image1Path = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/visualize/0.jpg'
image2Path = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/visualize/14.jpg'
kp1Path = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/Cache/dfm/ExtractionAndMatching/regions/1240514759.sift.feat'
kp2Path = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/Cache/dfm/ExtractionAndMatching/regions/2046071108.sift.feat'
matchPath = 'C:/Users/Administrator/Desktop/ImgEvalPipeline/matches.txt'

image1 = cv.imread(image1Path)
image2 = cv.imread(image2Path)

keyPointsImage1 = []
keyPointsImage2 = []

with open(kp1Path, 'r') as file:
    for line in file.readlines():
        data = line.split()
        x = float(data[0])
        y = float(data[1])
        
        kp = cv.KeyPoint(x, y, -1)

        keyPointsImage1.append(kp)

with open(kp2Path, 'r') as file:
    for line in file.readlines():
        data = line.split()
        x = float(data[0])
        y = float(data[1])
        
        kp = cv.KeyPoint(x, y, -1)

        keyPointsImage2.append(kp)

matches = []
with open(matchPath, 'r') as file:
    lines = file.readlines()
    for line in lines[3:]:
        data = line.split()

        indexKP1 = int(data[0])
        indexKP2 = int(data[1])
        
        matches.append(cv.DMatch(indexKP1, indexKP2, 0, 0))

image = cv.drawMatches(image1, keyPointsImage1, image2, keyPointsImage2, matches, None)
cv.imshow("Matches", image)
cv.waitKey()
