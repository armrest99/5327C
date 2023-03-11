#include "autons.hpp"
#include "main.h"
#include "pros/misc.h"
#include "roboto/roboto.hpp"

/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////
#define PI 3.1415926

//Math Definitions


#define ENC_TO_INCH (2.75*PI/36000)
#define DEG_TO_RAD PI/180
#define RAD_TO_DEG 180/PI

//The distance from the tracker wheels to the center
#define TRACKER_DISTANCE 2 //Unknown right now this is placeholder

const int DRIVE_SPEED = 127; // This is 110/127 (around 87% of max speed).  We don't suggest making
         // this 127. If this is 127 and the robot tries to heading correct,
         // it's only correcting by making one side slower.  When this is 87%,
         // it's correcting by making one side faster and one side slower,
         // giving better heading correction.
const int TURN_SPEED = 110;
const int SWING_SPEED = 90;

///
// Constants
///

// It's best practice to tune constants when the robot is empty and with heavier
// game objects, or with lifts up vs down. If the objects are light or the cog
// doesn't change much, then there isn't a concern here.

// void default_constants() {
//   chassis.set_slew_min_power(80, 80);
//   chassis.set_slew_distance(7, 7);
//   chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
//   chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
//   chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
//   chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
//   chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
// }

// void one_mogo_constants() {
//   chassis.set_slew_min_power(80, 80);
//   chassis.set_slew_distance(7, 7);
//   chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
//   chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
//   chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
//   chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
//   chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
// }

// void two_mogo_constants() {
//   chassis.set_slew_min_power(80, 80);
//   chassis.set_slew_distance(7, 7);
//   chassis.set_pid_constants(&chassis.headingPID, 11, 0, 20, 0);
//   chassis.set_pid_constants(&chassis.forward_drivePID, 0.45, 0, 5, 0);
//   chassis.set_pid_constants(&chassis.backward_drivePID, 0.45, 0, 5, 0);
//   chassis.set_pid_constants(&chassis.turnPID, 5, 0.003, 35, 15);
//   chassis.set_pid_constants(&chassis.swingPID, 7, 0, 45, 0);
// }

void exit_condition_defaults() {
  chassis.set_exit_condition(chassis.turn_exit, 10, 1, 100, 7, 50, 500);
  chassis.set_exit_condition(chassis.swing_exit, 10, 1, 100, 7, 50, 500);
  chassis.set_exit_condition(chassis.drive_exit, 8, 50, 60, 150, 50, 500);
}

// void modified_exit_condition() {
//   chassis.set_exit_condition(chassis.turn_exit, 100, 3, 500, 7, 500, 500);
//   chassis.set_exit_condition(chassis.swing_exit, 100, 3, 500, 7, 500, 500);
//   chassis.set_exit_condition(chassis.drive_exit, 80, 50, 300, 150, 500, 500);
// }
//ODOM VARIABLES
float tare_back;
float tare_left;
float prev_y = 0;
float prev_x = 0;
float prev_angle = 0;
//GLOBAL X
float pos_x = 0;
//GLOBAL Y
float pos_y = 0;

bool oneSpeed = true;
bool skillSpeed = false;
double tbh = 215.0;
double prev_error = 0.0;
double output;
bool isAuton = true;
bool skillsBool = false;

