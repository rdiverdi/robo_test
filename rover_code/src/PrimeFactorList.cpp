#include "ros/ros.h"
#include "std_msgs/Int16.h"
#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/String.h"

int factor(int input, int* output, int index=0){
    int i;
    float end = sqrt((float)input);
    for (i=2; i<=end; i++) {
        if (input % i == 0) {
            index = index + factor(input / i, output, index);
            index = index + factor(i, output, index);
            return index;
        }
    }
    output[index] = input;
    return 1;
}

class PrimeFactor
{

public:
    PrimeFactor()
    {
        //setup publishers
        factors_pub = n.advertise<std_msgs::Int16MultiArray>("/output", 1000);

        //setup subscribers
        sub = n.subscribe("/input", 10, &PrimeFactor::input_callback, this);
    }

    void input_callback(const std_msgs::Int16& input){
        int* factors;
        int len;
        factors = (int*) malloc(100);
        len = factor(input.data, factors);
        msg.data.clear();
        for (int i=0; i<len; i++){
            msg.data.push_back(factors[i]);
        }
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

    std_msgs::Int16MultiArray msg;

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
/*
int main(){
    int* test;
    test = (int*) malloc(100);
    int len = factor(50, test);
    int i;
    printf("[");
    for (i=0; i<len-1; i++) {
        printf("%d, ", test[i]);
    }
    printf("%d]\n", test[i]);
}
*/