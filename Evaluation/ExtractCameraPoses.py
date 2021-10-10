import numpy as np
from Scene import Scene
import math
import matplotlib.pyplot as plt

def calcAngularDifference(q_gt, q):
    eps = 1e-15
    q = q / (np.linalg.norm(q) + eps)
    q_gt = q_gt / (np.linalg.norm(q_gt) + eps)
    loss_q = np.maximum(eps, (1.0 - np.sum(q * q_gt)**2))
    err_q = np.arccos(1 - 2 * loss_q)
    return err_q

groundTruth = Scene()
estimation = Scene()

groundTruth.readAlembic('C:/Users/Administrator/Documents/Projects/Evaluation/poses_affe_downscaled_gt.txt')
estimation.readAlembic('C:/Users/Administrator/Documents/Projects/Evaluation/poses_affe_downscaled.txt')

estimationKeys = list(estimation.poses.keys())
angles = []
for eKey1 in list(estimationKeys):
    estimationKeys.remove(eKey1)
    if not eKey1 in groundTruth.poses:
        continue
    for eKey2 in list(estimationKeys):
        if not eKey2 in groundTruth.poses:
            continue
        diff_gt = calcAngularDifference(groundTruth.poses[eKey1], groundTruth.poses[eKey2])
        diff_e = calcAngularDifference(estimation.poses[eKey1], estimation.poses[eKey2])
        print(eKey1, eKey2)
        print("\tEstimation:", math.degrees(diff_e))
        print("\tGround Truth:", math.degrees(diff_gt))
        print("\tDifference:", math.degrees(abs(diff_gt - diff_e)))
        angles.append(math.degrees(abs(diff_gt - diff_e)))

bins = np.arange(0, 10, 1)
plt.hist(angles, bins=bins, density=True)
plt.show()