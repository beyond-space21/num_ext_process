import json
file = open('res.json','r')
obj = json.loads(file.read())
file.close()


bound_rect = [[
      -31.552093505859375,
      114.21440124511719
    ],
    [
      243.44879150390625,
      -4.3878936767578125
    ],
    [
      377.1878662109375,
      305.71038818359375
    ],
    [
      102.18698120117188,
      424.31268310546875
    ]]




from func import compute_angle, get_pdf_box
print("pdf angle: ",compute_angle(get_pdf_box(obj)))  # obj : whole response from pdf endpoint 
print("true angle: ",compute_angle(bound_rect))  # bound_rect : bound_rect from tiles endpoint 