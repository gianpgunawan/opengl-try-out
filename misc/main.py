import numpy as np

N = 0.1
F = 100.0
R = 0.01
T = 0.01

MAT_PROJECTION = np.array([
    [N/R, 0.0, 0.0, 0.0],
    [0.0, N/T, 0.0, 0.0],
    [0.0, 0.0, -(F + N)/(F - N), (-2.0*(F*N))/(F - N)],
    [0.0, 0.0, -1.0, 0.0]
])


z_val_front = -4.6
z_val_back  = -5.0
w_val = 1.0

vertices = np.array([
    [-0.5, -0.5, z_val_front, w_val],
    [ 0.5, -0.5, z_val_front, w_val],
    [ 0.5,  0.5, z_val_front, w_val],
    [ 0.5,  0.5, z_val_front, w_val],
    [-0.5,  0.5, z_val_front, w_val],
    [-0.5, -0.5, z_val_front, w_val],

    [-0.5, -0.5, z_val_back,  w_val],
    [ 0.5, -0.5, z_val_back,  w_val],
    [ 0.5,  0.5, z_val_back,  w_val],
    [ 0.5,  0.5, z_val_back,  w_val],
    [-0.5,  0.5, z_val_back,  w_val],
    [-0.5, -0.5, z_val_back,  w_val],

    [-0.5,  0.5,  z_val_back, w_val],
    [-0.5,  0.5, z_val_front, w_val],
    [-0.5, -0.5, z_val_front, w_val],
    [-0.5, -0.5, z_val_front, w_val],
    [-0.5, -0.5, z_val_back,  w_val],
    [-0.5,  0.5, z_val_back,  w_val],

    [ 0.5,  0.5, z_val_back,  w_val],
    [ 0.5,  0.5, z_val_front, w_val],
    [ 0.5, -0.5, z_val_front, w_val],
    [ 0.5, -0.5, z_val_front, w_val],
    [ 0.5, -0.5, z_val_back,  w_val],
    [ 0.5,  0.5, z_val_back,  w_val],

    [-0.5, -0.5, z_val_front, w_val],
    [ 0.5, -0.5, z_val_front, w_val],
    [ 0.5, -0.5, z_val_back,  w_val],
    [ 0.5, -0.5, z_val_back,  w_val],
    [-0.5, -0.5, z_val_back,  w_val],
    [-0.5, -0.5, z_val_front, w_val],

    [-0.5,  0.5, z_val_front, w_val],
    [ 0.5,  0.5, z_val_front, w_val],
    [ 0.5,  0.5, z_val_back,  w_val],
    [ 0.5,  0.5, z_val_back,  w_val],
    [-0.5,  0.5, z_val_back,  w_val],
    [-0.5,  0.5, z_val_front, w_val]
]);

MAT_TRANSLATE = np.array([
    [1, 0, 0, 0],
    [0, 1, 0, 0],
    [0, 0, 1, -3.0],
    [0, 0, 0, 1],
])

for i in range(0, len(vertices)):
    projected = MAT_PROJECTION @ MAT_TRANSLATE @ vertices[i]
    w = projected[3]
    ndc = (1.0/w) * projected
    # print(f"projected => {projected} || NDC => {ndc}")
    print(ndc)

