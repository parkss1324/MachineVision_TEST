import cv2
import yt_dlp
import mediapipe as mp

# YouTube URL에서 비디오 스트림 가져오기
def get_video_stream(youtube_url):
    ydl_opts = {'format': 'best[ext=mp4]'}  # mp4 형식의 최고 화질 스트림
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        info_dict = ydl.extract_info(youtube_url, download=False)  # 스트림 URL 가져오기
        video_url = info_dict['url']
        fps = info_dict.get('fps', 30)  # 동영상 FPS 가져오기 (기본값 30)
    return video_url, fps

# 비디오 처리
def process_video(video_url, fps):
    cap = cv2.VideoCapture(video_url)
    if not cap.isOpened():
        print("Error: Cannot open video stream.")
        return

    # Mediapipe 얼굴 감지 초기화
    mp_face_detection = mp.solutions.face_detection
    face_detection = mp_face_detection.FaceDetection(model_selection=0, min_detection_confidence=0.5)
    mp_drawing = mp.solutions.drawing_utils

    frame_delay = int(1000 / fps)  # 프레임 딜레이 계산 (ms 단위)

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Finished processing video.")
            break

        # Mediapipe는 RGB 이미지를 사용하므로 색상 변환
        rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        results = face_detection.process(rgb_frame)

        # 얼굴 감지 결과를 프레임에 표시
        if results.detections:
            for detection in results.detections:
                mp_drawing.draw_detection(frame, detection)

        # 결과 프레임을 표시
        cv2.imshow("YouTube Video Stream with Face Detection", frame)

        # 'q'를 눌러 종료
        if cv2.waitKey(frame_delay) & 0xFF == ord('q'):  # FPS에 맞춘 딜레이 적용
            break

    cap.release()
    cv2.destroyAllWindows()

# 메인 함수
if __name__ == "__main__":
    youtube_url = "https://www.youtube.com/watch?v=gdZLi9oWNZg"  # 유튜브 동영상 URL
    video_url, fps = get_video_stream(youtube_url)
    fps = 30  # FPS를 강제로 30으로 설정
    print(f"Video FPS: {fps}")
    process_video(video_url, fps)