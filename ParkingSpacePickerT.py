import cv2
import pickle

width, height = 107, 48

try:
    with open('CarParkPosT', 'rb') as f:
        posList = pickle.load(f)
except:
    posList = []


def mouseClick(events, x, y, flags, params):
    if events == cv2.EVENT_LBUTTONDOWN:
        posList.append((x, y))
    
        # n = 1
        # if posList == []:
        #     posList.append((x, y, "a"+n))
        # else:
        #     for i in posList:
        #         x, y, ad = i
        #         if ad != "a"+n:
        #             posList.append((x, y, 'a'+n))
        #         n += 1

    if events == cv2.EVENT_RBUTTONDOWN:
        for i, pos in enumerate(posList):
            x1, y1 = pos
            if x1 < x < x1 + width and y1 < y < y1 + height:
                posList.pop(i)

    with open('CarParkPosT', 'wb') as f:
        pickle.dump(posList, f)


while True:
    img = cv2.imread('carParkImg.png')
    for pos in posList:
        cv2.rectangle(img, pos, (pos[0] + width, pos[1] + height), (255, 0, 255), 2)

    cv2.imshow("Image", img)
    cv2.setMouseCallback("Image", mouseClick)
    cv2.waitKey(1)

    
        # n = 1
        # if posList == []:
        #     posList.append((x, y, "a"+n))
        # else:
        #     for i in posList:
        #         x, y, ad = i
        #         if ad != "a"+n:
        #             posList.append((x, y, 'a'+n))
        #         n += 1