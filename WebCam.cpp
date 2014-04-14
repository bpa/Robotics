#include "camera.h"
#include "opencv2/highgui/highgui_c.h"

using namespace cv;
static VideoCapture cap(0);

Camera::Camera() {
}

Mat Camera::getMat() {
	Mat frame;
	cap >> frame;
	return frame;
}
