import numpy as np


target = np.array([0, 0, 0])
pos = np.array([0.0, 0.0, 3.0])

direction = pos - target
direction = direction / np.linalg.norm(direction);

up_ws = np.array([0, 1, 0])
right = np.cross(up_ws, direction)
right = right / np.linalg.norm(right)
up = np.cross(direction, right)

print(direction)
print(right)
print(up)
