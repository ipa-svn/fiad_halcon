/*
 * test_main.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: svn
 */


#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "halcon_ros_if/halcon_bindings.h"
#include "halcon_ros_if/halcon_bridge.h"

#include "my_hdevoperatorimpl.h"

#include <vector>
#include <string>
#include <iostream>

#include <ros/ros.h>
#include <ros/package.h>




using namespace HalconCpp;


int main(int argc, char * argv[]){
  HTuple a(1);
  HTuple b(2);
  HTuple c(0);

  //CALL_PROCEDURE(argv[0], myprint, c);

  HTuple hv_mode("def");
  HTuple hv_result;

  try {

	  std::string path = "/home/svn/projects/fiad/src/fiad_halcon/halcon_if/hdev_engine_ros_if"; //ros::package::getPath("halcon_ros_if");
	  path += "/src/";
	  CALL_PROCEDURE(path.c_str(),
			  simple_test_procedure, hv_mode, &hv_result);
	  std::cout << "res = " << hv_result.I() << std::endl;

	  //std::cin.get();

	  CALL_PROCEDURE("/home/svn/projects/fiad/src/fiad_halcon/halcon_if/hdev_engine_ros_if/src/",
			  simple_test_procedure, hv_mode, &hv_result);
	  std::cout << "res = " << hv_result.I() << std::endl;

	  //std::cin.get();

	  // get image from file
	  HImage image;
	  HObject im_Res;
	  std::cout << "Im size before fill = " << sizeof(image) << std::endl;
	  HTuple file("/home/svn/Downloads/IMAG0231.jpg");
	  HTuple file2("/home/svn/Downloads/images.jpeg");
	  CALL_PROCEDURE("", read_image, file , &image);

	  CALL_PROCEDURE("", read_image, file2, &im_Res);
	  std::cout << "Im size after fill = " << sizeof(image) << std::endl;

	  sensor_msgs::Image ros_im;
	  HToSensorMsgsImage(image, &ros_im);

	  // disp with opencv for error check
	  if (ros_im.encoding == "bgr8")
	  {
		  std::cout << "should disp 3 rgb im " << std::endl;
		  cv::Mat rgb(ros_im.height, ros_im.width, CV_8UC3, const_cast<uchar*>(&ros_im.data[0]), ros_im.step);

		  cv::namedWindow("image", CV_WINDOW_AUTOSIZE);
		  cv::imshow("image", rgb);

		  cv::waitKey();
	  }
	  else if (ros_im.encoding == "mono8")
	  {
		  cv::Mat im(ros_im.height, ros_im.width, CV_8UC1, const_cast<uchar*>(&ros_im.data[0]), ros_im.step);

		  cv::namedWindow("image1", CV_WINDOW_AUTOSIZE);
		  cv::imshow("image1", im);

		  cv::waitKey();
	  }
	  else
		  std::cout << "Error unknown encoding" << std::endl;

	  HImage gen;
	  SensorMsgsToHImage(ros_im, &gen);

	  CALL_PROCEDURE("/home/svn/projects/fiad/src/fiad_halcon/halcon_if/hdev_engine_ros_if/src/",
	  			  abc, gen, gen, file, &hv_result, &im_Res);//, &hv_result);

	  /*
	  int n_ch1 = image.CountChannels().I();
	  std::vector<unsigned char*> channel_data_p;
	  HString p_type;
	  Hlong w;
	  Hlong h;
	  std::string encoding;
	  if (n_ch1 == 1)
	  {
		  // read pointer 1
		  void * tmp = image.GetImagePointer1(&p_type, &w, &h);
		  int step;
		  if(p_type == HString("byte"))
		  {
			  encoding = "mono8";
			  step = 1*w;
		  }
		  else if(p_type == HString("uint2"))
		  {
			  encoding = "mono16";
			  step = 2*w;
		  }
		  else
		  {
			  step = 1*w;
			  encoding = "passthrough";
			  std::cout << "Channel type " << p_type.Text() << " not supported in current implementation, add mapping to sens_msgs encoding" <<std::cout;
		  }

		  // assign data to message
		  channel_data_p.push_back((unsigned char*)tmp);

	  }
	  else if (n_ch1 == 3)
	  {
		  unsigned char * p_red;
		  unsigned char * p_green;
		  unsigned char * p_blue;

		  //get grb
		  image.GetImagePointer3((void**)&p_red, (void**)&p_green, (void**)&p_blue, &p_type, &w, &h);


	  }
	  if (n_ch1 > 3)
	  {
		  // get extra channels

	  }



	  std::cout << "Image has " << n_ch1 << " channels" << std::endl;

	  unsigned char * p_red;
	  unsigned char * p_green;
	  unsigned char * p_blue;


	  void* tmp = image.GetImagePointer1(&p_type, &w, &h);
	  std::cout << "type: " << p_type.Text() << " w, h: " << w << ", " << h << std::endl;
	  unsigned char* im_data = (unsigned char*)tmp;

	  cv::Mat rgb1(h, w, CV_8UC1, im_data);

	  cv::namedWindow("image1", CV_WINDOW_AUTOSIZE);
	  cv::imshow("image1", rgb1);

	  cv::waitKey();

	  image.GetImagePointer3((void**)&p_red, (void**)&p_green, (void**)&p_blue, &p_type, &w, &h);


	  std::cout << "type: " << p_type.Text() << " w, h: " << w << ", " << h << std::endl;
	  std::cin.get();

	  int step = 1;
	  int n_ch = 3;
	  unsigned char * im_data_comb = new unsigned char[w*h*step*n_ch];

	  // combine color channels as done in mixChanells in open cv
	  int s = w*h*step;
	  int full_st = step*n_ch;
	  int n = 0;
	  for(int k = 0; k < s; k++)
	  {
		  im_data_comb[n] = p_blue[k];
		  im_data_comb[n+1*step] = p_green[k];
		  im_data_comb[n+2*step] = p_red[k];
		  n = n+full_st;
	  }


	  cv::Mat rgb(h, w, CV_8UC3, im_data_comb);
	  //std::vector<cv::Mat> rgb_array;

	  // channel order important
	  //rgb_array.push_back(cv::Mat(h, w, CV_8UC1, p_blue).clone());
	  //rgb_array.push_back(cv::Mat(h, w, CV_8UC1, p_green).clone());
	  //rgb_array.push_back(cv::Mat(h, w, CV_8UC1, p_red).clone());
	  //cv::merge(rgb_array, rgb);

	  cv::namedWindow("image", CV_WINDOW_AUTOSIZE);
	  cv::imshow("image", rgb);

	  cv::waitKey();
*/



	/*  std::cin.get();

	  HImage back;
	  Hlong al(1);
	  back.GenImageInterleaved((void*)im_data_comb, "bgr", w, h, 1, "byte", w, h, 0, 0, -1, 0);

	  CALL_PROCEDURE("/home/svn/projects/fiad/src/fiad_halcon/halcon_if/hdev_engine_ros_if/src/",
	  			  abc, back, back, file, &hv_result, &im_Res);

*/
  }catch(HDevEngineCpp::HDevEngineException& hdev_exception) {
	  std::cout << (hdev_exception.Message()) << std::endl;
  }catch(HException& ex) {
	  std::cout <<  ex.ErrorText().Text() << std::endl;
  }

  // nicest but not working on all compilers, however fine with ubuntu 14.04 and indigo.
  /*ProcedureCall call1 = HALCON_PROCEDURE(argv[0], myadd, a,b, &c);
  call1.execute();

  ProcedureCall call2 = HALCON_PROCEDURE_SEQ(argv[0], myadd, (a)(b)(&c));
  call2.execute();

  ProcedureCall call3 = HALCON_PROCEDURE_ARRAY(argv[0], myadd, (3, (a,b,&c) ));
  call3.execute();

  CALL_PROCEDURE(argv[0], myadd, a,b, &c);
  CALL_PROCEDURE_SEQ(argv[0], myadd, (a)(b)(&c));
  CALL_PROCEDURE_ARRAY(argv[0], myadd, (3, (a,b,&c) ));

  CALL_PROCEDURE(argv[0], myprint, c);

  //ProcedureCall call4 = HALCON_PROCEDURE(argv[0], mynop);
  //call4.execute();

  CALL_PROCEDURE(argv[0], mynop);
  CALL_PROCEDURE_SEQ(argv[0], mynop, ());
  CALL_PROCEDURE_ARRAY(argv[0], mynop, (1,()) );*/
  return 0;
}


