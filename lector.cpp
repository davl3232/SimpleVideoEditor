#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
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

	VideoCapture video;
	Mat frame;

	// Intentar abrir video
	if (!video.open(argv[1]))
	{
		cout << "Video no encontrado." << endl;
		return 1;
	}
	video.set(CV_CAP_PROP_FOURCC, CV_FOURCC('H','2','6','4'));
	cout << video.get(CV_CAP_PROP_FOURCC) << endl;

	// Crear ventana
	namedWindow( NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE );
	waitKey(10);
	while (1) {
		// Extraer frame
		video >> frame;
		if ( !frame.empty() ) {
			// Mostrar imagen en ventana
			imshow(NOMBRE_VENTANA, frame);
			if ( waitKey(1) == 27 ) break;
		}
	}
	return 0;
}
