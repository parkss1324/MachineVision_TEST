import cv2
import pytesseract

# Tesseract 경로 설정 (설치 경로에 맞게 수정하세요)
pytesseract.pytesseract.tesseract_cmd = r"/usr/local/bin/tesseract"

# 이미지 파일 읽기
image = cv2.imread('car_plate.jpg')  # 전체 이미지를 대상으로 처리

# 이미지를 그레이스케일로 변환
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

# 블러링과 이진화 처리
blur = cv2.GaussianBlur(gray, (5, 5), 0)
_, thresh = cv2.threshold(blur, 150, 255, cv2.THRESH_BINARY)

# 윤곽선 검출
contours, _ = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

for cnt in contours:
    # 윤곽선의 사각형 경계 추출
    x, y, w, h = cv2.boundingRect(cnt)

    # 번호판의 크기로 추정되는 영역 필터링
    if 100 < w < 300 and 30 < h < 100:  # 번호판 크기 조건 (수정 가능)
        # 번호판 영역 추출
        plate = image[y:y+h, x:x+w]
        
        # OCR로 텍스트 추출
        text = pytesseract.image_to_string(plate, lang='eng')
        print("Detected Plate:", text.strip())
        
        # 이미지에 번호판 영역 표시
        cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

# 최종 결과 출력
cv2.imshow('Car Plate Detection', image)
cv2.waitKey(0)
cv2.destroyAllWindows()