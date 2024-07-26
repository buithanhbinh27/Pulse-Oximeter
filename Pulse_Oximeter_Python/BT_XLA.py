import cv2
import numpy as np
import matplotlib.pyplot as plt


# Hàm tính toán và vẽ histogram
def calc_and_plot_histogram(image, title):
    hist = cv2.calcHist([image], [0], None, [256], [0, 256])
    plt.figure()
    plt.title(title)
    plt.xlabel("Pixel value")
    plt.ylabel("Frequency")
    plt.plot(hist)
    plt.xlim([0, 256])
    plt.show()


# Hàm cân bằng histogram
def equalize_histogram(image):
    equalized_img = cv2.equalizeHist(image)
    return equalized_img


# Hàm dịch chuyển histogram
def shift_histogram(image, shift_value):
    shifted_img = np.clip(image + shift_value, 0, 255).astype(np.uint8)
    return shifted_img


# Hàm hiệu chỉnh histogram theo histogram định trước
def match_histograms(source, template):
    matched = cv2.matchTemplate(source, template)
    return matched


# Ví dụ thực hiện các chức năng trên ảnh xám

#Đọc ảnh xám từ file
image = cv2.imread('source.jpg', cv2.IMREAD_GRAYSCALE)

# Tính toán và vẽ histogram ban đầu
calc_and_plot_histogram(image, "Initial Histogram")

# Cân bằng histogram
equalized_image = equalize_histogram(image)
calc_and_plot_histogram(equalized_image, "Equalized Histogram")

# Dịch chuyển histogram
shifted_image = shift_histogram(image, 50)
calc_and_plot_histogram(shifted_image, "Shifted Histogram")

# Hiệu chỉnh histogram (ví dụ sử dụng lại ảnh ban đầu)
matched_image = match_histograms(image, equalized_image)
calc_and_plot_histogram(matched_image, "Matched Histogram")

# Hiển thị các ảnh kết quả
cv2.imshow('Original Image', image)
cv2.imshow('Equalized Image', equalized_image)
cv2.imshow('Shifted Image', shifted_image)
cv2.imshow('Matched Image', matched_image)
cv2.waitKey(0)
cv2.destroyAllWindows()