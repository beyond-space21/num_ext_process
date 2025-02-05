import pullNumbers
import cv2
import time
import json

start = time.time()

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

import concurrent.futures
def process(tile):
    x,y = tile
    img = cv2.imread("tiles/"+x+"/"+y+".png")
    pullNumbers.process(img,x,y)


with concurrent.futures.ProcessPoolExecutor(max_workers=20) as executor:
    executor.map(process, p1, chunksize = int(len(p1)/20))

with concurrent.futures.ProcessPoolExecutor(max_workers=20) as executor:
    executor.map(process, p2, chunksize = int(len(p2)/20))

with concurrent.futures.ProcessPoolExecutor(max_workers=20) as executor:
    executor.map(process, p3, chunksize = int(len(p3)/20))

with concurrent.futures.ProcessPoolExecutor(max_workers=20) as executor:
    executor.map(process, p4, chunksize = int(len(p4)/20))


end = time.time()
print("The time of execution of above program is :", (end - start) * 10**3, "ms")
