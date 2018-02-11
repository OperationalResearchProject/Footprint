import cv2
import csv
import numpy as np
from numpy import genfromtxt


def csv_reader(file_obj):
    return genfromtxt(file_obj, delimiter=' ')

def normalize_array(x):
	from sklearn.preprocessing import MinMaxScaler
	scaler = MinMaxScaler()
	scaler.fit(x)
	return scaler.transform(x)

def main(csv_path,image_name):
    matrice = csv_reader(csv_path)
    # On normalise entre 0 et 1
    nm = normalize_array(matrice)
    # On multiplie par 255 pour les niveaux de gris.
    cv2.imwrite(image_name, np.multiply(nm,255))
