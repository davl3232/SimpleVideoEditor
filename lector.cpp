#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;

#define NOMBRE_VENTANA "Simple Video Editor"

int main (int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cout << "Numero de argumentos invalido." << std::endl;
		return 1;
	}

	VideoCapture video;
	Mat frame;

	// Intentar abrir video
	if (!video.open(argv[1]))
	{
		std::cout << "Video no encontrado." << std::endl;
		return 1;
	}

	// Crear ventana
	namedWindow( NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE );
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
