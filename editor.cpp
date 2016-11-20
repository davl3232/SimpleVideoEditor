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
      //std::cout << "Realizando apertura" << std::endl;
      //std::cout << "Obteniendo elemento" << std::endl;
      //std::cout << "frame type: " << type2str(frame.type()) << std::endl;
      //std::cout << frame.size() << std::endl;

      // Apertura
      Mat element = getStructuringElement( MORPH_ELLIPSE,
                                           Size( 2 * aperture_size + 1, 2 * aperture_size + 1 ),
                                           Point( aperture_size, aperture_size ) );
      Mat erosion_mat, dilation_mat, channeled_bin;
      //std::cout << "Erosionando" << std::endl;
      erode( frame, erosion_mat, element );
      //std::cout << "Dilatando" << std::endl;
      dilate( erosion_mat, dilation_mat, element );
      //imshow("Otra ventana", dilation_mat);
      //std::cout << "Binarizando" << std::endl;
      //std::cout << "dilation_mat type: " << type2str(dilation_mat.type()) << std::endl;

      // Mostrar imagenes
      Size sz1 = dilation_mat.size();
      Mat disp(sz1.height, sz1.width + sz1.width, CV_8UC3);
      Mat left(disp, Rect(0, 0, sz1.width, sz1.height));
      Mat right(disp, Rect(sz1.width, 0, sz1.width, sz1.height));
      dilation_mat.copyTo(left);

      // Binarizacion
      //dilation_mat *= 1./255;
      Mat hsv_mat;
      cvtColor(dilation_mat, hsv_mat, COLOR_BGR2HSV);
      //std::cout << "hsv_mat type: " << type2str(hsv_mat.type()) << std::endl;
      if (max_hue < min_hue) {
        Mat channeled_bin1, channeled_bin2;
        inRange(hsv_mat, Scalar(0, min_sat, min_val), Scalar(max_hue, max_sat, max_val), channeled_bin1);
        inRange(hsv_mat, Scalar(min_hue, min_sat, min_val), Scalar(180, max_sat, max_val), channeled_bin2);
        bitwise_or(channeled_bin1, channeled_bin2, channeled_bin);
      } else {
        inRange(hsv_mat, Scalar(min_hue, min_sat, min_val), Scalar(max_hue, max_sat, max_val), channeled_bin);
      }

      //std::cout << "src_bin type: " << type2str(src_bin.type()) << std::endl;
      //std::cout << "Generando ceros" << std::endl;
      //std::cout << "channeled_bin type: " << type2str(channeled_bin.type()) << std::endl;
      /*      channeled_bin *= 255;
            cvtColor(channeled_bin, channeled_bin, COLOR_BGR2HSV);*/
      src_bin = channeled_bin;
      //channeled_bin.convertTo(src_bin, CV_8U);
      //std::cout << "src_bin type: " << type2str(channeled_bin.type()) << std::endl;

      //std::cout << channeled_bin.size() << std::endl;

      // Componentes
      dst_bin = Mat::zeros( src_bin.size(), CV_8U );
      //std::cout << "Extrayendo Componentes" << std::endl;
      ExtraerComponentes();
      Mat withGraphics;
      cvtColor(dst_bin, withGraphics, CV_GRAY2BGR);
      Point2f pos2d = trackedBall.currPixPos;
      circle(withGraphics, pos2d, trackedBall.rad, Scalar(0, 0, 255));
      line(withGraphics, pos2d + Point2f(5.0, 0.0), pos2d + Point2f(-5.0, 0.0), Scalar(0, 255, 0));
      line(withGraphics, pos2d + Point2f(0.0, 5.0), pos2d + Point2f(0.0, -5.0), Scalar(0, 255, 0));
      withGraphics.copyTo(right);
      imshow(NOMBRE_VENTANA, disp);
      //imshow(NOMBRE_VENTANA, dst_bin);
      std::cout << "Esfera:" << std::endl;
      std::cout << "   Posicion: " << trackedBall.currPos << std::endl;
      std::cout << "   Velocidad: " << trackedBall.vel << std::endl;
      std::cout << "   Radio: " << trackedBall.rad << std::endl;
      if ( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
    }
  }
  // the camera will be closed automatically upon exit
  // cap.close();
  return 0;

}

