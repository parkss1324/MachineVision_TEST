#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // 카메라 열기
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "cap open failed" << std::endl;
        return -1;
    }

    // 영상 크기 설정
    cap.set(CAP_PROP_FRAME_WIDTH, 320);  // 영상 가로 크기
    cap.set(CAP_PROP_FRAME_HEIGHT, 320); // 영상 세로 크기

    Mat img01, img02, img03;
    while (true) {
        // 카메라에서 세 개의 프레임을 읽어옴
        bool ret1 = cap.read(img01);
        bool ret2 = cap.read(img02);
        bool ret3 = cap.read(img03);

        if (!ret1 || !ret2 || !ret3) {
            std::cerr << "can't read cap" << std::endl;
            break;
        }

        // 그레이스케일 이미지로 변환
        Mat gray_img01, gray_img02, gray_img03;
        cvtColor(img01, gray_img01, COLOR_BGR2GRAY);
        cvtColor(img02, gray_img02, COLOR_BGR2GRAY);
        cvtColor(img03, gray_img03, COLOR_BGR2GRAY);

        // 두 이미지 간의 차이 계산
        Mat difference_01, difference_02;
        absdiff(gray_img01, gray_img02, difference_01);
        absdiff(gray_img02, gray_img03, difference_02);

        // 차이가 20 이하인 부분은 제거
        threshold(difference_01, difference_01, 20, 255, THRESH_BINARY);
        threshold(difference_02, difference_02, 20, 255, THRESH_BINARY);

        // 두 차이 이미지를 AND 연산
        Mat diff;
        bitwise_and(difference_01, difference_02, diff);

        // 차이 영역의 비트 수 계산
        int diff_cnt = countNonZero(diff);

        // 결과 영상 출력
        imshow("motion sensing", diff);

        // 차이 비트 수 출력
        std::cout << diff_cnt << std::endl;

        // ESC 키를 누르면 종료
        if (waitKey(1) == 27) {
            break;
        }
    }

    // 모든 윈도우 닫기
    destroyAllWindows();

    return 0;
}
