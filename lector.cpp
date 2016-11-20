#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
int main (int argc,char *argv[])
{
	if (argc<2)
	{
		std::cout<<"Numero de argumentos invalido"<<std::endl;
		return -1;
	}
	VideoCapture video (argv[1]);
	if(!video.isOpened())  // check if we succeeded
		return -1;
	Mat edges;
	namedWindow("edges",1);
	for(;;)
	{
		Mat frame;
		video >> frame; // get a new frame from camera
		cvtColor(frame, edges, CV_BGR2GRAY);
		GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
		Canny(edges, edges, 0, 30, 3);
		imshow("edges", edges);
	}
	return 0;
}
