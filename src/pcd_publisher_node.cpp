#include <ros/ros.h>
#include <std_msgs/String.h>
#include <laser_assembler/AssembleScans2.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/point_types.h>

typedef pcl::PointCloud<pcl::PointXYZ> PointCloud;

bool first = true;
ros::Publisher cloud_pub;
ros::Time begin_t,end_t;

void sweepCallback(const std_msgs::String::ConstPtr& msg)
{
    if(first)
    {
        begin_t = ros::Time::now();
        first = false;
    }
    else
    {
        ros::NodeHandle nh;

        ros::service::waitForService("assemble_scans2");
        ros::ServiceClient client = nh.serviceClient<laser_assembler::AssembleScans2>("assemble_scans2");
        laser_assembler::AssembleScans2 srv;
        end_t = ros::Time::now();
        srv.request.begin = begin_t;
        srv.request.end = end_t;

        if(client.call(srv))
        {
            cloud_pub.publish(srv.response.cloud);
            ROS_INFO("Publishing cloud with %lu points",srv.response.cloud.data.size());
            ROS_INFO("Swipe interval: %fs - %fs \n----------",begin_t.toSec(),end_t.toSec());
        }

        begin_t = end_t;
    }


}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "pcd_publisher");

    ros::NodeHandle nh;

    ros::Subscriber string_sub = nh.subscribe("/sweep", 1000, sweepCallback);
    cloud_pub = nh.advertise<sensor_msgs::PointCloud2> ("/point_cloud",1);

    ros::spin();

    return 0;
}
