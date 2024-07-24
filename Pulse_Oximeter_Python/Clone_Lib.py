import csv
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import savgol_filter
from scipy.fftpack import fft, fftfreq
from scipy.signal import find_peaks


def maxim_find_peaks(pn_x, n_size, n_min_height, n_min_distance, n_max_num):
    pn_locs = [0] * n_max_num
    n_npks = 0

    pn_locs, n_npks = maxim_peaks_above_min_height(pn_x, n_size, n_min_height)
    pn_locs, n_npks = maxim_remove_close_peaks(pn_locs, n_npks, pn_x, n_min_distance)

    n_npks = min(n_npks, n_max_num)
    return n_npks, pn_locs


def maxim_peaks_above_min_height(pn_x, n_size, n_min_height):
    pn_locs = []
    n_npks = 0
    i = 1

    while i < n_size - 1:
        if pn_x[i] > n_min_height and pn_x[i] > pn_x[i - 1]:
            n_width = 1
            while i + n_width < n_size and pn_x[i] == pn_x[i + n_width]:
                n_width += 1
            if pn_x[i] > pn_x[i + n_width] and n_npks < MAX_NUM:
                pn_locs.append(i)
                n_npks += 1
                i += n_width + 1
            else:
                i += n_width
        else:
            i += 1

    return pn_locs, n_npks


def maxim_remove_close_peaks(pn_locs, n_npks, pn_x, n_min_distance):
    pn_locs = maxim_sort_indices_descend(pn_x, pn_locs, n_npks)
    i = -1
    while i < len(pn_locs) - 1:
        n_old_npks = len(pn_locs)
        n_npks = i + 1
        for j in range(i + 1, n_old_npks):
            n_dist = pn_locs[j] - (-1 if i == -1 else pn_locs[i])
            if abs(n_dist) > n_min_distance:
                pn_locs[n_npks] = pn_locs[j]
                n_npks += 1
        i += 1

    pn_locs = maxim_sort_ascend(pn_locs, n_npks)
    return pn_locs, n_npks


def maxim_sort_ascend(pn_x, n_size):
    pn_x.sort()
    return pn_x


def maxim_sort_indices_descend(pn_x, pn_indx, n_size):
    indexed_pn_x = [(pn_x[i], i) for i in pn_indx]
    indexed_pn_x.sort(reverse=True, key=lambda x: x[0])
    return [x[1] for x in indexed_pn_x]





# Constants
FS = 25
BUFFER_SIZE = 100
MA4_SIZE = 4
MAX_NUM = 15
MIN_HEIGHT = 30
MIN_DISTANCE = 4

# SpO2 table
uch_spo2_table = [95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98, 99, 99, 99, 99,
              99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
              100, 100, 100, 100, 99, 99, 99, 99, 99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97,
              97, 97, 96, 96, 96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91,
              90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83, 82, 82, 81, 81,
              80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73, 72, 72, 71, 70, 69, 69, 68, 67,
              66, 66, 65, 64, 63, 62, 62, 61, 60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50,
              49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29,
              28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10, 9, 7, 6, 5,
              3, 2, 1]

with open('output_100_samples.txt', 'r') as file:
    data = file.read().split(',')
    # Separate IR and RED data
    ir_data = data[:100]
    ir_data = [int(value) for value in ir_data]
    red_data = data[100:200]
    red_data = [int(value) for value in red_data]


    spo2 = -999
    spo2_valid = 0
    heart_rate = -999
    hr_valid = 0

    an_x = [0] * BUFFER_SIZE
    an_y = [0] * BUFFER_SIZE

    # Calculate DC mean and subtract DC from IR
    ir_mean = sum(ir_data) // BUFFER_SIZE

    # Remove DC and invert signal for peak detection
    for k in range(BUFFER_SIZE):
        an_x[k] = -1 * (ir_data[k] - ir_mean)

    # 4-point Moving Average
    for k in range(BUFFER_SIZE - MA4_SIZE):
        an_x[k] = (an_x[k] + an_x[k + 1] + an_x[k + 2] + an_x[k + 3]) // 4

    # Calculate threshold
    n_th1 = sum(an_x) // BUFFER_SIZE
    n_th1 = max(30, min(n_th1, 60))

    an_ir_valley_locs = [0] * MAX_NUM
    an_ratio = [0] * 5

    # Find peaks (valleys) in the IR signal
    n_npks, an_ir_valley_locs = maxim_find_peaks(an_x, BUFFER_SIZE, n_th1, MIN_DISTANCE, MAX_NUM)

    # Heart rate calculation
    if n_npks >= 2:
        n_peak_interval_sum = sum(an_ir_valley_locs[k] - an_ir_valley_locs[k - 1] for k in range(1, n_npks)) // (
                n_npks - 1)
        heart_rate = (FS * 60) // n_peak_interval_sum
        hr_valid = 1
    else:
        heart_rate = -999
        hr_valid = 0

    # Reload raw values for SpO2 calculation
    for k in range(BUFFER_SIZE):
        an_x[k] = ir_data[k]
        an_y[k] = red_data[k]

    n_ratio_average = 0
    n_i_ratio_count = 0

    # Find precise min near valley locations and calculate SpO2
    for k in range(n_npks - 1):
        if an_ir_valley_locs[k + 1] - an_ir_valley_locs[k] > 3:
            n_y_dc_max = max(an_y[an_ir_valley_locs[k]:an_ir_valley_locs[k + 1]])
            n_x_dc_max = max(an_x[an_ir_valley_locs[k]:an_ir_valley_locs[k + 1]])
            n_y_dc_max_idx = an_y.index(n_y_dc_max)
            n_x_dc_max_idx = an_x.index(n_x_dc_max)

            n_y_ac = (an_y[an_ir_valley_locs[k + 1]] - an_y[an_ir_valley_locs[k]]) * (
                    n_y_dc_max_idx - an_ir_valley_locs[k])
            n_y_ac = an_y[an_ir_valley_locs[k]] + n_y_ac // (an_ir_valley_locs[k + 1] - an_ir_valley_locs[k])
            n_y_ac = an_y[n_y_dc_max_idx] - n_y_ac

            n_x_ac = (an_x[an_ir_valley_locs[k + 1]] - an_x[an_ir_valley_locs[k]]) * (
                    n_x_dc_max_idx - an_ir_valley_locs[k])
            n_x_ac = an_x[an_ir_valley_locs[k]] + n_x_ac // (an_ir_valley_locs[k + 1] - an_ir_valley_locs[k])
            n_x_ac = an_x[n_x_dc_max_idx] - n_x_ac

            n_nume = (n_y_ac * n_x_dc_max) >> 7
            n_denom = (n_x_ac * n_y_dc_max) >> 7

            if n_denom > 0 and n_i_ratio_count < 5 and n_nume != 0:
                an_ratio[n_i_ratio_count] = (n_nume * 100) // n_denom
                n_i_ratio_count += 1

    n_middle_idx = n_i_ratio_count // 2

    if n_middle_idx > 1:
        n_ratio_average = (an_ratio[n_middle_idx - 1] + an_ratio[n_middle_idx]) // 2
    else:
        n_ratio_average = an_ratio[n_middle_idx]

    if 2 < n_ratio_average < 184:
        n_spo2_calc = uch_spo2_table[n_ratio_average]
        pn_spo2 = n_spo2_calc
        pch_spo2_valid = 1
    else:
        pn_spo2 = -999
        pch_spo2_valid = 0

    print(f"HR: {heart_rate}")
    print(f"SpO2: {pn_spo2}")

