import numpy as np
import matplotlib.pyplot as plt
from scipy.fftpack import fft, fftfreq
from scipy.signal import find_peaks

def normalize(array):
    min_val = np.min(array)
    max_val = np.max(array)
    return (array - min_val) / (max_val - min_val)

def calculate_heart_rate(ir_data):
    max_amplitude_index = np.argmax(np.abs(ir_data))
    max_frequency = np.abs(fftfreq(len(ir_data))[max_amplitude_index])
    return max_frequency * 60


def calculate_spo2(ir_data, red_data):
    # Calculate AC and DC components of the signals
    ir_ac = ir_data - np.mean(ir_data)
    red_ac = red_data - np.mean(red_data)

    ir_dc = np.mean(ir_data)
    red_dc = np.mean(red_data)

    # Find peaks in the AC signals
    ir_peaks, _ = find_peaks(ir_ac)
    red_peaks, _ = find_peaks(red_ac)

    # Ensure both signals have the same number of peaks
    min_peaks = min(len(ir_peaks), len(red_peaks))
    ir_peaks = ir_peaks[:min_peaks]
    red_peaks = red_peaks[:min_peaks]

    # Calculate the RMS values for the AC components using the peaks
    ir_rms = np.sqrt(np.mean(ir_ac[ir_peaks] ** 2))
    red_rms = np.sqrt(np.mean(red_ac[red_peaks] ** 2))

    # Calculate the ratio of ratios (R)
    R = (red_rms / red_dc) / (ir_rms / ir_dc)

    # Use the approximation formula to calculate SpO2
    spo2_red =  red_dc - red_rms * R
    spo2_ir = ir_dc - ir_rms * R
    spo2 = (spo2_red + spo2_ir)/2
    return spo2

count = 0
sampling_frequency = 64

# Read data from the text file
with open('output.txt', 'r') as file:
    for line in file:
        data = line.strip().split(',')

        # Separate IR and RED data
        ir_data = np.array(data[:64], dtype=float)
        red_data = np.array(data[64:128], dtype=float)

        red_normalized = normalize(red_data)
        ir_normalized = normalize(ir_data)

        # FFT and plot magnitude
        red_fft_magnitude = np.abs(fft(red_data))
        ir_fft_magnitude = np.abs(fft(ir_data))


        print(calculate_heart_rate(ir_data))
        print(calculate_spo2(red_data, ir_data))

        plt.figure(figsize=(12, 6))
        plt.subplot(2, 2, 1)
        plt.plot(red_data, label='Red')
        plt.title(f'Red Data Line {count + 1}')
        plt.legend()

        plt.subplot(2, 2, 2)
        plt.plot(ir_data, label='IR', color='orange')
        plt.title(f'IR Data Line {count + 1}')
        plt.legend()

        plt.subplot(2, 2, 3)
        plt.plot(red_fft_magnitude, label='Red FFT')
        plt.title(f'Red FFT Magnitude Line {count + 1}')
        plt.legend()

        plt.subplot(2, 2, 4)
        plt.plot(ir_fft_magnitude, label='IR FFT', color='orange')
        plt.title(f'IR FFT Magnitude Line {count + 1}')
        plt.legend()

        plt.tight_layout()
        plt.show()

        count += 1