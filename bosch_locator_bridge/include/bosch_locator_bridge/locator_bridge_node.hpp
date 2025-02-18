// Copyright (c) 2021 - for information on the respective copyright owner
// see the NOTICE file and/or the repository https://github.com/boschglobal/locator_ros_bridge.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <unordered_map>

#include <Poco/Thread.h>

#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <sensor_msgs/LaserScan.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/PoseWithCovarianceStamped.h>

#include "bosch_locator_bridge/StartRecording.h"
#include "bosch_locator_bridge/ClientMapStart.h"
#include "bosch_locator_bridge/ClientMapSend.h"
#include "bosch_locator_bridge/ClientMapSet.h"
#include "bosch_locator_bridge/ClientMapList.h"

// forward declarations
class LocatorRPCInterface;
class SendingInterface;
class ClientControlModeInterface;
class ClientMapMapInterface;
class ClientMapVisualizationInterface;
class ClientRecordingMapInterface;
class ClientRecordingVisualizationInterface;
class ClientLocalizationMapInterface;
class ClientLocalizationVisualizationInterface;
class ClientLocalizationPoseInterface;
class ClientGlobalAlignVisualizationInterface;

/**
 * This is the main ROS node. It binds together the ROS interface and the Locator API.
 */
class LocatorBridgeNode
{
public:
  LocatorBridgeNode();
  ~LocatorBridgeNode();

  void init();

private:
  bool check_module_versions(const std::unordered_map<std::string, std::pair<int32_t, int32_t>>& module_versions);

  void laser_callback(const sensor_msgs::LaserScan& msg);
  void odom_callback(const nav_msgs::Odometry& msg);

  bool clientMapSendCb(bosch_locator_bridge::ClientMapSend::Request& req,
                       bosch_locator_bridge::ClientMapSend::Response& res);
  bool clientMapSetCb(bosch_locator_bridge::ClientMapSet::Request& req,
                      bosch_locator_bridge::ClientMapSet::Response& res);
  bool clientMapList(bosch_locator_bridge::ClientMapList::Request& req,
                     bosch_locator_bridge::ClientMapList::Response& res);

  bool clientLocalizationStartCb(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res);
  bool clientLocalizationStopCb(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res);

  void setSeedCallback(const geometry_msgs::PoseWithCovarianceStamped& msg);

  bool clientRecordingStartVisualRecordingCb(bosch_locator_bridge::StartRecording::Request& req,
                                             bosch_locator_bridge::StartRecording::Response& res);
  bool clientRecordingStopVisualRecordingCb(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res);

  bool clientMapStartCb(bosch_locator_bridge::ClientMapStart::Request& req,
                        bosch_locator_bridge::ClientMapStart::Response& res);
  bool clientMapStopCb(std_srvs::Empty::Request& req, std_srvs::Empty::Response& res);

  /// read out ROS parameters and use them to update the locator config
  void syncConfig();

  void setupBinaryReceiverInterfaces(const std::string& host);

  ros::NodeHandle nh_;
  std::unique_ptr<LocatorRPCInterface> loc_client_interface_;

  ros::Timer session_refresh_timer_;

  std::vector<ros::ServiceServer> services_;

  // Flag to indicate if the bridge should send odometry data to the locator. Value retrieved by the locator settings.
  bool provide_odometry_data_;
  ros::Subscriber laser_sub_;
  std::unique_ptr<SendingInterface> laser_sending_interface_;
  Poco::Thread laser_sending_interface_thread_;

  ros::Subscriber set_seed_sub_;

  // Flag to indicate if the bridge should send odometry data to the locator. Value retrieved by the locator settings.
  bool provide_laser_data_;
  ros::Subscriber odom_sub_;
  std::unique_ptr<SendingInterface> odom_sending_interface_;
  Poco::Thread odom_sending_interface_thread_;

  //! Binary interfaces and according threads
  std::unique_ptr<ClientControlModeInterface> client_control_mode_interface_;
  Poco::Thread client_control_mode_interface_thread_;
  std::unique_ptr<ClientMapMapInterface> client_map_map_interface_;
  Poco::Thread client_map_map_interface_thread_;
  std::unique_ptr<ClientMapVisualizationInterface> client_map_visualization_interface_;
  Poco::Thread client_map_visualization_interface_thread_;
  std::unique_ptr<ClientRecordingMapInterface> client_recording_map_interface_;
  Poco::Thread client_recording_map_interface_thread_;
  std::unique_ptr<ClientRecordingVisualizationInterface> client_recording_visualization_interface_;
  Poco::Thread client_recording_visualization_interface_thread_;
  std::unique_ptr<ClientLocalizationMapInterface> client_localization_map_interface_;
  Poco::Thread client_localization_map_interface_thread_;
  std::unique_ptr<ClientLocalizationVisualizationInterface> client_localization_visualization_interface_;
  Poco::Thread client_localization_visualization_interface_thread_;
  std::unique_ptr<ClientLocalizationPoseInterface> client_localization_pose_interface_;
  Poco::Thread client_localization_pose_interface_thread_;
  std::unique_ptr<ClientGlobalAlignVisualizationInterface> client_global_align_visualization_interface_;
  Poco::Thread client_global_align_visualization_interface_thread_;

  size_t scan_num_{ 0 };
  size_t odom_num_{ 0 };

  std::string last_recording_name_;
  std::string last_map_name_;
};
