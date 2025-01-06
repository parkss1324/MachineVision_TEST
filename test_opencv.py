import cv2

img = cv2.imread('lenna.bmp')

cv2.imshow('Lenna Image', img)
cv2.waitKey(0)
cv2.destroyAllWindows()