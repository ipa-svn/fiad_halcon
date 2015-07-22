/*
 * halcon_bridge.h
 *
 *  Created on: Jul 22, 2015
 *      Author: svn
 */

#ifndef HALCON_BRIDGE_H_
#define HALCON_BRIDGE_H_

#include <sensor_msgs/Image.h>

#include "halconcpp/HalconCpp.h"


void HToSensorMsgsImage(const HalconCpp::HImage& source, sensor_msgs::Image* dest);

void SensorMsgsToHImage(const sensor_msgs::Image& source, HalconCpp::HImage* dest);


#endif /* HALCON_BRIDGE_H_ */
