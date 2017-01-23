#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"
#include <string>

char* factor(int input){
    char* result;
    result = (char*) malloc(500);
    if (input == 1){
        sprintf(result, "%d", input);
        return result;
    }
    int i;
    float end = sqrt((float)input);
    for (i=2; i<=end; i++) {
        if (input % i == 0) {
            result = strcat(factor(input / i), ", ");
            return strcat(result, factor(i));
        }
    }
    sprintf(result, "%d", input);
    return result;
}

class PrimeFactor
{

public:
    PrimeFactor()
    {
        //setup publishers
        factors_pub = n.advertise<std_msgs::String>("/output", 1000);

        //setup subscribers
        sub = n.subscribe("/input", 10, &PrimeFactor::input_callback, this);
    }

    void input_callback(const std_msgs::Int16& input){
        //printf("callback");
        char* factors;
        factors = factor(input.data);
        msg.data = factors;
        factors_pub.publish(msg);
    }


    void run(){
        printf("start");
        ros::Rate rosrate(0.5);
        while (ros::ok()){
            ros::spinOnce();
            rosrate.sleep();
        }
    }

private:
    ros::NodeHandle n;
    ros::Publisher factors_pub;
    ros::Subscriber sub;

    std_msgs::String msg;

};//End of class SubscribeAndPublish

int main(int argc, char **argv)
{
    //Initiate ROS
    ros::init(argc, argv, "prime_factor");

    //Create an object of class SubscribeAndPublish that will take care of everything
    PrimeFactor object;

    object.run();

    return 0;
}

int main2(){
    puts(factor(10));
}