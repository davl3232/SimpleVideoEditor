#include <stdio.h>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <queue>
#include <limits>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#define NOMBRE_VENTANA "Rastreador de Esferas"

using namespace cv;
using namespace std;

typedef std::queue< Point2i > colaPix;
typedef std::vector< Point2i > region;
typedef unsigned char uchar;

Mat src_bin, dst_bin, frame;
int min_rtol = 100, min_gtol = 100, min_btol = 50, max_rtol = 255, max_gtol = 255, max_btol = 120, aperture_size = 4, max_kernel_size = 9;
int min_hue = 250, max_hue = 60;
int min_sat = 0, max_sat = 255;
int min_val = 250, max_val = 255;

void ExtraerComponentes();

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
  case CV_8U:  r = "8U"; break;
  case CV_8S:  r = "8S"; break;
  case CV_16U: r = "16U"; break;
  case CV_16S: r = "16S"; break;
  case CV_32S: r = "32S"; break;
  case CV_32F: r = "32F"; break;
  case CV_64F: r = "64F"; break;
  default:     r = "User"; break;
  }

  r += "C";
  r += (chans + '0');

  return r;
}


int main(int argc, char** argv )
{
  /*// Obtener argumentos pasados por linea de comandos
  if ( argc < 2 )
  {
    std::cerr << "Usage: " << argv[ 0 ] << " binary_image" << std::endl;
    return ( -1 );

  } // fi

  // Revisar argumentos pasados por linea de comandos
  std::cout << "-------------------------" << std::endl;
  for ( int a = 0; a < argc; a++ )
    std::cout << argv[ a ] << std::endl;
  std::cout << "-------------------------" << std::endl;
  */
  // Leer imagenes
  VideoCapture cap;
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);
  cap.set(CV_CAP_PROP_FPS, 60);
  cap.set(CV_CAP_PROP_EXPOSURE, 0.5);
  cap.set(CV_CAP_PROP_CONTRAST, 0.5);
  cap.set(CV_CAP_PROP_BRIGHTNESS, 0.5);
  //cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
  // open the default camera, use something different from 0 otherwise;
  // Check VideoCapture documentation.
  //namedWindow( "Otra ventana", CV_WINDOW_AUTOSIZE);
  namedWindow( NOMBRE_VENTANA, CV_WINDOW_AUTOSIZE );
  createTrackbar( "Kernel size:\n 2n +1", NOMBRE_VENTANA, &aperture_size, max_kernel_size);
  createTrackbar( "Min hue: ", NOMBRE_VENTANA, &min_hue, 180);
  createTrackbar( "Max hue: ", NOMBRE_VENTANA, &max_hue, 180);
  createTrackbar( "Min sat: ", NOMBRE_VENTANA, &min_sat, 255);
  createTrackbar( "Max sat: ", NOMBRE_VENTANA, &max_sat, 255);
  createTrackbar( "Min val: ", NOMBRE_VENTANA, &min_val, 255);
  createTrackbar( "Max val: ", NOMBRE_VENTANA, &max_val, 255);
  /*createTrackbar( "Min R: ", NOMBRE_VENTANA,
                        &min_rtol, 255);
  createTrackbar( "Min G: ", NOMBRE_VENTANA,
                        &min_gtol, 255);
  createTrackbar( "Min B: ", NOMBRE_VENTANA,
                        &min_btol, 255);
  createTrackbar( "Max R: ", NOMBRE_VENTANA,
                        &max_rtol, 255);
  createTrackbar( "Max G: ", NOMBRE_VENTANA,
                        &max_gtol, 255);
  createTrackbar( "Max B: ", NOMBRE_VENTANA,
                        &max_btol, 255);*/

  if (!cap.open("../shia.avi"))
    return 0;
  //sleep(1);
  for (;;) {
    cap >> frame;
    resize(frame, frame, Size(640, 360), 0, 0, INTER_CUBIC);
    if ( !frame.empty() ) {
      if ( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
    }
  }
  // the camera will be closed automatically upon exit
  // cap.close();
  return 0;

}

