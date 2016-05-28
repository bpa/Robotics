#ifndef CAMERA_H
#define CAMERA_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class Camera {
public:
	Camera();
	cv::Mat getMat();
};

#endif
