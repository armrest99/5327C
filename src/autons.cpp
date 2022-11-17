#include "main.h"
#include "pros/misc.h"
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

bool oneSpeed = true;
double tbh = 40.0;
double prev_error = 0.0;
double flyDrive = 0.0;
bool isAuton = true;


///
// Drive Example
///
void runFlywheel(double velocity) {
  flywheel.move_voltage(velocity);
  flywheel2.move_voltage(velocity);
}

void autoFlywheel(double velocity) {
    //double velocity = *velo;
    //runFlywheel(velocity);
    double change = .25;
    double currentVelo = flywheel.get_actual_velocity(); 
    double error = velocity - currentVelo; 
    double output = flyDrive + change * error;
    if (error * prev_error < 0) {
        output = 0.5 * (output + tbh);
        tbh = output;
    }
    controller.print(1, 1, "flywheelspeed %f", currentVelo);
    runFlywheel(output);
    flyDrive = output;
    prev_error = error;
}

void flywheel_task(void* param){
  double *v = (double*) param;


  
  //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
  while(isAuton){//pros::Task::notify_take(true, TIMEOUT_MAX)==1){//pros::Task::notify_take(true, TIMEOUT_MAX)){
    //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)))
    if (oneSpeed){
      autoFlywheel(480);
    }
    else{
      autoFlywheel(440);
    }
    //autoFlywheel(v);
    pros::delay(ez::util::DELAY_TIME);
  }
}
///
// Combining Turn + Drive
///
void test(){
  isAuton = true;
  pros::Task fly = pros::Task(flywheel_task, (void*)1);
  
  //flywheel = 0;
}

void flyPID(float voltage) {
  double error;
	double kP = 0.1;
	double kI = 0.001;
  double kD = 0.01;
	double totalError = 0;
	double lastError = voltage;
	while(error > 0){	
		float currentVoltage = (flywheel.get_voltage() + flywheel2.get_voltage()) / 2.00;

		error = voltage - currentVoltage;
		double errorDifference = lastError - error;
		totalError += error;
		double motorSpeed = (error * kP) + (totalError * kI) + (errorDifference * kD);


		lastError = error;
	}
}
void rightAuton() {
  //roller
  isAuton = true;
  pros::Task fly = pros::Task(flywheel_task, (void*)1);
  intake = -127;
  chassis.set_drive_pid(20, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(90, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(4, DRIVE_SPEED);
  chassis.wait_drive();
  
  
  // fire preloads
  chassis.set_drive_pid(-13, DRIVE_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(95, TURN_SPEED);
  chassis.wait_drive();

  pros::delay(1000);
  indexer.move_relative(600, 400);
  pros::delay(900);
  indexer.move_relative(600,600);
  pros::delay(200);

  // triple disc
  chassis.set_turn_pid(215, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(22, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(30, 50);
  chassis.wait_drive();

  //Fire triple stack
  chassis.set_turn_pid(130, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-2, DRIVE_SPEED);
  chassis.wait_drive();
  pros::delay(100);
  indexer.move_relative(600,600);
  pros::delay(1250);
  indexer.move_relative(600, 600);
  pros::delay(750);
  indexer.move_relative(600,600);
  oneSpeed = false;

  }

void leftAuton() {
     //roller
  isAuton = true;
  pros::Task fly = pros::Task(flywheel_task, (void*)1);
  chassis.set_drive_pid(3, DRIVE_SPEED, true);
  intake = -127;
  

  pros::delay(300);
  //Fire Preloads
  chassis.set_drive_pid(-13, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(-13, TURN_SPEED);
  chassis.wait_drive();

  pros::delay(1000);
  indexer.move_relative(600, 600);
  pros::delay(1500);
  indexer.move_relative(600,600);
  

  pros::delay(400);


  //triple stack
  chassis.set_turn_pid(-120, TURN_SPEED);
  chassis.wait_drive();
  oneSpeed = false;
  chassis.set_drive_pid(22, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(30, 60);
  chassis.wait_drive();

  // //Fire triple stack
  chassis.set_turn_pid(-41, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-11, DRIVE_SPEED);
  chassis.wait_drive();
  pros::delay(100);
  indexer.move_relative(600,600);
  pros::delay(1250);
  indexer.move_relative(600, 600);
  pros::delay(700);
  indexer.move_relative(600,600);

  // //Low goal discs
  // chassis.set_drive_pid(10, DRIVE_SPEED);
  // chassis.wait_drive();
  // chassis.set_turn_pid(10, TURN_SPEED);
  // chassis.wait_drive();
  // chassis.set_drive_pid(10, DRIVE_SPEED);
  // chassis.wait_drive();
  // chassis.set_turn_pid(-10, TURN_SPEED);
  // chassis.wait_drive();
  // indexer = 127;
  // pros::delay(850);
  // indexer = 0;
  // fly.notify();
}


void left_awp(){
    //roller
  isAuton = true;
  pros::Task fly = pros::Task(flywheel_task, (void*)1);
  chassis.set_drive_pid(3, DRIVE_SPEED, true);
  intake = -127;
  

  pros::delay(500);
  //Fire Preloads
  chassis.set_drive_pid(-13, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(-13, TURN_SPEED);
  chassis.wait_drive();

  pros::delay(1000);
  indexer.move_relative(600, 600);
  pros::delay(1300);
  indexer.move_relative(600,600);
  

  pros::delay(400);


  //triple stack
  chassis.set_turn_pid(-120, TURN_SPEED);
  chassis.wait_drive();
  oneSpeed = false;
  chassis.set_drive_pid(22, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(30, 60);
  chassis.wait_drive();

  // //Fire triple stack
  chassis.set_turn_pid(-41, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-11, DRIVE_SPEED);
  chassis.wait_drive();
  pros::delay(100);
  indexer.move_relative(600,600);
  pros::delay(1250);
  indexer.move_relative(600, 600);
  pros::delay(700);
  indexer.move_relative(600,600);



  // additional part

  pros::delay(200);
  chassis.set_turn_pid(-130, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_drive_pid(80, 127);
  chassis.wait_drive();
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