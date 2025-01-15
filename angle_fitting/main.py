import cv2
import numpy as np

# Points of the polygon
points = np.array(
    [[
        224,
        4
      ],
      [
        204,
        21
      ],
      [
        235,
        21
      ],
      [
        234,
        36
      ],
      [
        200,
        37
      ],
      [
        201,
        24
      ],
      [
        4,
        196
      ],
      [
        55,
        289
      ],
      [
        71,
        352
      ],
      [
        180,
        374
      ],
      [
        300,
        339
      ],
      [
        373,
        296
      ],
      [
        282,
        85
      ]]
    , np.int32)

#Reshape points for OpenCV (must be in a specific format)
points = points.reshape((-1, 1, 2))

# Determine the canvas size based on the max width and height
max_width = np.max(points[:, :, 0]) + 50  # Adding padding
max_height = np.max(points[:, :, 1]) + 50  # Adding padding

# Create a blank image
image = np.zeros((max_height, max_width, 3), dtype=np.uint8)

# Draw the polygon
cv2.polylines(image, [points], isClosed=True, color=(255, 255, 255), thickness=2)



points = np.array([
        [
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
    ]
], dtype=np.float32)

# Compute the minimum area rectangle
min_rect = cv2.minAreaRect(points)  # Get the rotated rectangle
rect_points = cv2.boxPoints(min_rect)  # Get the 4 corners of the rectangle
rect_points = np.int32(rect_points)  # Convert to integer

# Draw the rectangle
cv2.drawContours(image, [rect_points], 0, (0, 255, 0), 2)

# Display the image
cv2.imshow("Polygon", image)
cv2.waitKey(0)
cv2.destroyAllWindows()