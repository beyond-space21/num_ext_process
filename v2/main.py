import pullNumbers
import cv2
import time
import json
from multiprocessing import Pool

start = time.time()

# img = cv2.imread("tiles/186586/122590.png")
# pullNumbers.process(img,186586,122590)
# cv2.imshow("img",img)
# cv2.waitKey(0)

with open("INNER_CORDS.json", "r") as file:
    data = json.loads(file.read())["cords"]

p1 = []
p2 = []
p3 = []
p4 = []

for x,y in data:
    if(x%2 == 0):
        if(y%2 == 0):
            p1.append([x,y])
        else:
            p2.append([x,y])
    else:
        if(y%2 == 0):
            p3.append([x,y])
        else:
            p4.append([x,y])

print(len(p1),len(p2),len(p3),len(p4))
del data



with open("p1.json","w") as file:
    file.write(json.dumps({"data":p1}))

with open("p2.json","w") as file:
    file.write(json.dumps({"data":p1}))

with open("p3.json","w") as file:
    file.write(json.dumps({"data":p1}))

with open("p4.json","w") as file:
    file.write(json.dumps({"data":p1}))


# def process(crd):
#     x,y = crd
#     img = cv2.imread("/root/tiles/"+x+"/"+y+".png")
#     pullNumbers.process(img,x,y)





end = time.time()
print("The time of execution of above program is :", (end - start) * 10**3, "ms")
