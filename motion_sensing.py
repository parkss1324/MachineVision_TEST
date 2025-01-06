import cv2

cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320) # 영상 가로 크기
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 320) # 영상 세로 크기

if not cap.isOpened():
    print('cap open failed')

while True:
    ret, img01 = cap.read() # C++ = 'Mat'
    ret, img02 = cap.read()
    ret, img03 = cap.read()

    if not ret:
        print("can't read cap")
        break
    
    gray_img01 = cv2.cvtColor(img01, cv2.COLOR_BGR2GRAY) # 비교를 위한 그레이스케일 이미지 생성
    gray_img02 = cv2.cvtColor(img02, cv2.COLOR_BGR2GRAY)
    gray_img03 = cv2.cvtColor(img03, cv2.COLOR_BGR2GRAY)

    difference_01 = cv2.absdiff(gray_img01, gray_img02) # 둘의 값 차이를 비교 
    difference_02 = cv2.absdiff(gray_img02, gray_img03)

    ret, difference_01 = cv2.threshold(difference_01, 20, 255, cv2.THRESH_BINARY) # 20 이하의 값 차이 삭제
    ret, difference_02 = cv2.threshold(difference_02, 20, 255, cv2.THRESH_BINARY)

    diff = cv2.bitwise_and(difference_01, difference_02) # 값 차이가 나는 비트(영역)만 검출 
    diff_cnt = cv2.countNonZero(diff) # 차이가 발생한 비트 수 출력

    cv2.imshow("motion sensing", diff)

    print(diff_cnt)

    if cv2.waitKey(1) == 27: # ESC
        break

cv2.destroyAllWindows()