#include <iostream>
#include <unistd.h>
#include <ctime>
#include <algorithm>
#include <utility>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

#define NOMBRE_VENTANA "Simple Video Editor"

typedef pair<int,int> pii;
typedef pair<bool,pii> pbii;
typedef vector<pbii> vbii;

void MouseHandler(int event, int x, int y, int flags, void* userdata);
void Exiting();
void GuardarVideo();
void alertas(string ttl,string msg, int x, int y);
// Banderas de estado de GUI/reproducción del video
bool mouseOprimido = false;
bool reproduciendo = true;
bool oprimiendoPlay = false, oprimiendoStop = false, oprimiendoGuardar = false;
bool sobrePlay = false, sobreStop = false, sobreGuardar = false;
bool detenido = false;
bool guardando = false;
bool poniendoImagen = false;

// Conteo de frames
int numFrame = 0;

// Posición del Mouse
int mouseX = 0, mouseY = 0;
vbii mousePos;

// Areas para el GUI
Rect botonPlayArea, botonStopArea, botonGuardarArea, videoArea;

// Mat para guardar el frame
Mat frame;
Mat outFrame;

// Lugar donde se dibuja el GUI
Mat canvas;

// Nombres de archivos
string videoFilename;
string imgFilename;
string saveFilename;

// Archivo de video
VideoCapture cap, cap2;
Mat img;


// Alerts
Mat alerta=Mat(200,300,CV_8UC4,Scalar(255,255,255,255));

