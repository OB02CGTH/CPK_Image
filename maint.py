from ast import arg
import cv2
import pickle
import cvzone
import numpy as np
# import writedatabase
# import multiprocessing
from time import sleep
import firebase_admin
from firebase_admin import credentials
from firebase_admin import firestore

# Use a service account
cred = credentials.Certificate('xxx.json')
firebase_admin.initialize_app(cred)

db = firestore.client()

# Video feed
cap = cv2.VideoCapture('carPark.mp4')
# cap = cv2.VideoCapture(0)

with open('CarParkPosT', 'rb') as f:
    posList = pickle.load(f)

width, height = 107, 48
rounddb = 0
dataparkbu = []

# def confb(datapark):
#      for park in datapark:
#           x, y = park
#           data = {
#                u'state': y
#           }
#           # Add a new doc in collection 'cities' with ID 'LA'
#           db.collection(u'Parking').document(x).set(data)

def checkParkingSpace(imgPro):
    spaceCounter = 0
    i = 1
    datapark = []
    global dataparkbu
    global rounddb

    for pos in posList:
        print (pos)
        x, y = pos

        imgCrop = imgPro[y:y + height, x:x + width]
        # cv2.imshow(str(x * y), imgCrop)
        count = cv2.countNonZero(imgCrop)
        color = (0, 0, 0)

        if count < 900:
            color = (0, 255, 0)
            thickness = 5
            spaceCounter += 1
        else:
            color = (0, 0, 255)
            thickness = 2

        datapark.append(("a" + f"{i:02d}", color[2]))

        cv2.rectangle(img, pos, (pos[0] + width, pos[1] + height), color, thickness)
        cvzone.putTextRect(img, ("a" + f"{i:02d}" + ": " + str(count)), (x, y + height - 3), scale=1,
                           thickness=2, offset=0, colorR=color)
        i += 1
        
    rounddb += 1
    print("M : " + str(datapark))
    print("BU : " + str(dataparkbu))

    if dataparkbu:
        if rounddb >= 1:
            i = 1
            for park, parkbu in zip(datapark, dataparkbu):
                if park[1] != parkbu[1]:
                    data = {
                        u'state': str(park[1])
                    }
                    db.collection(u'Parking').document("a" + f"{i:02d}").set(data)
                    print("DB : " + str(park[0]) + ": " + str(park[1]))
                i += 1
            dataparkbu = datapark
            rounddb = 0
    else:
        i = 1
        for park in datapark:
            data = {
                u'state': str(park[1])
            }
            db.collection(u'Parking').document("a" + f"{i:02d}").set(data)
            print("DB : " + str(park[0]) + ": " + str(park[1]))
            i += 1
        dataparkbu = datapark

    cvzone.putTextRect(img, f'Free: {spaceCounter}/{len(posList)}', (100, 50), scale=3,
                           thickness=5, offset=20, colorR=(0,200,0))
while True:

    if cap.get(cv2.CAP_PROP_POS_FRAMES) == cap.get(cv2.CAP_PROP_FRAME_COUNT):
        cap.set(cv2.CAP_PROP_POS_FRAMES, 0)
    success, img = cap.read()
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    imgBlur = cv2.GaussianBlur(imgGray, (3, 3), 1)
    imgThreshold = cv2.adaptiveThreshold(imgBlur, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                         cv2.THRESH_BINARY_INV, 25, 16)
    imgMedian = cv2.medianBlur(imgThreshold, 5)
    kernel = np.ones((3, 3), np.uint8)
    imgDilate = cv2.dilate(imgMedian, kernel, iterations=1)

    checkParkingSpace(imgDilate)
    cv2.imshow("Image", img)
    # cv2.imshow("ImageBlur", imgBlur)
    # cv2.imshow("ImageThres", imgMedian)
    cv2.waitKey(10)