//Reset encoder positions
void tare_rot(){
	while (tare_back == 0){
		tare_back = -back_rot.get_value();
		pros::delay(1);
	}
	while (tare_left == 0){
		tare_left = -left_rot.get_value();
		pros::delay(1);
	}
}
float back_pos(){
 	return -back_rot.get_value() - tare_back;
}
float left_pos(){
 	return -left_rot.get_value() - tare_left;
}
double getAngle(){
	double angle = inertial.get_heading();
  return angle;
}
void Pilons(){ //ODOMETRY WOOOO
	double angle = getAngle();
	double new_angle = angle-prev_angle;
	//tracked values in inches
	double current_x = left_pos();
	double current_y = back_pos();
	double new_local_x = (current_x-prev_x);
	double new_local_y = (current_y-prev_y);
	double distance = sqrt(pow(new_local_y,2)+pow(new_local_x,2));
	//account for backwards movement
	double changes = atan2(new_local_y,new_local_x);
	changes = (std::isnan(changes)) ? PI/2 : changes;
	angle +=  changes;
	double dx = cos(angle)*distance*ENC_TO_INCH;
	double dy = sin(angle)*distance*ENC_TO_INCH;
	pos_x += dx;
	pos_y += dy;
	prev_x = current_x;
	prev_y = current_y;
	prev_angle = angle;
}
void rammusete(double targetX, double targetY, double targetAngle){ //ramsete controller motion algorithm
  bool stop = false;
  while (!stop){
    double errorX = (targetX - pos_x); //gives us global error
    double errorY = (targetY - pos_y);
    double errorA = (targetAngle - inertial.get_heading());
    double e_x = ((cos(inertial.get_heading())*errorX)+(sin(inertial.get_heading())*errorY)); //gives us the robot's local error
    double e_y = ((-sin(inertial.get_heading())*errorX)+(cos(inertial.get_heading())*errorY));

    double b = 0.1; //Proportional constant, larger value will make the controller more aggressive
    double damp = 0.05; //similar to the D constant for a PID controller
    double linVelo = (0.02 * e_x); //Linear Velocity
    double angVelo = (0.02 * errorA); //Angular Velocity

    double gain = 2 * damp * sqrt(pow(angVelo,2)+b*pow(linVelo,2)); //Calculates the gain value for the controller

    //Movement Speeds
    double fSpeed = (linVelo * cos(errorA) + gain * e_x);
    double angSpeed = (angVelo + gain * errorA + (b * linVelo * sin(errorA) * e_y)/errorA);
    double linMotorVelo = fSpeed / (4 * PI);
    double genSpeed = linMotorVelo + angSpeed;
    //Movement Commands
    left_wheel_front.move(genSpeed);
    left_wheel_middle.move(genSpeed);
    left_wheel_back.move(genSpeed);

    right_wheel_front.move(genSpeed);
    right_wheel_middle.move(genSpeed);
    right_wheel_back.move(genSpeed);

    if(right_wheel_middle.is_stopped() && left_wheel_middle.is_stopped()){
      stop = true;
    }
  }
  left_wheel_front.move(0);
  left_wheel_middle.move(0);
  left_wheel_back.move(0);

  right_wheel_front.move(0);
  right_wheel_middle.move(0);
  right_wheel_back.move(0);
}
void checkPos(double currentX, double currentY, double currentAngle, double targetX, double targetY, double targetAngle) {
  if (abs(currentX - targetX) > 2 || abs(currentY-targetY) > 2) {
    // math to get angle and distance, returns reqAngle, reqDistance
    double reqAngle = 0;
    double reqDistance = 0;
    chassis.set_turn_pid(reqAngle, TURN_SPEED);
    chassis.set_drive_pid(reqDistance, DRIVE_SPEED);
    chassis.set_turn_pid((reqAngle-targetAngle), TURN_SPEED);

    
  } else if (abs(currentAngle - targetAngle) > 3) {
    chassis.set_turn_pid((currentAngle-targetAngle), TURN_SPEED);
  } else {
    std:: cout << "correct pos";
  }
}

void runFlywheel(double velocity) { flywheel.move_voltage(velocity); }

void autoFlywheel(double velocity) {
  // double velocity = *velo;
  // runFlywheel(velocity);
  double change = .12;
  double currentVelo = flywheel.get_actual_velocity();
  double error = velocity - currentVelo;
  output += (change * error);
  if (error * prev_error < 0) {
    output = 0.5 * (output + tbh);
    tbh = output;
  }
  controller.print(1, 1, "flywheelspeed %f", currentVelo);
  runFlywheel(output);
  prev_error = error;
}