int main (int argc, char *argv[]) {
	//std::atexit(Exiting);
	if (argc < 4) {
		cout << "Numero de argumentos invalido." << endl;
		return 1;
	}
	videoFilename = string(argv[1]);
	imgFilename = string(argv[2]);
	saveFilename = string(argv[3]) + string(videoFilename.end()-4, videoFilename.end());
	cap.open(videoFilename);
	if (!cap.isOpened()) {
		//cout << "No se pudo abrir el archivo." << endl;
		alertas("No se pudo abrir el archivo","No se pudo abrir el archivo de video.",10,50);
		exit (-1);
	}

	img = imread(imgFilename, CV_LOAD_IMAGE_COLOR);
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
	Mat botonGuardarOprimido = imread("../assets/guardar_pressed.png", CV_LOAD_IMAGE_COLOR);
	Mat botonGuardarNormal = imread("../assets/guardar_normal.png", CV_LOAD_IMAGE_COLOR);
	Mat botonGuardarHover = imread("../assets/guardar_hover.png", CV_LOAD_IMAGE_COLOR);

	// Abrir ventana con callbacks
	namedWindow(NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
	setMouseCallback(NOMBRE_VENTANA, MouseHandler);

	// Primera iteración
	bool primeraIteracion = true;
	cout << CV_VERSION << endl;

	// Inicializar mousePos con tamaño de video original
	mousePos = vbii(numFrames, pbii(false, pii(0,0)));
	
	// Calcular areas de controles y video
	videoArea = Rect(0, 0, videoW, videoH);
	botonPlayArea = Rect(-botonPlayNormal.cols/2.0 + videoW/2.0, videoH, botonPlayNormal.cols, botonPlayNormal.rows);
	botonStopArea = Rect(-botonPlayNormal.cols/2.0 + videoW/2.0 - botonStopNormal.cols*1.5, videoH, botonPlayNormal.cols, botonPlayNormal.rows);
	botonGuardarArea = Rect(-botonPlayNormal.cols/2.0 + videoW/2.0 + botonGuardarNormal.cols*1.5, videoH, botonPlayNormal.cols, botonPlayNormal.rows);

	while (true) {
		// Tomar tiempo que dura el frame
	    auto t1 = chrono::high_resolution_clock::now();
		if (detenido) {
			numFrame = -1;
			cap.open(videoFilename);
			if (!cap.isOpened()) {
				//cout << "No se pudo abrir el archivo." << endl;
				alertas("No se pudo abrir el archivo","No se pudo abrir el archivo de video.",10,50);
				exit(-1);
			}
			primeraIteracion = true;
			detenido = false;
		}

	    // Lee frame del archivo si se está reproduciendo, o es el primer frame del video
	    if (reproduciendo || primeraIteracion) {
			cap >> frame;
			if (frame.empty()) {
				cout << "Archivo terminado." << endl;
				break;
			}
			numFrame++;
			primeraIteracion = false;
	    }

	    /*
		cout << "numFrame " << numFrame << endl;
		cout << "reproduciendo " << reproduciendo << endl;
		cout << "detenido " << detenido << endl;
		cout << "poniendoImagen " << poniendoImagen << endl;
		cout << "primeraIteracion " << primeraIteracion << endl;
		*/

		outFrame = Mat(frame.rows, frame.cols, frame.type(), Scalar(0,0,0,0));

		// Canvas GUI
		canvas = Mat(frame.rows + botonPlayNormal.rows, frame.cols, frame.type(), Scalar(0,0,0,0));

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

		if (oprimiendoGuardar) {
			botonGuardarOprimido.copyTo(canvas(botonGuardarArea));
		} else if (sobreGuardar) {
			botonGuardarHover.copyTo(canvas(botonGuardarArea));
		} else {
			botonGuardarNormal.copyTo(canvas(botonGuardarArea));
		}

		// Hacer copia del frame para mostrar
		frame.copyTo(outFrame(videoArea));

		// Meter imagen en frame de video
		if (poniendoImagen) {
			// Guardar posición del mouse
			mousePos[numFrame] = pbii(true, pii(mouseX, mouseY));

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
			img2.copyTo(outFrame(Rect(imgX, imgY, img2.cols, img2.rows)));
		}

		outFrame.copyTo(canvas(videoArea));

		// Mostrar canvas
		imshow(NOMBRE_VENTANA, canvas);

	    auto t2 = chrono::high_resolution_clock::now();
	    chrono::duration<double> diff = t2 - t1;
		// Detener al oprimir ESC
		t2 = chrono::high_resolution_clock::now();
	    diff = t2 - t1;
	    if (guardando) {
	    	GuardarVideo();
	    	guardando = false;
	    }
		if (waitKey(1000.0 / fps) == 27) break;
	    //std::cout << (double)chrono::duration_cast<chrono::nanoseconds>(diff).count()/1000000 << '\n';
	}

	return 0;
}

// Guardar video con imagen insertada en archivo
void GuardarVideo() {
	//cout << "Guardando video..." << endl;
	alertas("Guardando","Guardando video...",10,50);
	int currFrame = 0;
	cap2.open(videoFilename);
	if (!cap2.isOpened()) {
		//cout << "No se pudo abrir el archivo." << endl;
		alertas("No se pudo abrir el archivo","No se pudo abrir el archivo.",10,50);
		exit(-1);
	}
	Mat saveFrame;
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

	// Archivo de guardado
	VideoWriter save(saveFilename, VideoWriter::fourcc(fourCC[0],fourCC[1],fourCC[2],fourCC[3]), fps, Size(videoW, videoH));
	while (true) {
		// Leer frame del video original
		cap2 >> saveFrame;
		if (saveFrame.empty()) {
			//cout << "Archivo guardado exitosamente." << endl;
			alertas("Guardando","Archivo guardado exitosamente!",10,50);
			break;
		}

		if (mousePos[currFrame].first) {
			Mat img2 = img;
			// Cortar imagen para insertar en video
			int imgX = mousePos[currFrame].second.first - 0.5*(img.cols);
			int imgY = mousePos[currFrame].second.second - 0.5*(img.rows);
			int roiX = max(0,min(-imgX, saveFrame.cols));
			int roiY = max(0,min(-imgY, saveFrame.rows));
			int roiW = max(0,min(saveFrame.cols - imgX - roiX, img.cols - roiX));
			int roiH = max(0,min(saveFrame.rows - imgY - roiY, img.rows - roiY));
			imgX += roiX;
			imgY += roiY;

			// Insertar imagen en video
			img2 = Mat(img,Rect(roiX, roiY, roiW, roiH));
			img2.copyTo(saveFrame(Rect(imgX, imgY, img2.cols, img2.rows)));
		}
		save << saveFrame;
		currFrame++;
	}
}

// Cuando se acabe el main
void Exiting() {
	for (int i = 0; i < mousePos.size(); ++i) {
		cout << i << ": (" << mousePos[i].first << ",(" << mousePos[i].second.first << "," << mousePos[i].second.second << "))" << endl;
	}
	cout << endl;
}

// Callback del Mouse
void MouseHandler(int event, int x, int y, int flags, void* userdata) {
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
		}
		if (botonGuardarArea.contains(Point(x, y))) {
			oprimiendoGuardar = true;
			guardando = true;
		}
		if (videoArea.contains(Point(x, y))) {
			poniendoImagen = !poniendoImagen;
		}
		sobrePlay = false;
		sobreStop = false;
		sobreGuardar = false;
	} else if (event == EVENT_LBUTTONUP) {
		mouseOprimido = false;
		oprimiendoPlay = false;
		oprimiendoStop = false;
		oprimiendoGuardar = false;
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
		if (botonGuardarArea.contains(Point(x, y))) {
			sobreGuardar = true;
		} else {
			sobreGuardar = false;
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
		if (botonGuardarArea.contains(Point(x, y))) {
			sobreGuardar = true;
		} else {
			sobreGuardar = false;
		}
	}
	if (videoArea.contains(Point(x, y))) {
		mouseX = x;
		mouseY = y;
	}
}
void alertas(string ttl,string msg, int x, int y){
	alerta=Mat(200,300,CV_8UC4,Scalar(255,255,255,255));
	namedWindow(ttl, CV_WINDOW_AUTOSIZE | CV_GUI_NORMAL);
	putText(alerta,msg,Point(x,y),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0.0,0.0,255.0,255.0),2,LINE_8,false);		
	imshow(ttl,alerta);		
	waitKey(0);
}
