//Code for the QRE1113 Digital board


#include <ros.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;
  
geometry_msgs::Twist wheel_msg;
ros::Publisher pub_encoder("encoder", &wheel_msg);

//Connect pins
  int QRE1113_Pin = 2; //connected to digital 2
  

//Define Constants
  const int threshold_value = 700;  // 0 = maximum reflection, 3000 = no reflection 
  const int encoder_count = 8;     
  
  int current_color = 0;  // Black = 0, white = 1;
  int count = 0;          
  int rotations = 0;
  long master_timer;
  long time_diff;
  long rpm;


void setup(){
  nh.initNode();
  nh.advertise(pub_encoder);
 }

void loop(){
  
  int QRE_Value = readQD();
  
  if(QRE_Value > threshold_value && current_color == 1){
      current_color = 0;
      ++count;
  }
  else if(QRE_Value < threshold_value && current_color == 0){
      current_color = 1;
      ++count;     
  }
  if(count == encoder_count)
  {
    ++rotations;
    count = 0;  
    time_diff = (micros() - master_timer);
    rpm = (1.00000/(time_diff/1000000.00000))*60.00000;  
    master_timer = micros();
    wheel_msg.angular.x = rpm;
    pub_encoder.publish(&wheel_msg);
   
  }
 
  
  nh.spinOnce();
  delay(1);

     
}

int readQD(){
  //Returns value from the QRE1113
  //Lower numbers mean more refleacive
  //More than 3000 means nothing was reflected.
  pinMode( QRE1113_Pin, OUTPUT );
  digitalWrite( QRE1113_Pin, HIGH );
  delayMicroseconds(10);
  pinMode( QRE1113_Pin, INPUT );
  
  long time = micros();
  
  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  while(digitalRead(QRE1113_Pin) == HIGH && micros() - time < 3000); 
  int diff = micros() - time; 
  return diff;
}