void flywheel_task(void *param) {
  double *v = (double *)param;
  // pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false,
  // TIMEOUT_MAX)));
  while (isAuton) { // pros::Task::notify_take(true,
                    // TIMEOUT_MAX)==1){//pros::Task::notify_take(true,
                    // TIMEOUT_MAX)){
    // pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false,
    // TIMEOUT_MAX)))
    if (oneSpeed) {
      autoFlywheel(540);
    } 
    else if (skillSpeed) {
      autoFlywheel(420);
    } else {
      autoFlywheel(435);
    }
    // if (skillsBool){
    optical_sensor.set_led_pwm(100);
    //for red rollers
    if (optical_sensor.get_hue() < 20){
    intake.move_velocity(0);
    }
    //for blue rollers
    // if(optical_sensor.get_hue() > 210){
    //   intake .move_velocity (0)
    // }
  }

    // autoFlywheel(v);
    pros::delay(ez::util::DELAY_TIME);
}
///
// Combining Turn + Drive
///
void test() {
  isAuton = true;
  pros::Task fly = pros::Task(flywheel_task, (void *)1);

  // flywheel = 0;
}

void flyPID(float voltage) {
  double error;
  double kP = 0.1;
  double kI = 0.001;
  double kD = 0.01;
  double totalError = 0;
  double lastError = voltage;
  while (error > 0) {
    float currentVoltage = flywheel.get_voltage();

    error = voltage - currentVoltage;
    double errorDifference = lastError - error;
    totalError += error;
    double motorSpeed =
        (error * kP) + (totalError * kI) + (errorDifference * kD);

    lastError = error;
  }
}
void rightAuton() {
  // roller and 3 stack
  transmission.set_value(true);
  skillSpeed = false;
  skillsBool = false;
  tbh = 280.0;
  oneSpeed = true;
  isAuton = true;
  intake = 0;
  chassis.set_drive_pid(-15, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(90, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-7, DRIVE_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(300);
  intake = 0;
}

void leftAuton() {
  // roller
  transmission.set_value(true);
  tbh = 280.0;
  oneSpeed = true;
  isAuton = true;

  pros::Task fly = pros::Task(flywheel_task, (void *)1);
  intake = 0;
  chassis.set_drive_pid(-3, DRIVE_SPEED, true);
  chassis.wait_drive();
  // Fire Preloads
  intake = 127;
  pros::delay(300);
  intake = 0;

  chassis.set_swing_pid(ez::LEFT_SWING, 47, SWING_SPEED);
  chassis.wait_drive();
  actualIntakeLift.set_value(false);
  chassis.set_drive_pid(20, DRIVE_SPEED);
  chassis.wait_drive();
  intake = -127;
  actualIntakeLift.set_value(false);
  pros::delay(200);
  chassis.set_drive_pid(20, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(-34, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(15, DRIVE_SPEED);
  chassis.wait_drive();
  pros::delay(6000);
  intake.move_absolute(100, 600);
  pros::delay(100);
  intake = 0;
}

void Skills() {
  skillsBool = true;
  tbh = 210.0;
  oneSpeed = false;
  skillSpeed = true;
  angleChanger.set_value(false);
  pros::Task fly = pros::Task(flywheel_task, (void *)1);
  chassis.set_drive_pid(-3, DRIVE_SPEED, true);
  chassis.wait_drive();
  intake = -127;
  pros::delay(400);
  intake = 0;
  // Fire Preloads
  chassis.set_drive_pid(5, DRIVE_SPEED, true);
  chassis.wait_drive();
  intake = -127;
  chassis.set_turn_pid(-60, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(15, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(90, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-11.5, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(400);
  intake = 0;
  chassis.set_swing_pid(ez::RIGHT_SWING, 0, SWING_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(47, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 127;
  pros::delay(500);
  intake = -127;
  // intake and shoot low goal
  chassis.set_swing_pid(ez::LEFT_SWING, 90, SWING_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(34, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_drive_pid(-34, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::LEFT_SWING, 0, SWING_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_drive_pid(-24, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  chassis.set_turn_pid(40, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(54, DRIVE_SPEED, true);
  chassis.wait_drive();
  angleChanger.set_value(true);
  chassis.set_turn_pid(-40, TURN_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  // shotBlocker.set_value(true);
  // intake and shoot 3 stack
  chassis.set_turn_pid(40, TURN_SPEED);
  chassis.wait_drive();
  actualIntakeLift.set_value(true);
  chassis.set_drive_pid(22, DRIVE_SPEED, true);
  chassis.wait_drive();
  actualIntakeLift.set_value(false);
  pros::delay(700);
  chassis.set_turn_pid(-70, TURN_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_turn_pid(-130, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-11, DRIVE_SPEED);
  chassis.wait_drive();
  
  chassis.set_swing_pid(ez::LEFT_SWING, -180, SWING_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-5.5, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(400);
  intake = 0;
  actualIntakeLift.set_value(true);
  chassis.set_swing_pid(ez::RIGHT_SWING, -190, SWING_SPEED);
  chassis.wait_drive();
  intake = -127;
  chassis.set_drive_pid(5, DRIVE_SPEED, true);
  chassis.wait_drive();
  actualIntakeLift.set_value(false);
  pros::delay(700);
  // Last roller 
  chassis.set_turn_pid(-90, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-15, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(400);
  intake = 0;
  chassis.set_swing_pid(ez::RIGHT_SWING, -180, SWING_SPEED);
  chassis.wait_drive();
  intake = -127;
  angleChanger.set_value(false);
  chassis.set_drive_pid(37, DRIVE_SPEED, true);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_swing_pid(ez::LEFT_SWING, -90, SWING_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(34, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_drive_pid(-34, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::LEFT_SWING, -180, SWING_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_drive_pid(-41, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-130, TURN_SPEED);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  // shotBlocker.set_value(true);
  // itnake and shoot 3 discs
  chassis.set_drive_pid(36, DRIVE_SPEED, true);
  chassis.wait_drive();
  angleChanger.set_value(true);
  chassis.set_turn_pid(-210, TURN_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_swing_pid(ez::LEFT_SWING, -180, SWING_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(20, DRIVE_SPEED, true);
  chassis.wait_drive();
  angleChanger.set_value(false);
  chassis.set_turn_pid(-270, TURN_SPEED);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_drive_pid(-6, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-400, TURN_SPEED);
  chassis.wait_drive();
  actualIntakeLift.set_value(true);
  // shotBlocker.set_value(true);
  // 2nd to last 3 stack
  chassis.set_drive_pid(25, DRIVE_SPEED, true);
  chassis.wait_drive();
  angleChanger.set_value(true);
  actualIntakeLift.set_value(false);
  pros::delay(700);
  chassis.set_turn_pid(-270, TURN_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(500);
  intake = -127;
  chassis.set_swing_pid(ez::LEFT_SWING, -320, SWING_SPEED);
  chassis.wait_drive();
  expansion.set_value(true);
  chassis.set_drive_pid(-25, DRIVE_SPEED, true);
  chassis.wait_drive();
 
}

///
// Wait Until and Changing Max Speed
///
void wait_until_change_speed() {
  // wait_until will wait until the robot gets to a desired position

  // When the robot gets to 6 inches, the robot will travel the remaining
  // distance at a max speed of 40
  chassis.set_drive_pid(24, DRIVE_SPEED, true);
  chassis.wait_until(6);
  chassis.set_max_speed(40); // After driving 6 inches at DRIVE_SPEED, the robot
                             // will go the remaining distance at 40 speed
  chassis.wait_drive();

  chassis.set_turn_pid(45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(-45, TURN_SPEED);
  chassis.wait_drive();

  chassis.set_turn_pid(0, TURN_SPEED);
  chassis.wait_drive();

  // When the robot gets to -6 inches, the robot will travel the remaining
  // distance at a max speed of 40
  chassis.set_drive_pid(-24, DRIVE_SPEED, true);
  chassis.wait_until(-6);
  chassis.set_max_speed(40); // After driving 6 inches at DRIVE_SPEED, the robot
                             // will go the remaining distance at 40 speed
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
void tug(int attempts) {
  for (int i = 0; i < attempts - 1; i++) {
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
