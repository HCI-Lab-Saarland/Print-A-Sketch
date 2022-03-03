import cv2
import numpy as np

# This is the distance between print-head and the camera.
distance_between_head_and_camera = 95  # lines of data  = 1.3 cm


def reach_the_blob(length=distance_between_head_and_camera):  # create empty lines of data
    data = []
    # send zeros for the distance between
    for j in range(length):
        line = ""
        for i in range(128):
            line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def simple_line(width, pattern, length, interval=None):
    # Width: 0,1,2,3,4 --> 8,16,32,64,128
    # Pattern: 1,2,3 --> simple, curve, zigzag
    # Interval: if you set the interval, you can print with specific nozels that are in the interval.
    # Obviously width is getting desabled

    data = []
    if pattern == 1:  # straight line
        number_of_lines = length

        thresh = 4 * (2 ** width)
        upperbound = 64 + thresh
        lowerbound = 64 - thresh - 1

        if interval is not None:
            print(interval)
            lowerbound = interval[0]
            upperbound = interval[1]

        for j in range(number_of_lines):
            line = ""
            for i in range(128):
                if upperbound > i > lowerbound:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)  # turning data into a list of bytearrays
            print(arr_bin)
            data.append(arr_bin)  # append byte array to list of data

    elif pattern == 2:  # curved line

        number_of_lines_in_period = 100  # lines in each sin(x) period

        iter = int(length / number_of_lines_in_period)  # num of periods
        residual_lines = length % number_of_lines_in_period  # residual lines that don't make a complete period

        thresh = 4 * (2 ** width)

        upperbound = 64 + thresh
        lowerbound = 64 - thresh - 1

        if interval is not None:
            print(interval)
            lowerbound = interval[0]
            upperbound = interval[1]

        for k in range(iter):
            for j in range(number_of_lines_in_period):
                line = ""
                for i in range(128):
                    if upperbound > i > lowerbound:
                        line += '1'
                    else:
                        line += '0'

                shift = (number_of_lines_in_period / 2 *
                         (np.sin((j + 1) * (np.pi / (number_of_lines_in_period / 2)))
                          - np.sin(j * (np.pi / (number_of_lines_in_period / 2))))
                         )

                upperbound += shift / 4
                lowerbound += shift / 4

                # turning data into a list of bytearrays
                arr_bin = turn_line_to_bytearray(line)
                data.append(arr_bin)

        for j in range(residual_lines):
            line = ""
            for i in range(128):
                if upperbound > i > lowerbound:
                    line += '1'
                else:
                    line += '0'

            shift = (number_of_lines_in_period / 2 *
                     (np.sin((j + 1) * (np.pi / (number_of_lines_in_period / 2)))
                      - np.sin(j * (np.pi / (number_of_lines_in_period / 2))))
                     )

            upperbound += shift / 4
            lowerbound += shift / 4

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    elif pattern == 3:  # Zigzag
        data = []
        number_of_lines = 30

        thresh = 8 * width
        upperbound = 64 + thresh
        lowerbound = 64 - thresh

        if interval is not None:
            print(interval)
            lowerbound = interval[0]
            upperbound = interval[1]

        iter = int(length / number_of_lines)

        for k in range(iter):
            for j in range(number_of_lines):
                line = ""
                for i in range(128):
                    if upperbound > i > lowerbound:
                        line += '1'
                    else:
                        line += '0'

                if j < number_of_lines / 2:
                    upperbound += 2 * width
                    lowerbound += 2 * width
                else:
                    upperbound -= 2 * width
                    lowerbound -= 2 * width

                arr_bin = turn_line_to_bytearray(line)
                data.append(arr_bin)

    return data


