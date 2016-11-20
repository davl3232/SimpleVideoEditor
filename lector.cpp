#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
int main (int argc,char *argv[])
{
	if (argc<2)
	{
		std::cout<<"Numero de argumentos invalido"<<std::endl;
		return 1;
	}
	VideoCapture video (argv[1]);
	
}
