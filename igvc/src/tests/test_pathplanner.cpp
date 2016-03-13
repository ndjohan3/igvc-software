#include <gtest/gtest.h>
#include <ros/ros.h>
#include <nav_msgs/Path.h>
#include <igvc_msgs/action_path.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_ros/point_cloud.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_datatypes.h>
#include <mutex>
#include <algorithm>

class TestPathPlanner : public testing::Test {

	
	// motor_sub(handle.subscribe("/motors", 1, &TestJoystickDriver::motorsCallback, this)
public:
    TestPathPlanner()
      : handle(),
		disp_path_sub(handle.subscribe("/path_display", 1, &TestPathPlanner::disp_path_callback, this)),
		act_path_sub(handle.subscribe("/path", 1, &TestPathPlanner::path_callback, this)),
		expanded_sub(handle.subscribe("/expanded", 1, &TestPathPlanner::expanded_callback, this)),
		mock_map_pub(handle.advertise<pcl::PointCloud<pcl::PointXYZ> >("/map", 1)),
		mock_pose_pub(handle.advertise<geometry_msgs::PoseStamped> ("/odom_combined", 1)),
		mock_waypoint_pub(handle.advertise<geometry_msgs::PointStamped> ("/waypoint", 1))
    {
    }
    
    void disp_path_callback(const nav_msgs::Path& msg)
    {
        
    }
    
    void path_callback(const igvc_msgs::action_pathConstPtr& msg)
    {
        
    }
    
    void expanded_callback(const pcl::PointCloud<pcl::PointXYZ>& msg)
    {
        
    }

protected:
    virtual void SetUp() {
      while(!IsNodeReady()) {
        ros::spinOnce();
      }
    }

    virtual void TearDown() {

    }

    bool IsNodeReady() {
      return disp_path_sub.getNumPublishers() > 0 && act_path_sub.getNumPublishers() > 0 && expanded_sub.getNumPublishers() > 0 && 
    		  mock_map_pub.getNumSubscribers() > 0 && mock_pose_pub.getNumSubscribers() > 0 && mock_waypoint_pub.getNumSubscribers() > 0;
    }

    ros::NodeHandle handle;
    ros::Subscriber disp_path_sub;
    ros::Subscriber act_path_sub;
    ros::Subscriber expanded_sub;
    ros::Publisher mock_map_pub;
    ros::Publisher mock_pose_pub;
    ros::Publisher mock_waypoint_pub;
};

TEST_F(TestPathPlanner, test) {
  	//map publish
    pcl::PointCloud<pcl::PointXYZ>::Ptr map_cloud (new pcl::PointCloud<pcl::PointXYZ>);
    map_cloud->points.push_back (pcl::PointXYZ(1.0,0.0,0.0));
    mock_map_pub.publish(map_cloud);
    
    // pose publish
    geometry_msgs::PoseStamped::Ptr pose (new geometry_msgs::PoseStamped);
    pose->pose.position.x = 0.0;
    pose->pose.position.y = 0.0;
    pose->pose.position.z = 0.0;
    pose->pose.orientation.x = 1.0;
    mock_pose_pub.publish(pose);
    
    
    //waypoint publish
    geometry_msgs::PointStamped::Ptr waypoint (new geometry_msgs::PointStamped);
    waypoint->point.x = 2.0;
    waypoint->point.y = 0.0;
    waypoint->point.z = 0.0;
    mock_waypoint_pub.publish(waypoint);
    
    
    const igvc_msgs::action_path::ConstPtr& path_resp = ros::topic::waitForMessage<igvc_msgs::action_path>(act_path_sub.getTopic(), ros::Duration(1));
    EXPECT_TRUE(path_resp.get() != nullptr);
    
    const pcl::PointCloud<pcl::PointXYZ>::ConstPtr& expanded_resp = ros::topic::waitForMessage<pcl::PointCloud<pcl::PointXYZ>>(expanded_sub.getTopic(), ros::Duration(1));
    EXPECT_TRUE(expanded_resp.get() != nullptr);
    
    const nav_msgs::Path::ConstPtr& disp_path_resp = ros::topic::waitForMessage<nav_msgs::Path>(disp_path_sub.getTopic(), ros::Duration(1));
    EXPECT_TRUE(disp_path_resp.get() != nullptr);
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "test_pathplanner");
  testing::InitGoogleTest(&argc, argv);

  ros::AsyncSpinner spinner(1);
  spinner.start();
  int ret = RUN_ALL_TESTS();
  spinner.stop();
  ros::shutdown();
  return ret;
}