import csv
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import savgol_filter
# Open the text file
with open('output.txt', 'r') as file:
    lines = file.readlines()

# Open the CSV files
with open('ir_data.csv', 'w', newline='') as ir_file, open('red_data.csv', 'w', newline='') as red_file:
    ir_writer = csv.writer(ir_file)
    red_writer = csv.writer(red_file)

    # Write headers
    ir_writer.writerow(['IR Data'])
    red_writer.writerow(['RED Data'])

    i = 0
    # Process each line
    for line in lines:
        # Remove the trailing comma and split the line into data
        data = line.rstrip(',\n').split(',')

        # Separate IR and RED data
        ir_data = list(map(float, data[:64]))
        red_data = list(map(float, data[64:]))

        # Write the data to the CSV files
        # for i in range(len(ir_data)):

        ir_writer.writerow(ir_data)
        red_writer.writerow(red_data)
        # Tạo hình mới
        fig, axs = plt.subplots(2, 2, figsize=(10, 10))

        # Vẽ biểu đồ IR trên hình trên bên trái
        axs[0, 0].plot(ir_data, label='IR Data')
        axs[0, 0].set_title('Biểu đồ IR ' + str(i) + ' :')
        axs[0, 0].legend()

        # Vẽ biểu đồ RED trên hình trên bên phải
        axs[0, 1].plot(red_data, label='RED Data')
        axs[0, 1].set_title('Biểu đồ RED ' + str(i) + ' :')
        axs[0, 1].legend()

        # Biến đổi Fourier cho dữ liệu IR và vẽ biểu đồ
        ir_fft = np.fft.fft(ir_data)
        axs[1, 0].plot(np.abs(ir_fft), label='Fourier IR Data')
        axs[1, 0].set_title('Biểu đồ Fourier IR ' + str(i) + ' :')
        axs[1, 0].legend()

        # Biến đổi Fourier cho dữ liệu RED và vẽ biểu đồ
        red_fft = np.fft.fft(red_data)
        axs[1, 1].plot(np.abs(red_fft), label='Fourier RED Data')
        axs[1, 1].set_title('Biểu đồ Fourier RED ' + str(i) + ' :')
        axs[1, 1].legend()

        plt.show()

        # Apply Savitzky-Golay filter
        ir_data_smooth = savgol_filter(ir_data, 4, 2)  # window size 5, polynomial order 4
        red_data_smooth = savgol_filter(red_data, 4, 2)  # window size 5, polynomial order 4

        # Write the data to the CSV files
        ir_writer.writerow(ir_data_smooth)
        red_writer.writerow(red_data_smooth)

        # Compute centered moving average
        window_size = 64
        ir_data_smooth = np.convolve(ir_data_smooth, np.ones(window_size) / window_size, mode='same')
        red_data_smooth = np.convolve(red_data_smooth, np.ones(window_size) / window_size, mode='same')

        # Subtract the moving average from the data to get high-pass-filtered result
        ir_data_highpass = ir_data - ir_data_smooth
        red_data_highpass = red_data - red_data_smooth


        # Choose the threshold as 1/3 of the max PPG signal
        threshold_ir = np.max(ir_data_highpass) / 3
        threshold_red = np.max(red_data_highpass) / 3

        print("threshold from IR data: ", threshold_ir)
        print("threshold from RED data: ", threshold_red)

        # Count the number of times the signal crosses the threshold
        ir_crossings = np.where(np.diff(np.sign(ir_data_highpass - threshold_ir)))[0]
        red_crossings = np.where(np.diff(np.sign(red_data_highpass - threshold_red)))[0]

        # Print the number of crossings (heart rate)
        print("Heart rate from IR data: ", len(ir_crossings))
        print("Heart rate from RED data: ", len(red_crossings))

print("Data exported to ir_data.csv and red_data.csv.")
