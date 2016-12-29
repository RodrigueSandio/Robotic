#ifndef SR_NODE_WALL_FOLLOWING
#define SR_NODE_WALL_FOLLOWING

#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"

/*! \brief Demonstration task: "Wall Following"
 * 
 * This class controls robot. Robot finds nearest wall and goes along it.
 */
 
 class NodeWallFollowing
{
public:
	
	/*! \brief A constructor.
	 * 
	 * @param pub Publisher, which can send commands to robot.
	 * @param wallDist Desired distance from the wall.
	 * @param maxSp Maximum speed, that robot can go.
	 * @param dir 1 for wall on the right side of the robot (-1 for the left one).
	 * @param pr P constant for PSD regulator.
	 * @param su S constant for PSD regulator.
	 * @param di D constant for PSD regulator.
	 * @param an Angle coeficient for regulator.
	 */
	NodeWallFollowing(ros::Publisher pub, double wallDist, double maxSp, int dir, double pr, double su, double di, double an);
	
	/*! \brief A destructor.
	 */
	~NodeWallFollowing();

	/*! \brief This method publishes commands for robot.
	 *
	 * Commands are generated from data, which are stored in variables
	 * (#angleMin, #angleMax, #distMin, #distMax). If the robot is far from 
	 * nearest obstacle (distMin is bigger than desired distance plus 
	 * constant), it turns to the obstacle and goes there. If it is near 
	 * the desired distance, it turns and goes along the obstacle. 
	 * The whole time, when robot is going along the obstacle (wall), it 
	 * tries to keep desired distance and angle from the wall. The higher
	 * is the diference between desired and actual values, the lower is 
	 * speed.
	 */
	
	void publishMessage();

	/*! \brief This method reads data from sensor and processes them to variables.
	 * 
	 * This method finds maximum and minimum distance in data from sensor
	 * and stores these values (with appropriate angles) into variables: 
	 * #angleMin, #angleMax, #distMin, #distMax.
	 * 
	 * @param msg Message, which came from robot and contains data from
	 * laser scan.
	 */
	
	void messageCallback(const sensor_msgs::LaserScan::ConstPtr& msg);

//variables
	double wallDistance;//!<Desired distance from the wall.
	double r;			//!<Difference between desired distance from the wall and actual distance.
	double sumR;		//!<Sum of #r.
	double diffR;		//!<Estimated next r;
	double maxSpeed;	//!<Maximum speed of robot.
	double P;			//!<P constant for PSD regulator.
	double S;			//!<S constant for PSD regulator.
	double D; 			//!<D constant for PSD regulator.
	double angleCoef;	//!<Angle coeficient for regulator.
	int direction;		//!<1 for wall on the right side of the robot (-1 for the left one).
	double angleMin;	//!<Angle, at which was measured the shortest distance.
	double distMin;		//!<Minimum distance masured by ranger.
	double distFront;	//!<Distance, measured by ranger in front of robot.
	int go;				//!<If the obstacle is in front of robot, change to 0.
	ros::Publisher pubMessage;	//!<Object for publishing messages.
};

#endif