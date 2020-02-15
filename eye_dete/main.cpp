#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <queue>

#include <stdio.h>
#include <math.h>

#include "constants.h"
#include "findEyeCenter.h"

#include <stdlib.h>
using namespace std;
using namespace cv;

/** Constants **/

double fps,t=0;
char fps_string[10];  // 用于存放帧率的字符串
/** Function Headers */
void detectAndDisplay(cv::Mat frame);
/** Global variables */
std::string main_window_name = "Capture - Face detection";
std::string face_window_name = "Capture - Face";

cv::Mat debugImage;
/***************** @function main*****************/
int main(int argc, const char** argv) 
{
	cv::Mat frame;
	cv::namedWindow(main_window_name, CV_WINDOW_NORMAL);
	cv::namedWindow(face_window_name, CV_WINDOW_NORMAL);
	cv::VideoCapture capture(0);
	if (capture.isOpened()) 
	{
		while (true) 
		{
			t = (double)cv::getTickCount();
			capture.read(frame);		
			cv::flip(frame, frame, 1);// mirror it
			frame.copyTo(debugImage);
			if (!frame.empty()) 
			{
				detectAndDisplay(frame);
			}
			else 
			{
				printf(" --(!) No captured frame -- Break!");
				break;
			}
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			fps = 1.0 / t;
			sprintf(fps_string,"%.2f",fps); 
			std::string fpsString("FPS:");
			fpsString += fps_string;									
			putText(debugImage, fpsString,cv::Point(10, 30),FONT_HERSHEY_SIMPLEX,1,cv::Scalar(0, 0, 255),2);
			imshow(main_window_name, debugImage);
			int c = cv::waitKey(1);
			if ((char)c == ' ') { break; }
			if ((char)c == 'f') { imwrite("frame.png", frame); }
		}
	}
	return 0;
}

void findEyes(cv::Mat frame_gray, cv::Rect face) 
{
	cv::Mat faceROI = frame_gray(face);
	cv::Mat debugFace = faceROI;
	if (kSmoothFaceImage) {
		double sigma = kSmoothFaceFactor * face.width;
		GaussianBlur(faceROI, faceROI, cv::Size(0, 0), sigma);
	}
	//-- Find eye regions and draw them
	int eye_region_width = face.width * (kEyePercentWidth / 100.0);
	int eye_region_height = face.width * (kEyePercentHeight / 100.0);
	int eye_region_top = face.height * (kEyePercentTop / 100.0);
	//从人脸图像中标记出眼睛区域
	cv::Rect rightEyeRegion(face.width - eye_region_width - face.width*(kEyePercentSide / 100.0),
		eye_region_top, eye_region_width, eye_region_height);//*///删除改成右眼


	/**********************关键函数，寻找眼睛瞳孔*******************/
	cv::Point rightPupil = findEyeCenter(faceROI, rightEyeRegion, "Right Eye");
	//std::cout <<"rightPupil center:"<< rightPupil << endl;
	// change eye centers to face coordinates
	rightPupil.x += rightEyeRegion.x-1;
	rightPupil.y += rightEyeRegion.y+7;

	// draw eye centers
	std::cout << rightPupil<< endl;
	circle(debugFace, rightPupil, 3,255,-1,8);

	char coor_x_string[10], coor_y_string[5];
	sprintf(coor_x_string, "%d", rightPupil.x);
	sprintf(coor_y_string, "%d", rightPupil.y);
	strcat(coor_x_string, ",");
	strcat(coor_x_string, coor_y_string);

	putText(debugFace, coor_x_string, cv::Point(5, 20), FONT_HERSHEY_SIMPLEX, 0.5, 255);
	//-- Show The Eye Corners Pic
	imshow(face_window_name, faceROI);
}
/**********************@function detectAndDisplay************************/
void detectAndDisplay(cv::Mat frame) 
{
	Mat frame_gray;
	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	//equalizeHist( frame_gray, frame_gray );
	//cv::pow(frame_gray, CV_64F, frame_gray);
	Rect rect_face(100, 180, 230, 230);
	Rect rect_righteye(240, 215, 70, 50);
	rectangle(debugImage, rect_face,Scalar(0,255,0),3);//圈出眼睛区域
	rectangle(debugImage, rect_righteye, Scalar(0, 0, 255), 3);
	findEyes(frame_gray, rect_face);//将灰度图和脸部框框传入...
									//我为什么不直接传眼部框框呢？？？
}
