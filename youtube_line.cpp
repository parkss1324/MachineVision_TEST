#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

using namespace cv;
using namespace std;

string getYouTubeStreamUrl(const string& youtubeUrl) {
    string command = "yt-dlp -f 'best[ext=mp4]' --get-url \"" + youtubeUrl + "\" 2> yt-dlp-error.log > video_url.txt";
    cout << "Executing command: " << command << endl;
    int ret = system(command.c_str());  // Execute yt-dlp command

    if (ret != 0) {
        cerr << "Error: yt-dlp command failed. Check yt-dlp-error.log for details." << endl;
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

void detectLaneLines(Mat& frame) {
    Mat gray, blur, edges, mask, roi;

    // 1. Convert to grayscale
    cvtColor(frame, gray, COLOR_BGR2GRAY);

    // 2. Apply Gaussian Blur
    GaussianBlur(gray, blur, Size(5, 5), 0);

    // 3. Canny Edge Detection
    Canny(blur, edges, 50, 150);

    // 4. Define Region of Interest (ROI)
    mask = Mat::zeros(edges.size(), edges.type());
    Point points[1][4] = {
        {Point(frame.cols * 0.1, frame.rows), Point(frame.cols * 0.45, frame.rows * 0.6),
         Point(frame.cols * 0.55, frame.rows * 0.6), Point(frame.cols * 0.9, frame.rows)}};
    const Point* ppt[1] = {points[0]};
    int npt[] = {4};
    fillPoly(mask, ppt, npt, 1, Scalar(255, 255, 255));

    // Apply mask to edges
    bitwise_and(edges, mask, roi);

    // 5. Hough Transform for Line Detection
    vector<Vec4i> lines;
    HoughLinesP(roi, lines, 1, CV_PI / 180, 50, 50, 10);

    // Draw the lines on the original frame
    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        line(frame, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, LINE_AA);
    }
}

void processVideo(const string& videoUrl) {
    VideoCapture cap(videoUrl);  // Open video stream
    if (!cap.isOpened()) {
        cerr << "Error: Cannot open video stream." << endl;
        return;
    }

    Mat frame;
    int fps = static_cast<int>(cap.get(CAP_PROP_FPS));
    if (fps == 0) fps = 30;

    int frameDelay = 1000 / fps;

    while (true) {
        cap >> frame;
        if (frame.empty()) {
            cout << "Finished processing video." << endl;
            break;
        }

        // Detect lane lines
        detectLaneLines(frame);

        // Display the result
        imshow("YouTube Video Stream - Lane Detection", frame);

        if (waitKey(frameDelay) == 'q') {
            break;
        }
    }

    cap.release();
    destroyAllWindows();
}

int main() {
    string youtubeUrl = "https://www.youtube.com/watch?v=2t1i5LYoZXo&list=PL26w7arZdurV-lsFxCVL9zd9j9zrPsM4A";
    string videoUrl = getYouTubeStreamUrl(youtubeUrl);

    if (!videoUrl.empty()) {
        cout << "Stream URL: " << videoUrl << endl;
        processVideo(videoUrl);
    } else {
        cerr << "Failed to fetch video URL." << endl;
    }

    return 0;
}
