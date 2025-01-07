import cv2
import mediapipe as mp
import numpy as np

# Mediapipe 손 추적 모듈 초기화
mp_hands = mp.solutions.hands
mp_drawing = mp.solutions.drawing_utils
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.7)

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
    if canvas is None:
        canvas = np.zeros((h, w, 3), dtype=np.uint8)

    # Mediapipe로 손 추적
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(frame_rgb)

    if result.multi_hand_landmarks:
        for hand_landmarks in result.multi_hand_landmarks:
            # Mediapipe 손 랜드마크 그리기 (디버깅용)
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

            # 랜드마크 좌표 추출 (손가락 끝 - index finger tip)
            index_finger_tip = hand_landmarks.landmark[mp_hands.HandLandmark.INDEX_FINGER_TIP]
            x, y = int(index_finger_tip.x * w), int(index_finger_tip.y * h)

            # 손 위치에 따라 그리기
            if prev_x is not None and prev_y is not None:
                cv2.line(canvas, (prev_x, prev_y), (x, y), drawing_color, brush_thickness)

            # 현재 좌표 저장
            prev_x, prev_y = x, y

    else:
        # 손이 감지되지 않으면 이전 좌표 초기화
        prev_x, prev_y = None, None

    # 캔버스와 원본 프레임 합성
    blended_frame = cv2.addWeighted(frame, 0.5, canvas, 0.5, 0)

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
