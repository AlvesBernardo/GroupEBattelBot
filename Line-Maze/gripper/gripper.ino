#include <Servo.h>

//create servo object
Servo servo;
int angle = 10;

int distance;
void setup() {

    
     servo.attach(4);
     for (angle = 180; angle >= 0; angle -= 1){
      servo.write(angle);
      delay(15);
      }


     gripper();

  
}

void loop() {

    //==================[ Grippers ]====================
  servo.write(100); //Sends a signal for the servo to go to the 0 degrees position
  delay(3000);
  servo.write(45); //Sends a signal for the servo to go to the 180 degrees position
  delay(3000);


}


/*    =======================*/
/*    ======[funtions] ========*/
/*    =======================*/

void gripper() {

  if (distance > 25) {
    for (angle = 10; angle <= 180; angle += 1) {
      servo.write(angle);
      delay(15);
    }
  }

}
