#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace cv;
using namespace std;

// YouTube 동영상의 실제 스트림 URL을 가져오는 함수
string getYouTubeStreamUrl(const string& youtubeUrl) {
    // yt-dlp 명령어를 실행하여 스트림 URL 가져오기
    string command = "yt-dlp -f 'best[ext=mp4]' --get-url \"" + youtubeUrl + "\" 2> yt-dlp-error.log > video_url.txt";
    cout << "Executing command: " << command << endl;
    int ret = system(command.c_str());  // yt-dlp 명령어 실행

    if (ret != 0) {
        cerr << "Error: yt-dlp 명령어 실행 실패. yt-dlp-error.log를 확인하세요." << endl;
        return "";
    }

    ifstream urlFile("video_url.txt");  // 스트림 URL이 저장된 파일 열기
    string videoUrl;
    if (urlFile.is_open()) {
        getline(urlFile, videoUrl);  // 첫 번째 줄에서 URL 읽기
        urlFile.close();
    } else {
        cerr << "Error: 파일에서 비디오 URL을 읽을 수 없습니다." << endl;
        return "";
    }
    return videoUrl;  // 스트림 URL 반환
}

// 도로 차선 검출 함수
void detectLaneLines(Mat& frame) {
    Mat gray, blur, edges, mask, roi;

    // 1. 이미지를 그레이스케일로 변환
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // 2. 가우시안 블러 적용 (노이즈 제거)
    GaussianBlur(gray, blur, Size(5, 5), 0);

    // 3. Canny 에지 검출
    Canny(blur, edges, 50, 150);

    // 4. 관심 영역(ROI) 설정
    mask = Mat::zeros(edges.size(), edges.type());
    Point points[1][4] = {
        {Point(frame.cols * 0.1, frame.rows), Point(frame.cols * 0.45, frame.rows * 0.6),
         Point(frame.cols * 0.55, frame.rows * 0.6), Point(frame.cols * 0.9, frame.rows)}};
    const Point* ppt[1] = {points[0]};
    int npt[] = {4};
    fillPoly(mask, ppt, npt, 1, Scalar(255, 255, 255));  // 다각형 영역 채우기

    // 에지 이미지와 ROI 마스크의 AND 연산
    bitwise_and(edges, mask, roi);

    // 5. 허프 변환을 사용한 선 검출
    vector<Vec4i> lines;
    HoughLinesP(roi, lines, 1, CV_PI / 180, 50, 50, 10);

    // 검출된 선을 원본 프레임에 그림
    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, LINE_AA);
    }
}

// 비디오 스트림 처리 함수
void processVideo(const string& videoUrl) {
    VideoCapture cap(videoUrl);  // 비디오 스트림 열기
    if (!cap.isOpened()) {
        cerr << "Error: 비디오 스트림을 열 수 없습니다." << endl;
        return;
    }

    Mat frame;
    int fps = static_cast<int>(cap.get(CAP_PROP_FPS));  // FPS 가져오기
    if (fps == 0) fps = 30;  // 기본 FPS 설정

    int frameDelay = 1000 / fps;  // 프레임 딜레이(ms)

    while (true) {
        cap >> frame;  // 프레임 읽기
        if (frame.empty()) {
            cout << "비디오 처리가 완료되었습니다." << endl;
            break;
        }

        // 도로 차선 검출
        detectLaneLines(frame);

        // 결과를 화면에 표시
        imshow("YouTube Video Stream - Lane Detection", frame);

        // 'q' 키를 누르면 종료
        if (waitKey(frameDelay) == 'q') {
            break;
        }
    }

    cap.release();  // 비디오 스트림 해제
    destroyAllWindows();  // 모든 창 닫기
}

// 프로그램의 진입점
int main() {
    string youtubeUrl = "https://www.youtube.com/watch?v=2t1i5LYoZXo&list=PL26w7arZdurV-lsFxCVL9zd9j9zrPsM4A";
    string videoUrl = getYouTubeStreamUrl(youtubeUrl);  // YouTube 스트림 URL 가져오기

    if (!videoUrl.empty()) {
        cout << "Stream URL: " << videoUrl << endl;
        processVideo(videoUrl);  // 비디오 스트림 처리
    } else {
        cerr << "비디오 URL을 가져오는 데 실패했습니다." << endl;
    }

    return 0;
}
