import numpy as np
from Scene import Scene

def calcAngularDifference(q_gt, q):
    eps = 1e-15
    q = q / (np.linalg.norm(q) + eps)
    q_gt = q_gt / (np.linalg.norm(q_gt) + eps)
    loss_q = np.maximum(eps, (1.0 - np.sum(q * q_gt)**2))
    err_q = np.arccos(1 - 2 * loss_q)
    return err_q


groundTruth = Scene()
estimation = Scene()


groundTruth.readCalibration('C:/Users/Administrator/Documents/Data/sacre_coeur/set_100/calibration')
estimation.readAlembic('C:/Users/Administrator/Documents/Projects/AlembicReader/AlembicReader/out/build/x64-Debug/AlembicReader/Poses')

for key in estimation.poses:
    if key in groundTruth.poses:
        print(key, " ", calcAngularDifference(groundTruth.poses[key], estimation.poses[key]))
