import multiprocessing
import os
import math

import numpy as np
from joblib import Parallel, delayed
from tqdm import tqdm

from config import get_config, print_usage
from utils.io_helper import load_h5, save_h5
from utils.load_helper import load_calib
from utils.match_helper import compute_image_pairs
from utils.path_helper import (
    get_data_path, get_fullpath_list, get_item_name_list, get_kp_file,
    get_match_file, get_geom_file, get_geom_inl_file, get_filter_match_file,
    get_stereo_depth_projection_pre_match_file,
    get_stereo_depth_projection_refined_match_file,
    get_stereo_depth_projection_final_match_file,
    get_stereo_epipolar_pre_match_file, get_stereo_epipolar_refined_match_file,
    get_stereo_epipolar_final_match_file, get_stereo_path,
    get_stereo_pose_file, get_pairs_per_threshold,
    get_repeatability_score_file)
from utils.stereo_helper import (compute_stereo_metrics_from_E,
                                 is_stereo_complete)

from Scene import Scene
import matplotlib.pyplot as plt

data_dir = 'C:/Users/Administrator/Documents/Data/sacre_coeur/set_100'
estimation_dir = 'C:/Users/Administrator/Documents/Projects/Evaluation/poses_sacre_coeur.txt'

images_list = get_fullpath_list(data_dir, 'images')
image_names = get_item_name_list(images_list)

calib_list = get_fullpath_list(data_dir, 'calibration')
calib_dict = load_calib(calib_list)

estimation = Scene()
estimation.readAlembic(estimation_dir)

pairs = []

for i in range(0, len(image_names) - 1):
    for j in range(i + 1, len(image_names)):
        pairs.append(image_names[i] + "-" + image_names[j])

results = []
for pair in pairs:
    results.append(
        compute_stereo_metrics_from_E(
            img1 = images_list[image_names.index(pair.split('-')[0])],
            img2 = images_list[image_names.index(pair.split('-')[1])], 
            calib1 = calib_dict[pair.split('-')[0]], 
            calib2 = calib_dict[pair.split('-')[1]],
            pose1 = estimation.poses[pair.split('-')[0]],
            pose2 = estimation.poses[pair.split('-')[1]]
        )
    )

angles = []
for pair, result in zip(pairs, results):
    print(pair)
    print("\tRotation:")
    print("\t\tDifference:", math.degrees(result[2]))
    print("\tTranslation:")
    print("\t\tDifference:", math.degrees(result[3]))
    if result[2] > result[3]:
        angles.append(math.degrees(result[2]))
    else:
        angles.append(math.degrees(result[3]))

bins = np.arange(0, max(angles), 1)
plt.hist(np.clip(angles, bins[0], bins[-1]), bins=bins, density=False)
plt.show()
