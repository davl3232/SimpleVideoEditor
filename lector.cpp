#include <iostream>
#include <unistd.h>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#define NOMBRE_VENTANA "Simple Video Editor"

void ActualizarBarra( int, void* );
void mouseHandler(int event, int x, int y, int flags, void* userdata);

int estadoControles = 0;
bool reproduciendo = false;
bool oprimiendo = false;
Rect boton;
Mat canvas;

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
		cout << "No se pudo abrir el archvio." << endl;
		exit(-1);
	}

	Mat frame;
	double FPS = 24.0;
	string nombrePlay = "";
	nombrePlay += "|>";
	namedWindow(NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE);

	//setMouseCallback(const string& winname, MouseCallback onMouse, void* userdata=0 )

	// Leer video a 24 FPS
	while (true) {
		cap >> frame;
		if (frame.empty()) {

			cout << "Archivo terminado." << endl;
			break;
		}
		
		// Your boton
		boton = Rect(0, 0, frame.cols, 50);

		// The canvas
		canvas = Mat(frame.rows + boton.height, frame.cols, frame.type(), Scalar(0.0, 0.0, 0.0));

		// Draw the boton
		canvas(boton) = Scalar(200, 200, 200);
		canvas(boton);
		bool reproduciendo = false;
		putText(canvas, nombrePlay, Point(boton.width * 0.35, boton.height * 0.7), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
		if (oprimiendo) {
			rectangle(canvas, boton, Scalar(200, 200, 200), 2);
		} else {
			rectangle(canvas, boton, Scalar(0, 0, 255), 2);
		}

		// Draw the image
		frame.copyTo(canvas(Rect(0, boton.height, frame.cols, frame.rows)));

		// Setup callback function
		namedWindow(NOMBRE_VENTANA);
		setMouseCallback(NOMBRE_VENTANA, mouseHandler);

		imshow(NOMBRE_VENTANA, canvas);
		reproduciendo = true;

		// Detener al oprimir ESC
		if (waitKey(1000.0 / FPS) == 27) break;
	}

	return 0;
}

// Callback del Mouse
void mouseHandler(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		if (boton.contains(Point(x, y)))
		{
			cout << "Clicked!" << endl;
			oprimiendo = true;
		}
	}
	if (event == EVENT_LBUTTONUP)
	{
		oprimiendo = false;
	}

	imshow(NOMBRE_VENTANA, canvas);
	waitKey(1);
}
/*
static void onMouse( int event, int x, int y, int, void* )
{
    if( event != EVENT_LBUTTONDOWN )
        return;

    Point seed = Point(x,y);
    int lo = ffillMode == 0 ? 0 : loDiff;
    int up = ffillMode == 0 ? 0 : upDiff;
    int flags = connectivity + (newMaskVal << 8) +
                (ffillMode == 1 ? FLOODFILL_FIXED_RANGE : 0);
    int b = (unsigned)theRNG() & 255;
    int g = (unsigned)theRNG() & 255;
    int r = (unsigned)theRNG() & 255;
    Rect ccomp;

    Scalar newVal = isColor ? Scalar(b, g, r) : Scalar(r*0.299 + g*0.587 + b*0.114);
    Mat dst = isColor ? image : gray;
    int area;

    if( useMask )
    {
        threshold(mask, mask, 1, 128, THRESH_BINARY);
        area = floodFill(dst, mask, seed, newVal, &ccomp, Scalar(lo, lo, lo),
                  Scalar(up, up, up), flags);
        imshow( "mask", mask );
    }
    else
    {
        area = floodFill(dst, seed, newVal, &ccomp, Scalar(lo, lo, lo),
                  Scalar(up, up, up), flags);
    }

    imshow("image", dst);
    cout << area << " pixels were repainted\n";
}
*/