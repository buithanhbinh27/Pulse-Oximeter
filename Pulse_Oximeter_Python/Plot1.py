import csv
import matplotlib.pyplot as plt

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
        # Tạo biểu đồ đường
        # Tạo biểu đồ đường
        plt.figure(i)
        plt.plot(ir_data, label='IR Data')
        plt.plot(red_data, label='RED Data')

        # Đặt tiêu đề cho biểu đồ
        plt.title('Biểu đồ đường ' + str(i) + ' :')

        # Hiển thị chú thích
        plt.legend()

        # Hiển thị biểu đồ
        plt.show()

        i += 1

print("Data exported to ir_data.csv and red_data.csv.")
