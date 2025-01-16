#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    // Haar Cascade 파일 로드 (절대 경로 사용)
    CascadeClassifier face_cascade;
    if (!face_cascade.load("/Users/parksungsu/Documents/cpp_opencv/haarcascade_frontalface_default.xml")) {
        cerr << "Error loading cascade file" << endl;
        return -1;
    }

    // 웹캠 연결
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error opening video capture" << endl;
        return -1;
    }

    Mat frame;
    
    while (true) {
        cap >> frame; // 프레임 읽기
        if (frame.empty()) {
            cerr << "Error: Blank frame grabbed" << endl;
            break;
        }

        // 그레이스케일 변환
        Mat gray;
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // 얼굴 검출
        vector<Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 4);

        // 검출된 얼굴에 대해 사각형 그리기
        for (size_t i = 0; i < faces.size(); i++) {
            Rect face = faces[i];
            rectangle(frame, face, Scalar(255, 0, 0), 2);
            putText(frame, "Face Detected", Point(face.x, face.y - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
        }

        // 결과 이미지 표시
        imshow("Face Detection", frame);

        // 'q' 키를 누르면 종료
        if (waitKey(1) == 113) { // ASCII 코드로 'q'
            break;
        }
    }

    // 자원 해제
    cap.release();
    destroyAllWindows();

    return 0;
}
