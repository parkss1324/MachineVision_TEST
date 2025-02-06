#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace cv::ml;
using namespace std;

Ptr<KNearest> train_knn();
void on_mouse(int event, int x, int y, int flags, void* userdata);

int main()
{
	Ptr<KNearest> knn = train_knn();

	if (knn.empty()) {
		cerr << "Training failed!" << endl;
		return -1;
	}

	Mat img = Mat::zeros(400, 400, CV_8U);

	imshow("img", img);
	setMouseCallback("img", on_mouse, (void*)&img);

	while (true) {
		int c = waitKey(0);
		
		if (c == 27) {
			break;
		} else if (c == ' ') {
			Mat img_resize, img_float, img_flatten, res;

			resize(img, img_resize, Size(20, 20), 0, 0, INTER_AREA);
			img_resize.convertTo(img_float, CV_32F);
			img_flatten = img_float.reshape(1, 1);

			knn->findNearest(img_flatten, 3, res);
			cout << cvRound(res.at<float>(0, 0)) << endl;

			img.setTo(0);
			imshow("img", img);
		} 
	}

	return 0;
}

Ptr<KNearest> train_knn()
{
	Mat digits = imread("/Users/parksungsu/Documents/cpp_opencv/digits.png", IMREAD_GRAYSCALE);
	// 경로에 맞게 digits.png 파일 영상을 저장

	if (digits.empty()) {
		cerr << "Image load failed!" << endl;
		return 0;
	}

	Mat train_images, train_labels;

	// 가로 100개, 세로 50개의 필기체 숫자를 for 문을 통해 설정
	for (int j = 0; j < 50; j++) {
		for (int i = 0; i < 100; i++) {
			Mat roi, roi_float, roi_flatten;
			roi = digits(Rect(i * 20, j * 20, 20, 20)); // 가로 i, 세로 j 번째 필기체 숫자 영상을 roi에 저장
			roi.convertTo(roi_float, CV_32F); // 영상 자료형을 float로 변환하여 roi_float에 저장
			roi_flatten = roi_float.reshape(1, 1); // 20 x 20 roi_float 영상을 400 x 1 크기의 영상으로 변환하여 roi_floatten에 저장

			train_images.push_back(roi_flatten); // roi_flatten 영상을 train_images 행렬의 맨 아래 행으로 추가
			train_labels.push_back(j / 5); // train_labels 행렬의 맨 아래에 현재 추가한 필기체 숫자 영상의 정답(레이블)을 추가
		}
	}

	Ptr<KNearest> knn = KNearest::create(); // knn 객체를 생성하고, kNN 학습을 수행
	knn->train(train_images, ROW_SAMPLE, train_labels); 
	// train_images의 행렬 크기는 5000 x 400, train_labels의 행렬 크기는 5000 x 1

	return knn; // 학습된 knn 스마트 포인터를 반환
}

Point ptPrev(-1, -1);

void on_mouse(int event, int x, int y, int flags, void* userdata)
{
	Mat img = *(Mat*)userdata;

	if (event == EVENT_LBUTTONDOWN) {
		ptPrev = Point(x, y);
	} else if (event == EVENT_LBUTTONUP) {
		ptPrev = Point(-1, -1);
	} else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON)) {
		line(img, ptPrev, Point(x, y), Scalar::all(255), 40, LINE_AA, 0);
		ptPrev = Point(x, y);

		imshow("img", img);
	}
}