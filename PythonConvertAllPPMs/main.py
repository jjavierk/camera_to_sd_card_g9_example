import cv2
import numpy as np
import matplotlib.pyplot as plt
import pdb
import os

width = 640
height = 400

from os import walk
path = './adq/adq5/'
filenames = next(walk(path), (None, None, []))[2]  # [] if no file

try:
    os.mkdir(path)
except OSError as error:
    print(error)

for currentFile in filenames:
    current_file_wp = path + currentFile
    with open(current_file_wp, "rb") as rawimg:
        # Read the packed 16bits
        bayer_im0 = np.fromfile(rawimg, np.uint16, width * height)
        bayer_im = np.reshape(bayer_im0, (height, width))

        # Apply Demosacing (COLOR_BAYER_BG2BGR gives the best result out of the 4 combinations).
        bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2RGB)  # The result is BGR format with 16 bits per pixel and 12 bits range [0, 2^10-1].

        # pdb.set_trace()

        # Show image for testing (multiply by 64 because imshow requires full uint16 range [0, 2^16-1]).
        # cv2.imshow('bgr', bgr*64)
        # cv2.waitKey()
        # cv2.destroyAllWindows()
        #
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2RGB)
        # cv2.imwrite("./BG2RGB.png", bgr*64)
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_BG2BGR)
        # cv2.imwrite("./BG2BGR.png", bgr*64)
        #
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GB2RGB)
        # cv2.imwrite("./GB2RGB.png", bgr*64)
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GB2BGR)
        # cv2.imwrite("./GB2BGR.png", bgr*64)
        #
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GR2RGB)
        # cv2.imwrite("./GR2RGB.png", bgr*64)
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_GR2BGR)
        # cv2.imwrite("./GR2BGR.png", bgr*64)

        #
        # outputFileName_t = currentFile.split('.')
        # outputFileName = path + '/out/' + outputFileName_t[0] + 'RG2RGB.png'
        # bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_RG2RGB)
        # cv2.imwrite(outputFileName, bgr*64)
        outputFileName_t = currentFile.split('.')
        outputFileName = path + '/out/' + outputFileName_t[0] + 'RG2BGR.png'
        bgr = cv2.cvtColor(bayer_im, cv2.COLOR_BAYER_RG2BGR)
        cv2.imwrite(outputFileName, bgr*64)
