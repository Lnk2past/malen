import numpy as np
np.random.seed(1337)

def populate():
    return np.random.randint(0, 100, size=(2), dtype=np.dtype('i4'))

def populate2d():
    return np.random.randint(0, 100, size=(2, 3), dtype=np.dtype('i4'))

def populate3d():
    return np.random.randint(0, 100, size=(2, 3, 4), dtype=np.dtype('i4'))

def print_matrix(m):
    print(m)
