import cv2
import mediapipe as mp

# MediaPipe 초기화
mp_pose = mp.solutions.pose
mp_drawing = mp.solutions.drawing_utils

def main():
    # 웹캠 초기화
    cap = cv2.VideoCapture(0)

    # Pose 모듈 사용 설정
    with mp_pose.Pose(min_detection_confidence=0.5, min_tracking_confidence=0.5) as pose:
        while cap.isOpened():
            ret, frame = cap.read()
            if not ret:
                print("웹캠을 읽을 수 없습니다.")
                break

            # BGR 이미지를 RGB로 변환
            image = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
            image.flags.writeable = False

            # 자세 추정
            results = pose.process(image)

            # BGR 이미지를 다시 표시용으로 변환
            image.flags.writeable = True
            image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

            if results.pose_landmarks:
                # 랜드마크 그리기
                mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)

                # 특정 자세 확인
                if detect_specific_pose(results.pose_landmarks.landmark):
                    cv2.putText(image, "Hands Up Detected!", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                else:
                    cv2.putText(image, "Normal Pose", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)

            # 결과 출력
            cv2.imshow('Pose Detection', image)

            # 종료 조건
            if cv2.waitKey(5) & 0xFF == ord('q'):
                break

    cap.release()
    cv2.destroyAllWindows()

def detect_specific_pose(landmarks):
    """
    양손이 머리보다 위에 있는지 감지
    :param landmarks: MediaPipe에서 제공하는 자세 랜드마크
    :return: 특정 자세 여부(True/False)
    """
    # 좌표 추출 (머리와 손목)
    nose_y = landmarks[mp_pose.PoseLandmark.NOSE.value].y
    left_hand_y = landmarks[mp_pose.PoseLandmark.LEFT_WRIST.value].y
    right_hand_y = landmarks[mp_pose.PoseLandmark.RIGHT_WRIST.value].y

    # 특정 조건: 손목이 코보다 위에 위치
    if left_hand_y < nose_y and right_hand_y < nose_y:
        return True
    return False

if __name__ == "__main__":
    main()
