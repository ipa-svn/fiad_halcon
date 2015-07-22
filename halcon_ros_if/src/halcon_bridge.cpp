/*
 * halcon_bridge.cpp
 *
 *  Created on: Jul 22, 2015
 *      Author: svn
 */

#include "halcon_ros_if/halcon_bridge.h"


#include <vector>
#include <string>
#include <iostream>
#include <map>


void HToSensorMsgsImage(const HalconCpp::HImage& source, sensor_msgs::Image* dest)
{
	int n_ch = source.CountChannels().I();
	std::vector<unsigned char*> channel_data_p;
	HalconCpp::HString p_type;
	Hlong w;
	Hlong h;
	std::string encoding;
	if (n_ch == 1)
	{
		// read pointer 1
		channel_data_p.push_back((unsigned char*)source.GetImagePointer1(&p_type, &w, &h));

	}
	else if (n_ch >= 3)
	{
		unsigned char * p_red;
		unsigned char * p_green;
		unsigned char * p_blue;

		//get grb
		source.GetImagePointer3((void**)&p_red, (void**)&p_green, (void**)&p_blue, &p_type, &w, &h);

		channel_data_p.push_back(p_blue);
		channel_data_p.push_back(p_green);
		channel_data_p.push_back(p_red);

		if (n_ch > 3)
		{
			std::cout << "more than 3 channels currently not supported for conversion and will be disregarded" << std::endl;
			n_ch = 3;
		}
	}


	int step;
	if(p_type == HalconCpp::HString("byte"))
	{
		if(n_ch == 1)
			dest->encoding = "mono8";
		else if(n_ch == 3)
			dest->encoding = "bgr8";
		step = 1;
	}
	else if(p_type == HalconCpp::HString("uint2"))
	{
		if(n_ch == 1)
			dest->encoding = "mono16";
		else if(n_ch == 3)
			dest->encoding = "bgr16";
		step = 2;
	}
	else
	{
		step = 1;
		dest->encoding = "passthrough";
		std::cout << "Channel type " << p_type.Text() << " not supported in current implementation, add mapping to sens_msgs encoding" <<std::cout;
	}


	std::cout << "Image has " << n_ch << " channels with type: " << p_type.Text() << " w, h: " << w << ", " << h << std::endl;

	// combine color channels as done in mixChanells in open cv
	int channel_size = w*h*step;
	int full_step = step*n_ch;

	int im_size = w*h*full_step;
	unsigned char * im_data_comb = new unsigned char[im_size];

	int n = 0;
	for(int k = 0; k < channel_size; k++)
	{
		for (int i = 0; i < channel_data_p.size(); i++)
		{
			im_data_comb[n + i*step] = channel_data_p[i][k];
		}
		n = n+full_step;
	}

	dest->width = w;
	dest->height = h;
	dest->is_bigendian = false;
	dest->step = full_step * w; // TODO add: *w;


	dest->data.resize(im_size);
	memcpy((unsigned char*)(&dest->data[0]), im_data_comb, im_size);

}

void SensorMsgsToHImage(const sensor_msgs::Image& source, HalconCpp::HImage* dest)
{
	if (source.encoding == "bgr8")
	{
		dest->GenImageInterleaved((void*)(&source.data[0]), "bgr", (Hlong) source.width, (Hlong)source.height, 1,
				"byte", (Hlong)source.width, (Hlong)source.height, 0, 0, -1, 0);
	}
	else if(source.encoding == "mono8")
	{
		dest->GenImage1("byte", (Hlong)source.width, (Hlong)source.height, (void*)(&source.data[0]));
	}
}


