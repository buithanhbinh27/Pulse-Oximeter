import csv

# Read data from the text file
with open('output.txt', 'r') as file:
    data = file.read().split(',')

# Separate IR and RED data
ir_data = data[:64]
red_data = data[64:]

# Write IR data to a CSV file
with open('ir_data.csv', 'w', newline='') as ir_file:
    writer = csv.writer(ir_file)
    writer.writerow(['IR Data'])
    writer.writerow(ir_data)

# Write RED data to a CSV file
with open('red_data.csv', 'w', newline='') as red_file:
    writer = csv.writer(red_file)
    writer.writerow(['RED Data'])
    writer.writerow(red_data)

print("Data exported to ir_data.csv and red_data.csv.")