
import cv2
import numpy as np

area_thresh = 1000

camera_width = 400
camera_height = 250

resize_rate = camera_height / 128


def findContours(img, imgContour):
    blob_detected = False
    contours, _ = cv2.findContours(img, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
    rect = []
    # print(len(contours))
    for cnt in contours:
        area = cv2.contourArea(cnt)
        # if 900 > area > 400:
        if area > area_thresh:
            peri = cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, .02 * peri, True)
            x, y, width, height = cv2.boundingRect(approx)
            rect.append((int(x / resize_rate), int(y / resize_rate), int(width / resize_rate), int(height / resize_rate)))

            # draw contour
            cv2.rectangle(imgContour, (x, y), (x + width, y + height), (0, 255, 0), 2)

            # check if we have any blobs
            if len(rect) > 0:
                blob_detected = True
            else:
                blob_detected = False

    return rect, blob_detected


def pre_process(frame):
    kernel = np.ones((5, 5), np.uint8)

    imgGray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    blur = cv2.GaussianBlur(imgGray, (5, 5), 0)
    erod = cv2.erode(blur, kernel, iterations=1)
    imgBlur = cv2.GaussianBlur(erod, (9, 9), 0)

    # imgCanny = cv2.Canny(imgBlur, 20, 20)
    imgCanny = cv2.Canny(imgBlur, 100, 180)

    imgDill = cv2.dilate(imgCanny, kernel, iterations=1)

    return imgDill


def processed_frame(frame):
    if frame is not None:
        imageContour = frame.copy()
        preprocessed = pre_process(frame)
        rect, blob = findContours(preprocessed, imgContour=imageContour)

        return rect, blob, imageContour
