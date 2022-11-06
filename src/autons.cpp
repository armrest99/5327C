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


double tbh = 0.0;
double prev_error = 0.0;
double flyDrive = 0.0;
///
// Drive Example
///
void runFlywheel(double speed) {
  flywheel.move_voltage(speed);
  flywheel2.move_voltage(speed);
}

void autoFlywheel(double velocity) {
    //double velocity = *velo;
    //runFlywheel(velocity);
    double change = .025;
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
    
    // double currentVelo = (flywheel2.get_actual_velocity() + flywheel.get_actual_velocity()) / 2; 
    // double bangConstant = std::abs((currentVelo-velocity)/100); 

    // if (currentVelo > velocity) {
    //     runFlywheel(velocity - (bangConstant));
    // }
    // else if (currentVelo < velocity) {
    //     runFlywheel(velocity + (bangConstant)); 
    // }
    // else{
    //   runFlywheel(velocity);
    // }
}

void flywheel_task(void* param){
  double *v = (double*) param;
  bool oneSpeed = true;
  //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
  while(true){//pros::Task::notify_take(true, TIMEOUT_MAX)==1){//pros::Task::notify_take(true, TIMEOUT_MAX)){
    //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
    if (oneSpeed) {
      pros::lcd::set_text(2,"a");
      autoFlywheel(500);
    }
    else if (!oneSpeed) {
      pros::lcd::set_text(2,"b");
      autoFlywheel(50);
    }
    //autoFlywheel(v);
  }
}
///
// Combining Turn + Drive
///
void test(){
  pros::Task fly = pros::Task(flywheel_task, (void*)1);
  pros::delay(5000);
  indexer = 127;
 
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

void leftAuton() {
  //roller
  pros::Task fly = pros::Task(flywheel_task, (void*)1);
  chassis.set_drive_pid(5, DRIVE_SPEED, true);
  intake = -127;
  

  pros::delay(300);
  //Fire Preloads
  chassis.set_drive_pid(-13, DRIVE_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(-15, TURN_SPEED);
  chassis.wait_drive();

  pros::delay(200);
  indexer.move_relative(650, 600);
  pros::delay(300);
  indexer.move_relative(650,600);

  //triple stack
  // chassis.set_turn_pid(-120, TURN_SPEED);
  // chassis.wait_drive();
  // chassis.set_drive_pid(22, DRIVE_SPEED);
  // chassis.wait_drive();
  // chassis.set_drive_pid(30, 60);
  // chassis.wait_drive();

  // //Fire triple stack
  // chassis.set_turn_pid(-35, TURN_SPEED);
  // chassis.wait_drive();
  // chassis.set_drive_pid(-2, DRIVE_SPEED);
  // chassis.wait_drive();
  // pros::delay(100);

  // indexer = 127;
  // pros::delay(200);
  // indexer = 0;
  // pros::delay(100);
  // indexer = 127;
  // pros::delay(200);
  // indexer = 0;
  // pros::delay(100);
  // indexer = 127;
  // pros::delay(200);
  // indexer = 0;


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