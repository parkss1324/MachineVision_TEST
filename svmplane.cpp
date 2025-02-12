#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace cv::ml;
using namespace std;

int main(void)
{
	Mat train = Mat_<float>({ 8, 2 }, { // 8개의 점 좌표를 포함하는 train 행렬을 생성
		150, 200, 200, 250, 100, 250, 150, 300,
		350, 100, 400, 200, 400, 300, 350, 400 });
	Mat label = Mat_<int>({ 8, 1 }, { 0, 0, 0, 0, 1, 1, 1, 1 }); // 훈련 데이터 점들의 클래스를 정의한 label 행렬을 생성

	Ptr<SVM> svm = SVM::create(); // SVM 객체를 생성하여 svm에 저장
	svm->setType(SVM::C_SVC); // SVM 타입을 C_SVC로 설정
	svm->setKernel(SVM::RBF); // SVM 커널 함수를 RBF로 설정
	svm->trainAuto(train, ROW_SAMPLE, label); // 최적의 파라미터 C와 gamma를 자동으로 찾아 학습

	Mat img = Mat::zeros(Size(500, 500), CV_8UC3); // 분류 결과를 나타낼 img 영상을 생성

	for (int j = 0; j < img.rows; j++) { // 영상의 모든 픽셀 좌표에 대해 SVM 응답을 구하여 빨간색 또는 녹색으로 표현
		for (int i = 0; i < img.cols; i++) {
			Mat test = Mat_<float>({ 1, 2 }, { (float)i, (float)j });
			int res = cvRound(svm->predict(test));

			if (res == 0)
				img.at<Vec3b>(j, i) = Vec3b(128, 128, 255); // R
			else
				img.at<Vec3b>(j, i) = Vec3b(128, 255, 128); // G
		}
	}

	for (int i = 0; i < train.rows; i++) {
		int x = cvRound(train.at<float>(i, 0)); // 0번 클래스
		int y = cvRound(train.at<float>(i, 1)); // 1번 클래스
		int l = label.at<int>(i, 0);

		if (l == 0)
			circle(img, Point(x, y), 5, Scalar(0, 0, 128), -1, LINE_AA); // R
		else
			circle(img, Point(x, y), 5, Scalar(0, 128, 0), -1, LINE_AA); // G
	}

	imshow("svm", img);

	waitKey();
	return 0;
}