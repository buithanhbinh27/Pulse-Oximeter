import numpy as np
import matplotlib.pyplot as plt
from scipy.fftpack import fft, fftfreq
import pandas as pd


def normalize(array):
    min_val = np.min(array)
    max_val = np.max(array)
    return (array - min_val) / (max_val - min_val)


def calculate_heart_rate(ir_data):
    max_amplitude_index = np.argmax(np.abs(ir_data))
    max_frequency = np.abs(fftfreq(len(ir_data))[max_amplitude_index])
    HR = max_frequency * 60
    print(HR)
    return


# Tính SpO2 từ tín hiệu Red và IR
def calculate_spo2(red_data, ir_data):
    red_ac = np.std(red_data)
    ir_ac = np.std(ir_data)
    red_dc = np.mean(red_data)
    ir_dc = np.mean(ir_data)

    r = (red_ac / red_dc) / (ir_ac / ir_dc)
    spo2 = 110 - 25 * r  # Công thức thực nghiệm đơn giản

    print(spo2)
    return

count=0;
sampling_frequency = 64
# Read data from the text file
with open('output.txt', 'r') as file:
    for line in file:
        data = line.strip().split(',')

        # Separate IR and RED data
        ir_data = data[:64]
        red_data = data[64:128]

        ir_data = np.array(ir_data, dtype=float)
        red_data = np.array(red_data, dtype=float)

        red_normalized = normalize(red_data)
        ir_normalized = normalize(ir_data)

        calculate_heart_rate(ir_data)
        calculate_spo2(red_data, ir_data)

        # Biểu diễn các phần tử của mảng trên cùng 1 đồ thị
        plt.figure(figsize=(12, 6))
        plt.subplot(2, 2, 1)
        plt.plot(red_data, label='Red')
        plt.title(f'Red Data Line {count + 1}')
        plt.legend()

        plt.subplot(2, 2, 2)
        plt.plot(ir_data, label='IR', color='orange')
        plt.title(f'IR Data Line {count + 1}')
        plt.legend()

        # Khai triển FFT và biểu diễn đáp ứng biên độ
        red_fft_magnitude = fft(red_data)
        ir_fft_magnitude = fft(ir_data)

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

        count+=1



