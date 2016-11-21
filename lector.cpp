#include <iostream>
#include <unistd.h>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#define NOMBRE_VENTANA "Simple Video Editor"

void ActualizarBarra( int, void* );
void mouseHandler(int event, int x, int y, int flags, void* userdata);

bool mouseOprimido = false;
bool reproduciendo = true;
bool oprimiendoPlay = false, oprimiendoStop = false;
bool sobrePlay = false, sobreStop = false;
bool detenido = false;
Rect botonPlayArea, botonStopArea, videoArea;
Mat canvas;
string filename;
VideoCapture cap;
int main (int argc, char *argv[]) {
	if (argc < 2) {
		cout << "Numero de argumentos invalido." << endl;
		return 1;
	}
	filename = string(argv[1]);
	cap.open(filename);
	if (!cap.isOpened()) {
		cout << "No se pudo abrir el archvio." << endl;
		exit(-1);
	}

	Mat frame;
	// Leer video a 24 fps
	double fps = cap.get(CV_CAP_PROP_FPS);
	string nombrePlay = "";
	nombrePlay += "|>";
	namedWindow(NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE);

	// Cargar GUI
	Mat botonPlayOprimido = imread("../assets/play_pressed.png", CV_LOAD_IMAGE_COLOR);
	Mat botonPlayNormal = imread("../assets/play_normal.png", CV_LOAD_IMAGE_COLOR);
	Mat botonPlayHover = imread("../assets/play_hover.png", CV_LOAD_IMAGE_COLOR);
	Mat botonPauseOprimido = imread("../assets/pause_pressed.png", CV_LOAD_IMAGE_COLOR);
	Mat botonPauseNormal = imread("../assets/pause_normal.png", CV_LOAD_IMAGE_COLOR);
	Mat botonPauseHover = imread("../assets/pause_hover.png", CV_LOAD_IMAGE_COLOR);
	Mat botonStopOprimido = imread("../assets/stop_pressed.png", CV_LOAD_IMAGE_COLOR);
	Mat botonStopNormal = imread("../assets/stop_normal.png", CV_LOAD_IMAGE_COLOR);
	Mat botonStopHover = imread("../assets/stop_hover.png", CV_LOAD_IMAGE_COLOR);

	// Abrir ventana con callbacks
	namedWindow(NOMBRE_VENTANA);
	setMouseCallback(NOMBRE_VENTANA, mouseHandler);

	bool primeraIteracion = true;
	int numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	cout << numFrames << endl;
	while (true) {
		while (true) {
		    auto t1 = chrono::high_resolution_clock::now();
		    if (reproduciendo) {
				cap >> frame;
				if (frame.empty()) {
					cout << "Archivo terminado." << endl;
					break;
				}
		    }
			if (primeraIteracion) {
				videoArea = Rect(0, 0, frame.cols, frame.rows);
				botonPlayArea = Rect(-botonPlayNormal.cols/2.0 + frame.cols/2.0, frame.rows, botonPlayNormal.cols, botonPlayNormal.rows);
				botonStopArea = Rect(-botonPlayNormal.cols/2.0 + frame.cols/2.0 - botonStopNormal.cols*1.5, frame.rows, botonPlayNormal.cols, botonPlayNormal.rows);
				primeraIteracion = false;
			}

			// Canvas GUI
			canvas = Mat(frame.rows + botonPlayNormal.rows, frame.cols, frame.type(), Scalar(0,0,0,0));

			// Debug
			cout << "reproduciendo: " << reproduciendo << endl;
			cout << "mouseOprimido: " << mouseOprimido << endl;
			cout << "oprimiendoPlay: " << oprimiendoPlay << endl;
			cout << "sobrePlay: " << sobrePlay << endl;
			cout << "oprimiendoStop: " << oprimiendoStop << endl;
			cout << "sobreStop: " << sobreStop << endl;
			
			// Dibujar GUI
			if (reproduciendo) {
				if (oprimiendoPlay) {
					botonPauseOprimido.copyTo(canvas(botonPlayArea));
				} else if (sobrePlay) {
					botonPauseHover.copyTo(canvas(botonPlayArea));
				} else {
					botonPauseNormal.copyTo(canvas(botonPlayArea));
				}
			} else {
				if (oprimiendoPlay) {
					botonPlayOprimido.copyTo(canvas(botonPlayArea));
				} else if (sobrePlay) {
					botonPlayHover.copyTo(canvas(botonPlayArea));
				} else {
					botonPlayNormal.copyTo(canvas(botonPlayArea));
				}
			}
			if (oprimiendoStop) {
				botonStopOprimido.copyTo(canvas(botonStopArea));
			} else if (sobreStop) {
				botonStopHover.copyTo(canvas(botonStopArea));
			} else {
				botonStopNormal.copyTo(canvas(botonStopArea));
			}

			// Dibujar frame video
			if (!detenido) {
				frame.copyTo(canvas(videoArea));
			}

			imshow(NOMBRE_VENTANA, canvas);

		    auto t2 = chrono::high_resolution_clock::now();
		    chrono::duration<double> diff = t2 - t1;
			// Detener al oprimir ESC
			if (waitKey(1000.0 / fps) == 27) break;
			t2 = chrono::high_resolution_clock::now();
		    diff = t2 - t1;
		    //std::cout << (double)chrono::duration_cast<chrono::nanoseconds>(diff).count()/1000000 << '\n';
		}
	}

	return 0;
}
/*
	0: mouseSuelto ->
	1: mouseOprimido
*/
// Callback del Mouse
void mouseHandler(int event, int x, int y, int flags, void* userdata) {
	if (event == EVENT_LBUTTONDOWN) {
		mouseOprimido = true;
		if (botonPlayArea.contains(Point(x, y))) {
			reproduciendo = !reproduciendo;
			detenido = false;
			oprimiendoPlay = true;
		}
		if (botonStopArea.contains(Point(x, y))) {
			oprimiendoStop = true;
			reproduciendo = false;
			detenido = true;
			cap.open(filename);
			if (!cap.isOpened()) {
				cout << "No se pudo abrir el archvio." << endl;
				exit(-1);
			}
		}
		sobrePlay = false;
		sobreStop = false;
	} else if (event == EVENT_LBUTTONUP) {
		mouseOprimido = false;
		oprimiendoPlay = false;
		oprimiendoStop = false;
		if (botonPlayArea.contains(Point(x, y))) {
			sobrePlay = true;
		} else {
			sobrePlay = false;
		}
		if (botonStopArea.contains(Point(x, y))) {
			sobreStop = true;
		} else {
			sobreStop = false;
		}
	} else {
		if (botonPlayArea.contains(Point(x, y))) {
			sobrePlay = true;
		} else {
			sobrePlay = false;
		}
		if (botonStopArea.contains(Point(x, y))) {
			sobreStop = true;
		} else {
			sobreStop = false;
		}
	}
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