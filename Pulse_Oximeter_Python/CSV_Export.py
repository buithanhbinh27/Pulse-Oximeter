import serial
import openpyxl
from openpyxl import Workbook

# Thiết lập cổng Serial
ser = serial.Serial('COM3', 115200)  # Thay COM3 bằng cổng Serial của bạn
ser.flush()

# Tạo workbook mới và sheet
wb = Workbook()
ws = wb.active

# Đặt header cho các cột
headers = ['IR'+str(i) for i in range(64)] + ['Red'+str(i) for i in range(64)]
ws.append(headers)

try:
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('utf-8').strip()
            data = line.split(',')

            if len(data) == 128:  # 64 mẫu IR và 64 mẫu Red
                row = [float(val) for val in data if val]
                ws.append(row)
                print("Row added to Excel")

            # Lưu file Excel
            wb.save("output.xlsx")

except KeyboardInterrupt:
    print("Stopping script...")

finally:
    ser.close()
    wb.save("output.xlsx")
    print("Excel file saved.")
