import cv2
import yt_dlp

# YouTube URL에서 비디오 스트림 가져오기
def get_video_stream(youtube_url):
    ydl_opts = {'format': 'best[ext=mp4]'}  # mp4 형식의 최고 화질 스트림
    with yt_dlp.YoutubeDL(ydl_opts) as ydl:
        info_dict = ydl.extract_info(youtube_url, download=False) # False = 스트림, True = 다운로드
        video_url = info_dict['url']
    return video_url

# 비디오 처리
def process_video(video_url):
    cap = cv2.VideoCapture(video_url)

    if not cap.isOpened():
        print("Error: Cannot open video stream.")
        return

    while cap.isOpened():
        ret, frame = cap.read()
        if not ret:
            print("Finished processing video.")
            break

        # 동영상 프레임 처리 (예: 화면에 표시)
        cv2.imshow("YouTube Video Stream", frame)

        # 'q'를 눌러 종료
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()

# 메인 함수
if __name__ == "__main__":
    youtube_url = "https://www.youtube.com/watch?v=rRzxEiBLQCA"  # 유튜브 동영상 URL
    video_url = get_video_stream(youtube_url)
    process_video(video_url)
