import cv2

def main():
    # Haar Cascade를 이용한 얼굴 탐지 모델 로드
    face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")
    
    # 웹캠 열기
    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("웹캠을 열 수 없습니다.")
        return

    while True:
        # 프레임 읽기
        ret, frame = cap.read()
        if not ret:
            break

        frame = cv2.flip(frame, 1)  # 좌우 반전 (거울 효과)

        # 이미지를 그레이스케일로 변환 (얼굴 탐지를 위해)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # 얼굴 탐지
        faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

        # 탐지된 얼굴에 모자이크 적용
        for (x, y, w, h) in faces:
            frame = apply_mosaic(frame, x, y, w, h, level=15)

        # 결과 표시
        cv2.imshow("Face Mosaic", frame)

        # 종료 조건
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):  # 'q' 키를 눌러 종료
            break

    # 리소스 해제
    cap.release()
    cv2.destroyAllWindows()

def apply_mosaic(image, x, y, w, h, level=15):
    """
    지정된 영역에 모자이크를 적용
    :param image: 원본 이미지
    :param x, y, w, h: 모자이크를 적용할 영역 좌표 및 크기
    :param level: 모자이크의 크기 (작을수록 더 세밀함)
    :return: 모자이크 처리된 이미지
    """
    # 대상 영역 추출
    roi = image[y:y+h, x:x+w]
    
    # 모자이크 적용 (축소 후 확대)
    roi = cv2.resize(roi, (w // level, h // level), interpolation=cv2.INTER_LINEAR)
    roi = cv2.resize(roi, (w, h), interpolation=cv2.INTER_NEAREST)
    
    # 원본 이미지에 모자이크 영역 덮어쓰기
    image[y:y+h, x:x+w] = roi
    return image

if __name__ == "__main__":
    main()