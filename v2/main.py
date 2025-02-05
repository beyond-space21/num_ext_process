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


def process(crd):
    x,y = crd
    img = cv2.imread("tiles/"+x+"/"+y+".png")
    pullNumbers.process(img,x,y)


# def shd(arr):
    # ckn = int(len(arr)/20)
    # batch = []
    # temp = []
    # for i in arr:
    #     if len(temp) > ckn:
    #         batch.append(temp)
    #         temp = []
    #     else:
    #         temp.append(i)
    # batch.append(temp)

    # for i in range(len(batch)):
    #     for job in batch[i]:
    #         prs1 = multiprocessing.Process(target=process, args=(job[0],job[1]))



with Pool(processes=20) as pool:
    pool.map(process, p1, chunksize=int(len(p1)/20))


end = time.time()
print("The time of execution of above program is :", (end - start) * 10**3, "ms")
