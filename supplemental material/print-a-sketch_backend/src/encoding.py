import cv2
import numpy as np


def getImageCode(path):
    originalImage = cv2.imread(path)
    grayImage = cv2.cvtColor(originalImage, cv2.COLOR_BGR2GRAY)
    (thresh, blackAndWhiteImage) = cv2.threshold(grayImage, 127, 255, cv2.THRESH_BINARY)
    image = blackAndWhiteImage

    # image = blackAndWhiteImage
    width = (len(image[0]))
    scale_percent = (128 / width) * 100  # percent of original size
    width = int(image.shape[1] * scale_percent / 100)
    height = int(image.shape[0] * scale_percent / 100)
    dim = (width, height)

    # resize image
    resized = cv2.resize(image, dim, interpolation=cv2.INTER_AREA)
    f = open(f'{path}_bw.txt', "a")
    for row in resized:
        for pixel in row:
            if pixel == 255:
                f.write('1')
            else:
                f.write('0')
        f.write('\n')
    f.close()
    binary_img = []
    with open(f'{path}_bw.txt', "r") as a_file:
        for line in a_file:
            stripped_line = line.strip()
            list = [int(line[i:i + 8], 2) for i in range(0, len(stripped_line), 8)]
            arr = bytearray(list)
            binary_img.append(arr)

    img = np.zeros((128, 128))
    for i in range(len(resized)):
        for j in range(len(resized[i])):
            if resized[i][j] == 255:
                img[i][j] = 1
            else:
                img[i][j] = 0

    return img, binary_img


def show_image_in_consul(img):
    for line in img:
        for pixel in line:
            print(int(pixel), end="")
        print()
