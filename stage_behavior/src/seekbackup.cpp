#include <ros/ros.h>
#include <tf/transform_datatypes.h>
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/PoseStamped.h" 
#include <nav_msgs/Path.h>
#include <tf/transform_listener.h>
#include <tf/tf.h>
#include <math.h>

#define GOAL_TOPIC "move_base_simple/goal"
#define PUBLISHER_TOPIC "cmd_vel0"
#define NODE_NAME "seek_behaviour"
#define PLAN_FRAME   "odom"
#define ROBOT_FRAME  "base_footprint"

using namespace std;

enum State {
    IDLE,
    PREPROCESS,
    DRIVING,
    ARRIVED,
};

class SeekBehaviour {
public:
    // members

    // methods
    SeekBehaviour();
    geometry_msgs::PoseStamped getRobotPose();
    void publish_cmd(geometry_msgs::Twist twist);
    bool checkObstacle();
    void process();



protected:
    // members
    ros::NodeHandle nh;
    ros::Subscriber goal_sub;
    ros::Publisher vel_pub;
    geometry_msgs::PoseStamped goal;
    State currentState; // state of seeking procedure
    tf::TransformListener *listener;
    tf::StampedTransform transform;


    void on_Goal_Received(geometry_msgs::PoseStamped msg);

};

SeekBehaviour::SeekBehaviour() {
    listener = new tf::TransformListener();
    currentState = IDLE;
    vel_pub = nh.advertise<geometry_msgs::Twist>(PUBLISHER_TOPIC, 1);
    goal_sub = nh.subscribe<geometry_msgs::PoseStamped>(GOAL_TOPIC, 100, &SeekBehaviour::on_Goal_Received, this);

}

void SeekBehaviour::publish_cmd(geometry_msgs::Twist twist) {
    cout << twist << " published to cmd_vel0" << endl;
    vel_pub.publish(twist);
    ros::spinOnce();

    tf::TransformListener listener;
    tf::StampedTransform transform;

    return;
}

bool SeekBehaviour::checkObstacle() {
    bool received = false;
    nh.getParam("/escape/scan_received", received);
    ROS_INFO_STREAM(received);
    return received;
}

void SeekBehaviour::on_Goal_Received(geometry_msgs::PoseStamped goalMsg) {
    if (currentState == IDLE) {
        geometry_msgs::PoseStamped currentPose = getRobotPose();
        currentState = PREPROCESS;
        if (currentPose.pose.position.x != goalMsg.pose.position.x && currentPose.pose.position.y != goalMsg.pose.position.y) {
            process();
            goal = goalMsg;
            geometry_msgs::Twist twist;
            twist.angular.z = 0;
            twist.linear.x = 0;
            while(currentState==PREPROCESS){
                publish_cmd(twist);
                
            }

        }
    }
}

void SeekBehaviour::process() {
    if (currentState == DRIVING) {            goal = goalMsg;
            geometry_msgs::Twist twist;
            twist.angular.z = 0;
            twist.linear.x = 0;
            double turn = 0;
            tf::Quaternion qCur, qNext;

            qCur = tf::createQuaternionFromRPY(getRobotPose().pose.orientation.x, getRobotPose().pose.orientation.y, getRobotPose().pose.orientation.z);
            qNext = tf::createQuaternionFromYaw(calculateAngle(getRobotPose().pose, goalMsg.pose));
            double angle = qCur.angleShortestPath(qNext);
        getRobotPose();
        
    }

}

geometry_msgs::PoseStamped SeekBehaviour::getRobotPose() {


    bool transform_exist = false;
    ros::Time now = ros::Time::now();
    geometry_msgs::PoseStamped current_robot_pose;
    transform_exist = listener->waitForTransform(PLAN_FRAME, ROBOT_FRAME, now, ros::Duration(2.0));
    if (transform_exist) {
        ROS_INFO_STREAM("Transform found");
        listener->lookupTransform(PLAN_FRAME, ROBOT_FRAME, ros::Time(0), transform);

        current_robot_pose.header.frame_id = ROBOT_FRAME;
        current_robot_pose.pose.position.x = transform.getOrigin().x();
        current_robot_pose.pose.position.y = transform.getOrigin().y();
        current_robot_pose.pose.position.z = transform.getOrigin().z();
        current_robot_pose.pose.orientation.w = transform.getRotation().w();
        current_robot_pose.pose.orientation.x = transform.getRotation().x();
        current_robot_pose.pose.orientation.y = transform.getRotation().y();
        current_robot_pose.pose.orientation.z = transform.getRotation().z();

        ROS_INFO_STREAM(current_robot_pose);
    }
    return current_robot_pose;
}

/*Main Function*/
int main(int argc, char **argv) {
    ros::init(argc, argv, NODE_NAME);
    SeekBehaviour seek_behaviour;
    seek_behaviour.process();


    ros::spin();
    return 0;
}
