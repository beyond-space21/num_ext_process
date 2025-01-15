file = open("INNER_CORDS.json",'r')
import json
data = json.loads(file.read())
file.close()
del file

all = data['cords']

min_x=187173
max_x=0

min_y=122689
max_y=0

for i in all:
    if(i[0] < min_x):#min_x
        min_x = i[0]
    if(i[0]>max_x):#max_x
        max_x = i[0]

    if(i[1] < min_y):#min_y
        min_y = i[1]
    if(i[1]>max_y):#max_y
        max_y = i[1]


print("min_x",min_x)
print("max_x",max_x)
print()
print("min_y",min_y)
print("max_y",max_y)



# min_x 186584
# max_x 189577

# min_y 189519
# max_y 187173