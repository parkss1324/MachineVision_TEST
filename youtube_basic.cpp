#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace cv;
using namespace std;

string getYouTubeStreamUrl(const string& youtubeUrl) {
    string command = "yt-dlp -f 'best[ext=mp4]' --get-url " + youtubeUrl + " > video_url.txt";
    cout << "Executing command: " << command << endl;
    int ret = system(command.c_str());  // yt-dlp 실행

    if (ret != 0) {
        cerr << "Error: yt-dlp command failed." << endl;
        return "";
    }

    ifstream urlFile("video_url.txt");
    string videoUrl;
    if (urlFile.is_open()) {
        getline(urlFile, videoUrl);
        urlFile.close();
    } else {
        cerr << "Error: Cannot read video URL from file." << endl;
        return "";
    }
    return videoUrl;
}

void processVideo(const string& videoUrl) {
    VideoCapture cap(videoUrl);  // 스트림 URL로 비디오 열기
    if (!cap.isOpened()) {
        cerr << "Error: Cannot open video stream." << endl;
        return;
    }

    Mat frame;
    int fps = static_cast<int>(cap.get(CAP_PROP_FPS));  // FPS 가져오기
    if (fps == 0) fps = 30;  // 기본 FPS

    int frameDelay = 1000 / fps;  // 딜레이 계산 (ms)

    while (true) {
        cap >> frame;  // 프레임 읽기
        if (frame.empty()) {
            cout << "Finished processing video." << endl;
            break;
        }

        // 프레임을 화면에 표시
        imshow("YouTube Video Stream", frame);

        // 'q' 키를 눌러 종료
        if (waitKey(frameDelay) == 'q') {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
}

int main() {
    string youtubeUrl = "https://www.youtube.com/watch?v=gdZLi9oWNZg";
    string videoUrl = getYouTubeStreamUrl(youtubeUrl);  // yt-dlp로 실제 스트림 URL 추출

    if (!videoUrl.empty()) {
        cout << "Stream URL: " << videoUrl << endl;
        processVideo(videoUrl);  // 실제 스트림 URL 처리
    } else {
        cerr << "Failed to fetch video URL." << endl;
    }

    return 0;
}
