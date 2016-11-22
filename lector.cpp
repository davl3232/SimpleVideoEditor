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

// Banderas de estado de GUI/reproducción del video
bool mouseOprimido = false;
bool reproduciendo = true;
bool oprimiendoPlay = false, oprimiendoStop = false;
bool sobrePlay = false, sobreStop = false;
bool detenido = false;
bool poniendoImagen = false;

// Posición del Mouse
int mouseX = 0, mouseY = 0;

// Areas para el GUI
Rect botonPlayArea, botonStopArea, videoArea;

// Lugar donde se dibuja el GUI
Mat canvas;

// Nombres de archivos
string videoFilename;
string imgFilename;
string saveFilename;

// Archivo de video
VideoCapture cap;

int main (int argc, char *argv[]) {
	if (argc < 4) {
		cout << "Numero de argumentos invalido." << endl;
		return 1;
	}
	videoFilename = string(argv[1]);
	imgFilename = string(argv[2]);
	saveFilename = string(argv[3]) + string(videoFilename.end()-4, videoFilename.end());
	cap.open(videoFilename);
	if (!cap.isOpened()) {
		cout << "No se pudo abrir el archivo." << endl;
		exit(-1);
	}

	Mat img = imread(imgFilename, CV_LOAD_IMAGE_COLOR);

	Mat frame;
	Mat img2 = img;

	// Leer propiedades de video
	double fps = cap.get(CV_CAP_PROP_FPS);
	int numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	int videoW = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int videoH = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int ex = static_cast<int>(cap.get(CV_CAP_PROP_FOURCC));
	char fourCC[] = {
		static_cast<char>(ex & 0XFF),
		static_cast<char>((ex & 0XFF00) >> 8),
		static_cast<char>((ex & 0XFF0000) >> 16),
		static_cast<char>((ex & 0XFF000000) >> 24),
		0
	};
	/*
	union { int v; char c[5];} unionFourCC ;
	unionFourCC.v = ex;
	unionFourCC.c[4]='\0';
	cout << numFrames << endl;
	*/

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
	namedWindow(NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
	setMouseCallback(NOMBRE_VENTANA, mouseHandler);

	bool primeraIteracion = true;
	cout << CV_VERSION << endl;

	VideoWriter save(saveFilename, VideoWriter::fourcc(fourCC[0],fourCC[1],fourCC[2],fourCC[3]), fps, Size(videoW, videoH));
	cout << ex << endl;
	
	// Calcular areas de controles y video
	videoArea = Rect(0, 0, videoW, videoH);
	botonPlayArea = Rect(-botonPlayNormal.cols/2.0 + videoW/2.0, videoH, botonPlayNormal.cols, botonPlayNormal.rows);
	botonStopArea = Rect(-botonPlayNormal.cols/2.0 + videoW/2.0 - botonStopNormal.cols*1.5, videoH, botonPlayNormal.cols, botonPlayNormal.rows);
	Mat saveFrame;

	while (true) {
	    auto t1 = chrono::high_resolution_clock::now();
	    if (reproduciendo) {
			cap >> frame;
			if (frame.empty()) {
				cout << "Archivo terminado." << endl;
				break;
			}
	    }
		saveFrame = Mat(frame.rows, frame.cols, frame.type(), Scalar(0,0,0,255));
		//frame.copyTo(saveFrame);

		// Canvas GUI
		canvas = Mat(frame.rows + botonPlayNormal.rows, frame.cols, frame.type(), Scalar(0,0,0,255));

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

		// Cortar imagen para insertar en video
		int imgX = mouseX - 0.5*(img.cols);
		int imgY = mouseY - 0.5*(img.rows);
		int roiX = max(0,min(-imgX, frame.cols));
		int roiY = max(0,min(-imgY, frame.rows));
		int roiW = max(0,min(frame.cols - imgX - roiX, img.cols - roiX));
		int roiH = max(0,min(frame.rows - imgY - roiY, img.rows - roiY));
		imgX += roiX;
		imgY += roiY;

		img2 = Mat(img,Rect(roiX, roiY, roiW, roiH));
		frame.copyTo(saveFrame(videoArea));

		// Meter imagen en frame de video
		if (poniendoImagen) {
			img2.copyTo(saveFrame(Rect(imgX, imgY, img2.cols, img2.rows)));
		}

		// Dibujar frame video
		if (reproduciendo) {
			save << saveFrame;
		}
		saveFrame.copyTo(canvas(videoArea));

		// Mostrar canvas
		imshow(NOMBRE_VENTANA, canvas);

	    auto t2 = chrono::high_resolution_clock::now();
	    chrono::duration<double> diff = t2 - t1;
		// Detener al oprimir ESC
		t2 = chrono::high_resolution_clock::now();
	    diff = t2 - t1;
		if (waitKey(1000.0 / fps) == 27) break;
	    //std::cout << (double)chrono::duration_cast<chrono::nanoseconds>(diff).count()/1000000 << '\n';
	}

	return 0;
}
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
			cap.open(videoFilename);
			if (!cap.isOpened()) {
				cout << "No se pudo abrir el archivo." << endl;
				exit(-1);
			}
		}
		if (videoArea.contains(Point(x, y))) {
			poniendoImagen = !poniendoImagen;
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
	if (videoArea.contains(Point(x, y))) {
		mouseX = x;
		mouseY = y;
	}
}