#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace cv::ml;
using namespace std;

Ptr<SVM> train_hog_svm(const HOGDescriptor& hog);
void on_mouse(int event, int x, int y, int flags, void* userdata);

int main()
{
#if _DEBUG // 디버그 모드에서 에러가 발생하므로 문자열 출력 후 종료
	cout << "svmdigit.exe should be built as Relase mode!" << endl;
	return 0;
#endif

    // HOGDescritor 객체 hog 생성
	HOGDescriptor hog(Size(20, 20), Size(10, 10), Size(5, 5), Size(5, 5), 9); 
    // 20 x 20 영상에서 5 x 5 셀과 10 x 10 블록, 각 셀마다 9개의 그래디언트 방향 히스토그램 설정

	Ptr<SVM> svm = train_hog_svm(hog); // SVM 학습

	if (svm.empty()) {
		cerr << "Training failed!" << endl;
		return -1;
	}

	Mat img = Mat::zeros(400, 400, CV_8U);

	imshow("img", img);
	setMouseCallback("img", on_mouse, (void*)&img);

	while (true) {
		int c = waitKey();

		if (c == 27) {
			break;
		} else if (c == ' ') { // 
			Mat img_resize;
			resize(img, img_resize, Size(20, 20), 0, 0, INTER_AREA); // img 영상을 20 x 20 크기로 변환

			vector<float> desc;
			hog.compute(img_resize, desc); // HOG 기술자 계산

			Mat desc_mat(desc);
			int res = cvRound(svm->predict(desc_mat.t())); // 1 x 324 행렬 반환
			cout << res << endl; 

			img.setTo(0);
			imshow("img", img);
		}
	}

	return 0;
}

Ptr<SVM> train_hog_svm(const HOGDescriptor& hog)
{
	Mat digits = imread("/Users/parksungsu/Documents/cpp_opencv/digits.png", IMREAD_GRAYSCALE);

	if (digits.empty()) {
		cerr << "Image load failed!" << endl;
		return 0;
	}

	Mat train_hog, train_labels;

	for (int j = 0; j < 50; j++) { // 5000개의 필기체 숫자 영상 확인
		for (int i = 0; i < 100; i++) {
			Mat roi = digits(Rect(i * 20, j * 20, 20, 20));

			vector<float> desc;
			hog.compute(roi, desc);

			Mat desc_mat(desc);
			train_hog.push_back(desc_mat.t()); // 5000 x 324
			train_labels.push_back(j / 5); // 5000 x 1 
		}
	}

	Ptr<SVM> svm = SVM::create(); // SVM 객체 생성
	svm->setType(SVM::Types::C_SVC);
	svm->setKernel(SVM::KernelTypes::RBF);
	svm->setC(2.5);
	svm->setGamma(0.50625);
	svm->train(train_hog, ROW_SAMPLE, train_labels); // SVM 학습 진행

	return svm;
}

Point ptPrev(-1, -1);

void on_mouse(int event, int x, int y, int flags, void* userdata) // 마우스로 숫자 영상 그림
{
	Mat img = *(Mat*)userdata;

	if (event == EVENT_LBUTTONDOWN)
		ptPrev = Point(x, y);
	else if (event == EVENT_LBUTTONUP)
		ptPrev = Point(-1, -1);
	else if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		line(img, ptPrev, Point(x, y), Scalar::all(255), 40, LINE_AA, 0);
		ptPrev = Point(x, y);

		imshow("img", img);
	}
}