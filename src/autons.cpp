#include "main.h"
#include "roboto/roboto.hpp"


/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////


const int DRIVE_SPEED = 110; // This is 110/127 (around 87% of max speed).  We don't suggest making this 127.
                             // If this is 127 and the robot tries to heading correct, it's only correcting by
                             // making one side slower.  When this is 87%, it's correcting by making one side
                             // faster and one side slower, giving better heading correction.
const int TURN_SPEED  = 90;
const int SWING_SPEED = 90;



///
// Constants
///

// It's best practice to tune constants when the robot is empty and with heavier game objects, or with lifts up vs down.
// If the objects are light or the cog doesn't change much, then there isn't a concern here.

void default_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void one_mogo_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

void two_mogo_constants() {
  chassis.set_slew_min_power(80, 80);
  chassis.set_slew_distance(7, 7);
  chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
  chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
  chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
  chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
}

// void exit_condition_defaults() {
//   chassis.set_exit_condition(chassis.turn_exit, 100, 3, 500, 7, 500, 500);
//   chassis.set_exit_condition(chassis.swing_exit, 100, 3, 500, 7, 500, 500);
//   chassis.set_exit_condition(chassis.drive_exit, 80, 50, 300, 150, 500, 500);
// }

// void modified_exit_condition() {
//   chassis.set_exit_condition(chassis.turn_exit, 100, 3, 500, 7, 500, 500);
//   chassis.set_exit_condition(chassis.swing_exit, 100, 3, 500, 7, 500, 500);
//   chassis.set_exit_condition(chassis.drive_exit, 80, 50, 300, 150, 500, 500);
// }



///
// Drive Example
///
void leftAuton() {
  // The first parameter is target inches
  // The second parameter is max speed the robot will drive at
  // The third parameter is a boolean (true or false) for enabling/disabling a slew at the start of drive motions
  // for slew, only enable it when the drive distance is greater then the slew distance + a few inches

  
}



///
// Turn Example
///
void rightAuton() {
  // The first parameter is target degrees
  // The second parameter is max speed the robot will drive at


  
}
void runFlywheel(double velocity) {
  flywheel.move_velocity(velocity);
  flywheel2.move_velocity(velocity);
}

void autoFlywheel(double velocity) {
    //double velocity = *velo;
    //runFlywheel(velocity);

    double avgFVel = (flywheel2.get_actual_velocity() + flywheel.get_actual_velocity()) / -2; 
    double bangConstant = std::abs(velocity- avgFVel)/velocity; 

    if (avgFVel < velocity) {
        runFlywheel(velocity + (bangConstant * (avgFVel - velocity)));
    }
    else {
        runFlywheel(velocity + (bangConstant * (velocity - avgFVel))); 
    }

}

void flywheel_task(void* param){
  double *v = (double*) param;
  bool a = true;
  //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
  while(true){//pros::Task::notify_take(true, TIMEOUT_MAX)==1){//pros::Task::notify_take(true, TIMEOUT_MAX)){
    //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
    if (a) autoFlywheel(400);
    else runFlywheel(0);
    if(pros::Task::notify_take(true, TIMEOUT_MAX)){
      a = !a;
    }
    //autoFlywheel(v);
  }
}

///
// Combining Turn + Drive
///
void test(){
  pros::Task fly = pros::Task(flywheel_task, (void*) 103);
  pros::delay(10000);
  fly.notify();

  //flywheel = 0;
}

void drive_example() {
  intake = -127;
  pros::Task fly = pros::Task(flywheel_task, (void*) 80);
  //autoFlywheel(103.0);

  pros::delay(200);

  intake = 0;

  chassis.set_drive_pid(-10, DRIVE_SPEED);
  chassis.wait_drive();
  pros::delay(300);

  chassis.set_turn_pid(-7, TURN_SPEED);
  chassis.wait_drive();

  pros::delay(500);

  intake = -127;

  indexer = 127;
  // flywheel = 127;
  // flywheel2 = -127;
  
  pros::delay(200);

  indexer = 0;
  

  pros::delay(500);

  indexer = 127;
  
  pros::delay(300);
  
  indexer = 0;
  // flywheel = 100;
  // flywheel2 = -100;

  chassis.set_drive_pid(0, DRIVE_SPEED);
  chassis.wait_drive();


  chassis.set_turn_pid(-115, TURN_SPEED);
  chassis.wait_drive();
  pros::delay(300);

  chassis.set_drive_pid(25, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_drive_pid(40, 30, true);
  chassis.wait_drive();
  pros::delay(300);


  chassis.set_turn_pid(-30, TURN_SPEED);
  chassis.wait_drive();
  pros::delay(300);

  indexer = 127;

  pros::delay(300);

  indexer = 0;

  pros::delay(300);

  indexer = 127;

  pros::delay(300);

  indexer = 0;

  pros::delay(300);

  indexer = 127;

  pros::delay(300);

  indexer = 0;

  fly.notify();







}



///
// Wait Until and Changing Max Speed
///
void wait_until_change_speed() {
  // wait_until will wait until the robot gets to a desired position


  // When the robot gets to 6 inches, the robot will travel the remaining distance at a max speed of 40
  chassis.set_drive_pid(24, DRIVE_SPEED, true);
  chassis.wait_until(6);
  chassis.set_max_speed(40); // After driving 6 inches at DRIVE_SPEED, the robot will go the remaining distance at 40 speed
  chassis.wait_drive();

  chassis.set_turn_pid(45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(-45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(0, TURN_SPEED);
  chassis.wait_drive();

  // When the robot gets to -6 inches, the robot will travel the remaining distance at a max speed of 40
  chassis.set_drive_pid(-24, DRIVE_SPEED, true);
  chassis.wait_until(-6);
  chassis.set_max_speed(40); // After driving 6 inches at DRIVE_SPEED, the robot will go the remaining distance at 40 speed
  chassis.wait_drive();
}



///
// Swing Example
///
void swing_example() {
  // The first parameter is ez::LEFT_SWING or ez::RIGHT_SWING
  // The second parameter is target degrees
  // The third parameter is speed of the moving side of the drive


  chassis.set_turn_pid(90, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(0, TURN_SPEED);
  chassis.wait_drive();
}



///
// Auto that tests everything
///
void combining_movements() {
  chassis.set_drive_pid(24, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_swing_pid(ez::RIGHT_SWING, -45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(0, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(-24, DRIVE_SPEED, true);
  chassis.wait_drive();
}



///
// Interference example
///
void tug (int attempts) {
  for (int i=0; i<attempts-1; i++) {
    // Attempt to drive backwards
    printf("i - %i", i);
    chassis.set_drive_pid(-12, 127);
    chassis.wait_drive();

    // If failsafed...
    if (chassis.interfered) {
      chassis.reset_drive_sensor();
      chassis.set_drive_pid(-2, 20);
      pros::delay(1000);
    }
    // If robot successfully drove back, return
    else {
      return;
    }
  }
}

// If there is no interference, robot will drive forward and turn 90 degrees. 
// If interfered, robot will drive forward and then attempt to drive backwards. 
void interfered_example() {
 chassis.set_drive_pid(24, DRIVE_SPEED, true);
 chassis.wait_drive();

 if (chassis.interfered) {
   tug(3);
   return;
 }

 chassis.set_turn_pid(90, TURN_SPEED);
 chassis.wait_drive();
}



// . . .
// Make your own autonomous functions here!
// . . .