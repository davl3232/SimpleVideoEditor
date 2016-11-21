#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

#define NOMBRE_VENTANA "Simple Video Editor"

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		cout << "Numero de argumentos invalido." << endl;
		return 1;
	}
	string filename(argv[1]);
	VideoCapture cap(filename);
	if (!cap.isOpened()) {
		cout << "Unable to open the camera" << endl;
		exit(-1);
	}

	Mat image;
	double FPS = 24.0;
	// Read camera frames (at approx 24 FPS) and show the result
	while (true) {
		cap >> image;
		if (image.empty()) {
			cout << "Can't read frames from your camera" << endl;
			break;
		}

		imshow("Camera feed", image);

		// Stop the camera if users presses the "ESC" key
		if (waitKey(1000.0 / FPS) == 27) break;
	}

	return 0;
}