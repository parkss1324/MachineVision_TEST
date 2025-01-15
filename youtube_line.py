import cv2
import yt_dlp
import numpy as np

# YouTube URL에서 비디오 스트림 가져오기
def get_video_stream(youtube_url):
    ydl_opts = {'format': 'best[ext=mp4]'}  # mp4 형식의 최고 화질 스트림
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        info_dict = ydl.extract_info(youtube_url, download=False)  # 스트림 URL 가져오기
        video_url = info_dict['url']
        fps = info_dict.get('fps', 30)  # 동영상 FPS 가져오기 (기본값 30)
    return video_url, fps

# 도로 직선 검출
def detect_road_lanes(frame):
    # 그레이스케일로 변환
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # 가우시안 블러 적용 (노이즈 제거)
    blurred = cv2.GaussianBlur(gray, (5, 5), 0)

    # Canny Edge Detection 적용
    edges = cv2.Canny(blurred, 50, 150)

    # ROI (관심 영역) 설정
    height, width = edges.shape
    mask = np.zeros_like(edges)

    # 도로에 해당하는 다각형 영역 정의
    polygon = np.array([[
        (width * 0.1, height),  # 왼쪽 하단
        (width * 0.9, height),  # 오른쪽 하단
        (width * 0.6, height * 0.6),  # 오른쪽 위
        (width * 0.4, height * 0.6)   # 왼쪽 위
    ]], dtype=np.int32)

    # 관심 영역에 대한 마스크 생성
    cv2.fillPoly(mask, polygon, 255)
    roi_edges = cv2.bitwise_and(edges, mask)

    # 허프 변환 직선 검출
    lines = cv2.HoughLinesP(
        roi_edges, 
        rho=1, 
        theta=np.pi / 180, 
        threshold=50, 
        minLineLength=100, 
        maxLineGap=50
    )

    # 도로 위 직선 그리기
    line_image = np.zeros_like(frame)
    if lines is not None:
        for line in lines:
            x1, y1, x2, y2 = line[0]
            cv2.line(line_image, (x1, y1), (x2, y2), (0, 255, 0), 5)  # 초록색 직선

    # 원본 프레임에 직선 덮어쓰기
    combined = cv2.addWeighted(frame, 0.8, line_image, 1, 1)
    return combined

# 비디오 처리
def process_video(video_url, fps):
    cap = cv2.VideoCapture(video_url)
    if not cap.isOpened():
        print("Error: Cannot open video stream.")
        return

    frame_delay = int(1000 / fps)  # 프레임 딜레이 계산 (ms 단위)

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Finished processing video.")
            break

        # 도로 직선 검출 및 시각화
        frame_with_lanes = detect_road_lanes(frame)

        # 결과 프레임을 표시
        cv2.imshow("Road Lane Detection", frame_with_lanes)

        # 'q'를 눌러 종료
        if cv2.waitKey(frame_delay) & 0xFF == ord('q'):  # FPS에 맞춘 딜레이 적용
            break

    cap.release()
    cv2.destroyAllWindows()

# 메인 함수
if __name__ == "__main__":
    youtube_url = "https://www.youtube.com/watch?v=icc0a3yAwnQ"  # 유튜브 동영상 URL
    video_url, fps = get_video_stream(youtube_url)
    fps = 30  # FPS를 강제로 30으로 설정
    print(f"Video FPS: {fps}")
    process_video(video_url, fps)
