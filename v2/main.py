import pullNumbers
import cv2
import time
start = time.time()

# for i in range(122587,122594):
#     pth = "tiles/186586/"+str(i)+".png"
#     print(pth)
#     img = cv2.imread(pth)

#     cv2.imshow("og",img)
#     pullNumbers.process(img)
#     cv2.imshow("processed",img)
#     cv2.waitKey(0)





img = cv2.imread("tiles/186586/122590.png")
pullNumbers.process(img,186586,122590)
# cv2.imshow("img",img)
# cv2.waitKey(0)


end = time.time()
print("The time of execution of above program is :",
      (end-start) * 10**3, "ms")