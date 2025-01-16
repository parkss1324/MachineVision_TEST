#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main()
{
    VideoCapture cap(0);
    
    if (!cap.isOpened()) {
        cerr << "Error: Couldn't open camera." << endl;
        return -1;
    }

    // 카메라 해상도 설정 (선택 사항)
    cap.set(CAP_PROP_FRAME_WIDTH, 640);  // 가로 해상도
    cap.set(CAP_PROP_FRAME_HEIGHT, 480); // 세로 해상도

    Mat frame;

    while (true) {
        // 카메라에서 프레임을 읽어옴
        cap >> frame;

        // 프레임이 유효한지 확인
        if (frame.empty()) {
            cerr << "Error: Blank frame grabbed." << endl;
            break;
        }

        // 프레임을 화면에 출력
        imshow("Camera", frame);

        // 'q' 키를 누르면 종료
        if (waitKey(10) == 'q') {
            break;
        }
    }

    // 카메라 및 윈도우 리소스 해제
    cap.release();
    destroyAllWindows();

    return 0;
}