import pullNumbers
import cv2
import time
import json
import pickle
import numpy as np
start = time.time()

with open("INNER_CORDS.json","r") as file:
    data = json.loads(file.read())['cords']


print(data[0])

# img = cv2.imread("tiles/186586/122590.png")
# pullNumbers.process(img,186586,122590)
# cv2.imshow("img",img)
# cv2.waitKey(0)


end = time.time()
print("The time of execution of above program is :",
      (end-start) * 10**3, "ms")