import numpy as np
import quaternion
from Scene import Scene
import math
import matplotlib.pyplot as plt

def quaternionAngle(q_gt, q):
   diff = q_gt * np.conjugate(q)
   return 2 * np.arccos(diff.w)

def translationAngle(t_gt, t):
    eps = 1e-15
    t = t / (np.linalg.norm(t) + eps)
    t_gt = t_gt / (np.linalg.norm(t_gt) + eps)
    loss_t = np.maximum(eps, (1.0 - np.sum(t * t_gt)**2))
    err_t = np.arccos(np.sqrt(1 - loss_t))
    return err_t

def vecError(v_gt, v):
    unit_v_gt = v_gt / np.linalg.norm(v_gt)
    unit_v = v / np.linalg.norm(v)
    d = np.dot(unit_v_gt, unit_v)
    err_v = np.arccos(d)
    return err_v

def rotationMatrixDelta(r_1, r_2):
    return np.dot(r_2, r_1.T)

def rotationQuatDelta(q_1, q_2):
    return q_2 * np.conjugate(q_1)

def translationDelta(t_1, t_2, dR):
    return t_2 - np.dot(dR, t_1)

def reduce(m):
    return [m[0][:3], m[1][:3], m[2][:3]]

groundTruth = Scene()
estimation = Scene()

groundTruth.readMVS('C:/Users/Administrator/Desktop/ImgEvalPipeline/cameras/')
estimation.readAlembic('C:/Users/Administrator/Desktop/ImgEvalPipeline/Cache/aliceVision/StructureFromMotion/poses.txt')

estimationKeys = list(estimation.poses.keys())
angles = []
for eKey1 in list(estimationKeys):
    estimationKeys.remove(eKey1)
    if not eKey1 in groundTruth.poses:
        #angles.append(float('inf'))
        continue
    for eKey2 in list(estimationKeys):
        diff_gt = quaternionAngle(groundTruth.poses[eKey1].rotation, groundTruth.poses[eKey2].rotation)
        diff_e = quaternionAngle(estimation.poses[eKey1].rotation, estimation.poses[eKey2].rotation)
        dq = abs(diff_gt - diff_e)
        print(eKey1, eKey2)
        print("\tRotation:")
        print("\t\tEstimation:", math.degrees(diff_e))
        print("\t\tGround Truth:", math.degrees(diff_gt))
        print("\t\tDifference:", math.degrees(dq))
        if(math.degrees(dq) < 180):
            angles.append(math.degrees(dq))
        else:
            angles.append(360 - math.degrees(dq))

threshold = 10
counter = 0
cummulatedAngles = 0
for angle in np.clip(angles, 0, threshold):
    counter += 1
    cummulatedAngles += threshold - math.floor(angle)

cummulatedAngles /= (threshold * counter)

print("Min Angle: ", min(angles))
print("Max Angle: ", max(angles))
print("mAA: ", cummulatedAngles)

bins = np.arange(0, threshold, 1)
plt.hist(np.clip(angles, bins[0], bins[-1]), bins=bins, density=False, cumulative=False)
#plt.hist(angles, bins=bins, density=False, cumulative=False)
#plt.show()
