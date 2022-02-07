import PoseEvaluation as pe
import numpy as np
import matplotlib.pyplot as plt

def plotAngles(angles, threshold, cumulative=False):
    bins = np.arange(0, threshold + 1, 1)
    plt.hist(angles, bins=bins, density=False, cumulative=cumulative)
    plt.show()

angles = [1, 2, 3, 4, 5]

maa = pe.calculateMAA(angles, 10)
#plotAngles(angles, 10, True)
