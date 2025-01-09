import cv2
import mediapipe as mp
import numpy as np

# Mediapipe 손 추적 모듈 초기화

mp_hands = mp.solutions.hands 
# 손 추적 및 감지

mp_drawing = mp.solutions.drawing_utils
# 손의 랜드마크를 이미지 위에 표시, 손가락 관절과 관절 사이의 선을 그려 뼈대를 표현

hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.7)
# min_detection_confidence = 손을 처음 감지할 때의 최소 신뢰도 
# 값이 높을수록 감지 신뢰도가 증가하지만, 손을 더 감지하기 어려울 수 있음

# min_tracking_confidence = 감지된 손을 추적할 때의 최소 신뢰도
# 값이 높을수록 추적 정확도가 높아지지만, 손의 이동을 더 놓칠 수 있음

# 캔버스 설정
canvas = None
drawing_color = (255, 0, 0)  # 파란색
brush_thickness = 5
eraser_thickness = 50

# 웹캠 열기
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

print("Press 'q' to quit.")

# 이전 손가락 위치 저장 (드로잉 선 연결용)
prev_x, prev_y = None, None

while True:
    ret, frame = cap.read()
    if not ret:
        print("Error: Could not read frame.")
        break

    # 프레임 크기 확인 및 캔버스 초기화

    frame = cv2.flip(frame, 1)  # 좌우 반전 (거울 효과)
    h, w, c = frame.shape
    # 각각 높이, 너비, 채널을 의미

    if canvas is None:
        canvas = np.zeros((h, w, 3), dtype=np.uint8)
        # 3은 BGR의 컬러 공간의 채널 수를 설정
        # 각 픽셀 값이 0인 검은 화면으로 이루어진 이미지 생성

    # Mediapipe로 손 추적

    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    # OpenCV에서 읽은 이미지는 BGR 색상 공간으로 표시하지만
    # Mediapipe는 RGB 색상 공간을 사용하여 이미지를 처리하므로
    # cvtColor() 함수를 사용해 이미지 변환

    result = hands.process(frame_rgb)
    # 손 감지 및 랜드마크 추출 결과를 반환
    # 감지된 손의 왼손/오른손 여부를 포함하며 
    # 각 손은 손가락 관절을 감지한 21개의 랜드마크를 가짐

    if result.multi_hand_landmarks:
    # 손 랜드마크를 감지했는지 확인
    # multi_hand_landmarks = 손 랜드마크 데이터를 포함하는 리스트
    # 감지된 손마다 한 개의 랜드마크 세트가 포함

        for hand_landmarks in result.multi_hand_landmarks:
        # hand_landmarks = 한 손의 21개 랜드마크 좌표를 포함하는 객체

            # Mediapipe 손 랜드마크 그리기 (디버깅용)

            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # 랜드마크 좌표 추출 (손가락 끝 - index finger tip)

            index_finger_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            # INDEX_FINGER_TIP = 검지 손가락 끝

            x, y = int(index_finger_tip.x * w), int(index_finger_tip.y * h)
            # 검지 손가락의 가로와 세로 비율을 실제 이미지 크기(w, h)로 변환하여 픽셀 좌표 (x, y)계산

            # 손 위치에 따라 그리기
            if prev_x is not None and prev_y is not None:
                cv2.line(canvas, (prev_x, prev_y), (x, y), drawing_color, brush_thickness)
                # line() = OpenCV 그리기 함수
                # 이전 좌표(prev_x, prev_y)와 현재 좌표(x, y)를 잇는 선을 canvas에 그림

            # 현재 좌표 저장
            prev_x, prev_y = x, y
            # 현재 좌표(x, y)를 저장하여, 다음 반복에서 선을 그릴 때 시작 좌표로 사용

    else:
        prev_x, prev_y = None, None
        # 손이 감지되지 않으면 이전 좌표 초기화

    blended_frame = cv2.addWeighted(frame, 1, canvas, 1, 0)
    # 캔버스와 원본 프레임 합성
    # addWeighted() = 두 이미지를 원래 밝기(1)로 합성

    # 출력
    cv2.imshow("Drawing Canvas", blended_frame)

    # 종료 조건
    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):  # 'q'를 누르면 종료
        break
    elif key == ord('c'):  # 'c'를 누르면 캔버스 초기화
        canvas = np.zeros((h, w, 3), dtype=np.uint8)

# 리소스 해제
cap.release()
cv2.destroyAllWindows()
