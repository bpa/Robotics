#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <signal.h>
#include "camera.h"

using namespace cv;

int cannyThreshold = 90;
int accumulatorThreshold = 120;
static void signal_handler(int signal);

int main(int argc, char** argv)
{
	signal(SIGUSR1, SIG_IGN);
	signal(SIGINT, signal_handler);
	Mat src;
	Camera camera;
	for (;;) {
		src = camera.getMat();
		//GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
		std::vector<Vec3f> circles;
		HoughCircles( src, circles, CV_HOUGH_GRADIENT, 1, src.rows/8, cannyThreshold, accumulatorThreshold, 0, 0 );
		std::cout << "-----" << std::endl;
		for( size_t i = 0; i < circles.size(); i++ )
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			// circle center
			//circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
			// circle outline
			//circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
			//imwrite("cap.jpg", display);
			std::cout << cvRound(circles[i][0]) << ',' << cvRound(circles[i][1]) << " r: " << cvRound(circles[i][2]) << std::endl;
		}
	}

	return 0;
}

static void signal_handler(int signal) {
	exit(1);
}