def print_image(path):
    # Turning image to black and white
    originalImage = cv2.imread(path)
    grayImage = cv2.cvtColor(originalImage, cv2.COLOR_BGR2GRAY)
    (thresh, blackAndWhiteImage) = cv2.threshold(grayImage, 127, 255, cv2.THRESH_BINARY)
    image = blackAndWhiteImage

    width = (len(image[0]))
    scale_rate = (128 / width)
    width = int(image.shape[1] * scale_rate)
    height = int(image.shape[0] * scale_rate)
    dim = (width, height)

    # resize image
    resized = cv2.resize(image, dim, interpolation=cv2.INTER_AREA)

    data = []
    for row in resized:
        line = ""
        for pixel in row:
            if pixel == 255:
                line += '0'
            else:
                line += '1'

        arr = turn_line_to_bytearray(line)
        data.append(arr)
    print(resized.shape)

    return data


def stop_and_continue(blob_data, interval):
    data = []

    blob = blob_data[0]  # getting the first blob information
    x, y, w, h = blob

    lowerbound = interval[0]  # previous printing interval information
    upperbound = interval[1]
    print(lowerbound, upperbound)
    number_of_lines = x + 40

    for i in range(number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    number_of_lines = w + 110

    for j in range(number_of_lines):
        line = ""
        for i in range(128):
            line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    lowerbound = interval[0]
    upperbound = interval[1]
    print(lowerbound, upperbound)
    number_of_lines = 500

    for i in range(number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def stop_and_dont_continue(blob_data, interval):  # interval
    data = []

    blob = blob_data[0]
    x, y, w, h = blob

    lowerbound = interval[0]
    upperbound = interval[1]
    print(lowerbound, upperbound)
    number_of_lines = x + 40

    for i in range(number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def connect_to_endpoint(blob_data, interval):
    data = []

    # detect the top blob
    blob = blob_data[0]

    lowerbound = interval[0]
    upperbound = interval[1]

    x, y, w, h = blob

    y_center = int(((2 * y + h) / 2))
    printing_line_center = 128 - int((upperbound + lowerbound) / 2)
    delta_y = (printing_line_center - y_center) - 15
    number_of_lines = int(x) + distance_between_head_and_camera + 12

    dy = (delta_y / number_of_lines)
    # dy = 0
    for i in range(number_of_lines):
        line = ""
        for j in range(128):
            if upperbound > j > lowerbound:
                line += '1'
            else:
                line += '0'
        upperbound += dy
        lowerbound += dy

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    print(blob_data)
    return data


def connect_to_startpoint(blob_data):
    blob = blob_data[0]
    number_of_lines = 450

    x, y, w, h = blob

    data = reach_the_blob(int(distance_between_head_and_camera + x + w / 2))  # start from the middle of the blob

    # It starts from the blob printing with the half of the width of the blob
    lowerbound = y + int(h / 4)
    upperbound = y + int(3 * h / 4)

    for j in range(number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        line = line[::-1]  # Reverse the data array

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def connect_to_start_point_curly(blob_data):
    blob = blob_data[0]
    x, y, w, h = blob
    data = reach_the_blob(int(distance_between_head_and_camera + x + w - 2))  # start from the end of the blob
    length = 1000
    width = 2
    number_of_lines = 100

    iter = int(length / number_of_lines)  # number of periods
    residual_lines = length % number_of_lines

    thresh = 4 * (width)

    blob_center = y + int(h / 2)

    upperbound = blob_center + thresh
    lowerbound = blob_center - thresh - 1

    for k in range(iter):
        for j in range(number_of_lines):
            line = ""
            for i in range(128):
                if upperbound > i > lowerbound:
                    line += '1'
                else:
                    line += '0'

            shift = (number_of_lines / 2 *
                     (np.sin((j + 1) * (np.pi / (number_of_lines / 2)))
                      - np.sin(j * (np.pi / (number_of_lines / 2))))
                     )

            upperbound += shift / 4
            lowerbound += shift / 4

            line = line[::-1]
            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    for j in range(residual_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        shift = (number_of_lines / 2 *
                 (np.sin((j + 1) * (np.pi / (number_of_lines / 2)))
                  - np.sin(j * (np.pi / (number_of_lines / 2))))
                 )

        upperbound += shift / 4
        lowerbound += shift / 4

        line = line[::-1]
        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def continue_trace(blob_data):
    blob = blob_data[0]
    number_of_lines = 450

    x, y, w, h = blob

    data = reach_the_blob(distance_between_head_and_camera)

    lowerbound = y
    upperbound = y + h
    print(blob)
    print(lowerbound, upperbound)
    for j in range(number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'
        line = line[::-1]
        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def parallel_line(blob_data):
    blob = blob_data[0]
    number_of_lines = 400
    x, y, w, h = blob

    data = reach_the_blob(x + distance_between_head_and_camera)
    y_center = (2 * y + h) / 2
    print(y_center)

    if y_center < 64:
        y_par = y + h + 64
    else:
        y_par = y - 2 * h

    lowerbound = y_par
    upperbound = y_par + 16

    for j in range(number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'
        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def pad(interval):
    data = []

    lowerbound = interval[0]
    upperbound = interval[1]

    number_of_lines = upperbound - lowerbound

    lowerbound = int(lowerbound - number_of_lines)
    upperbound = int(upperbound + number_of_lines)

    if number_of_lines > 64:
        number_of_lines = 64

    for j in range(3 * number_of_lines):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'
        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def angle(interval, degree):
    data = []

    lowerbound = interval[0]
    upperbound = interval[1]

    number_of_lines = upperbound - lowerbound

    if degree == 45:
        upperbound_temp = lowerbound
        lowerbound_temp = lowerbound

        for j in range(number_of_lines + 5):
            line = ""
            upperbound_temp += 1
            lowerbound_temp += 1
            for i in range(128):
                if upperbound_temp > i > lowerbound_temp:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

        upperbound_temp = upperbound + 5
        lowerbound_temp = lowerbound

        for j in range(4 * number_of_lines):
            line = ""
            upperbound_temp += 1
            lowerbound_temp += 1
            for i in range(128):
                if upperbound > i > lowerbound or upperbound_temp > i > lowerbound_temp:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

        data = data[::-1]

    if degree == 90:
        for j in range(number_of_lines):
            line = ""
            for i in range(128):
                if i > lowerbound:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    if degree == 135:
        for j in range(number_of_lines):
            line = ""
            upperbound += 1
            lowerbound += 1
            for i in range(128):
                if lowerbound < i < upperbound:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    if degree == 225:
        upperbound += 5
        for j in range(4 * number_of_lines + 20):
            line = ""
            upperbound -= 1
            lowerbound -= 1
            for i in range(128):
                if lowerbound < i < upperbound:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    if degree == 270:
        for j in range(number_of_lines):
            line = ""
            for i in range(128):
                if i < upperbound:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    if degree == 315:
        upperbound_temp = upperbound
        lowerbound_temp = lowerbound
        for j in range(2 * number_of_lines):
            line = ""
            upperbound_temp -= 1
            lowerbound_temp -= 1
            for i in range(128):
                if upperbound > i > lowerbound or upperbound_temp > i > lowerbound_temp:
                    line += '1'
                else:
                    line += '0'

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

        data = data[::-1]

    return data


def route_around(blob_data, interval):
    data = simple_line(1, 1, distance_between_head_and_camera, interval)
    thresh = 24  # Distance between bottom of the line and top-middle of the blob //TODO: ?

    lowerbound = interval[0]
    upperbound = interval[1]

    x, y, w, h = blob_data[0]
    x_center = int(x - 10)
    y_center = int(y + h / 2)
    intersects = False

    top = y
    bottom = y + h
    print("blob_interval:  ", top, bottom)
    print("printing interval:  ", lowerbound, upperbound)

    if top < lowerbound < bottom or top < upperbound < bottom:
        intersects = True

    bend_up = False

    if y_center > 64:
        bend_up = True

    print(intersects)

    if intersects:

        number_of_lines = x_center + int(w / 2)

        if bend_up:
            y_end = y - thresh
            delta_y = upperbound - y_end
            dy = delta_y / number_of_lines

            for i in range(number_of_lines):
                line = ""
                for i in range(128):
                    if upperbound > i > lowerbound:
                        line += '1'
                    else:
                        line += '0'
                upperbound += dy
                lowerbound += dy

                arr_bin = turn_line_to_bytearray(line)
                data.append(arr_bin)

            dy = -dy

            for i in range(number_of_lines):
                line = ""
                for j in range(128):
                    if upperbound > j > lowerbound:
                        line += '1'
                    else:
                        line += '0'
                upperbound += dy
                lowerbound += dy

                arr_bin = turn_line_to_bytearray(line)
                data.append(arr_bin)
        else:
            y_end = y + h + 2 * thresh

            delta_y = lowerbound - y_end

            dy = delta_y / number_of_lines

            for i in range(number_of_lines):
                line = ""
                for i in range(128):
                    if upperbound > i > lowerbound:
                        line += '1'
                    else:
                        line += '0'
                upperbound += dy
                lowerbound += dy
                print(line)

                arr_bin = turn_line_to_bytearray(line)
                data.append(arr_bin)

            delta_y = -delta_y

            dy = delta_y / number_of_lines

            for i in range(number_of_lines):
                line = ""
                for i in range(128):
                    if upperbound > i > lowerbound:
                        line += '1'
                    else:
                        line += '0'
                upperbound += dy
                lowerbound += dy
                print(line)

                arr_bin = turn_line_to_bytearray(line)
                data.append(arr_bin)
    else:
        data = simple_line(2, 1, 1000, interval)

    addition_data = simple_line(2, 1, 200, (lowerbound, upperbound))

    for line in addition_data:
        data.append(line)

    return data


def LED():
    data = []

    upperbound = 64 + 18 - 30
    lowerbound = 64 - 18 - 30

    for j in range(16):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    for j in range(15):
        line = ""
        for i in range(128):
            line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    for j in range(16):
        line = ""
        for i in range(128):
            if upperbound > i > lowerbound:
                line += '1'
            else:
                line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def resistor(size=1):
    data = []

    number_of_lines = 30

    thresh = 8 * size
    upperbound = 64 + thresh
    lowerbound = 64 - thresh
    dy = 6 * size
    for k in range(7):
        for j in range(number_of_lines):
            line = ""
            for i in range(128):
                if upperbound > i > lowerbound:
                    line += '1'
                else:
                    line += '0'

            if j < number_of_lines / 4:
                upperbound += dy
                lowerbound += dy
            elif 1 / 4 * number_of_lines <= j <= 3 / 4 * number_of_lines:
                upperbound -= dy
                lowerbound -= dy
            else:
                upperbound += dy
                lowerbound += dy

            arr_bin = turn_line_to_bytearray(line)
            data.append(arr_bin)

    upperbound = 64 + thresh - 3
    lowerbound = 64 - thresh + 3

    for i in range(200):
        line = ""
        for j in range(128):
            if upperbound > j > lowerbound:
                line += '1'
            else:
                line += '0'

        arr_bin = turn_line_to_bytearray(line)
        data.append(arr_bin)

    return data


def get_interval(last_line):  # this function gives us the interval that we used to print the last line
    str = ""
    print(last_line)

    if last_line is not None:
        for i in range(16):
            bnr = bin(last_line[i]).replace('0b', '')
            x = bnr[::-1]  # this reverses an array
            while len(x) < 8:
                x += '0'
            bnr = x[::-1]
            str += bnr

        print(str)

        lowerbound = 0
        upperbound = 0

        if str[0] == '1' and str[127] == "1":
            return (0, 128)

        for i in range(len(str) - 1):
            if str[i + 1] == '1' and str[i] == '0':
                lowerbound = i
            if str[i + 1] == '0' and str[i] == '1':
                upperbound = i

        else:
            print((lowerbound, upperbound))
            return lowerbound, upperbound + 1


def turn_line_to_bytearray(line):
    print(line)
    stripped_line = line.strip()
    list = [int(line[i:i + 8], 2) for i in range(0, len(stripped_line), 8)]
    arr_bin = bytearray(list)
    return arr_bin
