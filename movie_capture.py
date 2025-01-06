import cv2

cap = cv2.VideoCapture(0)

# 비디오 저장을 위한 설정
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = int(cap.get(cv2.CAP_PROP_FPS)) or 30  # 기본 FPS 설정 (FPS가 0일 경우 기본값 30 사용)
fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # 동영상 코덱 (Mac에서 호환 가능한 코덱 선택)
out = cv2.VideoWriter('output.mp4', fourcc, fps, (frame_width, frame_height))

if not cap.isOpened():
    print("카메라를 열 수 없습니다.")
    exit()

print("녹화 시작")

try:
    while True:
        ret, frame = cap.read()
        if not ret:
            print("프레임을 읽을 수 없습니다. 녹화를 종료합니다.")
            break

        # 화면에 현재 프레임 표시
        cv2.imshow('Recording', frame)

        # 동영상 파일에 프레임 저장
        out.write(frame)

        # 'q' 키를 누르면 녹화 종료
        if cv2.waitKey(1) & 0xFF == ord('q'):
            print("녹화를 종료합니다.")
            break
except KeyboardInterrupt:
    print("\n녹화를 강제로 종료합니다.")

# 자원 해제
cap.release()
out.release()
cv2.destroyAllWindows()
