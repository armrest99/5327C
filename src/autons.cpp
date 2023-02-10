#include "main.h"
#include "pros/misc.h"
#include "roboto/roboto.hpp"

/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////

const int DRIVE_SPEED =
    110; // This is 110/127 (around 87% of max speed).  We don't suggest making
         // this 127. If this is 127 and the robot tries to heading correct,
         // it's only correcting by making one side slower.  When this is 87%,
         // it's correcting by making one side faster and one side slower,
         // giving better heading correction.
const int TURN_SPEED = 90;
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
bool skillSpeed = false;
double tbh = 215.0;
double prev_error = 0.0;
double output;
bool isAuton = true;
bool skillsBool = false;
///
// Drive Example
///
void runFlywheel(double velocity) { flywheel.move_voltage(velocity); }

void autoFlywheel(double velocity) {
  // double velocity = *velo;
  // runFlywheel(velocity);
  double change = .25;
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
      autoFlywheel(470);
    } else if (skillSpeed) {
      autoFlywheel(380);
    } else {
      autoFlywheel(435);
    }
    // if (skillsBool){
    //   optical_sensor.set_led_pwm(100);
    //   if (optical_sensor.get_hue() > 210){
    //   intake.move_velocity(0);
    //   }
    //   else {
    //   intake.move_velocity(-420);
    //   }
    // }

    // autoFlywheel(v);
    pros::delay(1);
  }
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
    float currentVoltage =
        (flywheel.get_voltage() + flywheel2.get_voltage()) / 2.00;

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
  skillSpeed = false;
  skillsBool = false;
  tbh = 235.0;
  oneSpeed = true;
  isAuton = true;
  pros::Task fly = pros::Task(flywheel_task, (void *)1);
  intake = 127;
  intakeLift.set_value(false);
  chassis.set_drive_pid(-6.97056274848, DRIVE_SPEED);
  chassis.wait_drive();
  intakeLift.set_value(true);
  pros::delay(500);
  chassis.set_drive_pid(5, DRIVE_SPEED);
  chassis.wait_drive();
  transmission.set_value(true);
  chassis.set_turn_pid(-75, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-10, DRIVE_SPEED);
  chassis.wait_drive();
  intake = 0;
  chassis.set_turn_pid(-45, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-15, DRIVE_SPEED);
  chassis.wait_drive();
  intake = 127;
  pros::delay(100);
  intake = 0;
  chassis.set_drive_pid(5, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(-35, TURN_SPEED);
  chassis.wait_drive();
  intake.move_velocity(-500);
  pros::delay(200);

  // fire preloads
  chassis.set_turn_pid(-25, TURN_SPEED);
  chassis.wait_drive();
  intake = 127
  chassis.set_drive_pid(5, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-5, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(-135, TURN_SPEED);
  chassis.wait_drive();
  
  chassis.set_drive_pid(35, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(-15, TURN_SPEED);
  chassis.wait_drive();
  intake.move_velocity(-500);
  pros::delay(200);

  // triple disc
  chassis.set_turn_pid(-45, TURN_SPEED);
  chassis.wait_drive();
  intake = 127;
  chassis.set_drive_pid(15, DRIVE_SPEED);
  chassis.wait_drive();
  chassis.set_turn_pid(-90, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(28, DRIVE_SPEED);
  chassis.set_drive_pid(-7, DRIVE_SPEED);
  chassis.set_turn_pid(-5, TURN_SPEED);
  chassis.wait_drive();
  intake.move_velocity(-500);
  pros::delay(200);
}

void leftAuton() {
  // roller
  tbh = 235.0;
  oneSpeed = true;
  isAuton = true;

  pros::Task fly = pros::Task(flywheel_task, (void *)1);
  intake = -127;
  chassis.set_drive_pid(3, DRIVE_SPEED, true);
  chassis.wait_drive();

  pros::delay(100);
  // Fire Preloads
  chassis.set_drive_pid(-8, DRIVE_SPEED, true);
  chassis.wait_drive();

  chassis.set_turn_pid(-22, TURN_SPEED);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  pros::delay(700);
  intake = 127;
  pros::delay(160);
  intake = 0;
  pros::delay(800);
  intake = 127;
  pros::delay(160);
  intake = 0;
  // shotBlocker.set_value(true);
  pros::delay(400);
  oneSpeed = false;

  // triple stack
  chassis.set_turn_pid(-120, TURN_SPEED);
  chassis.wait_drive();
  intakeLift.set_value(true);
  chassis.set_drive_pid(18, DRIVE_SPEED);
  chassis.wait_drive();
  intakeLift.set_value(false);
  pros::delay(900);

  // //Fire triple stack
  chassis.set_turn_pid(-23, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-8, DRIVE_SPEED);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 127;
  pros::delay(160);
  intake = 0;
  pros::delay(700);
  intake = 127;
  pros::delay(160);
  intake = 0;
  pros::delay(700);
  intake = 127;
  pros::delay(160);
  intake = 0;
  // shotBlocker.set_value(true);
  // //Low goal discs
  // chassis.set_drive_pid(10, DRIVE_SPEED);
  // chassis.wait_drive();
  // chassis.set_turn_pid(10, TURN_SPEED);
  // chassis.wait_drive();
  // chassis.set_drive_pid(10, DRIVE_SPEED);
  // chassis.wait_drive();
  // chassis.set_turn_pid(-10, TURN_SPEED);
  // chassis.wait_drive();
  // intake = 127;
  // pros::delay(850);
  // intake = 0;
  // fly.notify();
}

void Skills() {
  skillsBool = true;
  tbh = 190.0;
  oneSpeed = false;
  skillSpeed = true;
  pros::Task fly = pros::Task(flywheel_task, (void *)1);
  chassis.set_drive_pid(3, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(250);
  // Fire Preloads
  chassis.set_drive_pid(-5, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(120, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(15, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::RIGHT_SWING, 90, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(11.5, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(200);
  chassis.set_swing_pid(ez::LEFT_SWING, 0, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-47, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(1200);
  intake = 0;
  // shotBlocker.set_value(true);
  // intake and shoot low goal
  // chassis.set_turn_pid(-90, TURN_SPEED);
  // chassis.wait_drive();
  // chassis.set_drive_pid(25, 90, true);
  // chassis.wait_drive();
  // chassis.set_drive_pid(-25, DRIVE_SPEED, true);
  // chassis.wait_drive();
  // chassis.set_turn_pid(0, TURN_SPEED);
  // chassis.wait_drive();
  // intake = 80;
  // pros::delay(1050);
  // intake = 0;
  // intake and shoot 3 disc in line
  chassis.set_drive_pid(34, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-129.5, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(56, 100, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-46, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(7, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(400);
  intake = 0;
  chassis.set_drive_pid(-5, DRIVE_SPEED, true);
  chassis.wait_drive();
  intake = 75;
  pros::delay(400);
  intake = 0;
  pros::delay(200);
  intake = 75;
  pros::delay(400);
  intake = 0;
  // shotBlocker.set_value(true);
  // intake and shoot 3 stack
  intakeLift.set_value(true);
  chassis.set_turn_pid(-137, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(22, DRIVE_SPEED, true);
  chassis.wait_drive();
  intakeLift.set_value(false);
  pros::delay(300);
  chassis.set_swing_pid(ez::LEFT_SWING, -129, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(34.75, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-177, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(9.25, DRIVE_SPEED);
  chassis.wait_drive();
  pros::delay(200);
  chassis.set_swing_pid(ez::RIGHT_SWING, -86, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-35.5, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(1200);
  intake = 0;
  // shotBlocker.set_value(true);
  // turn to next 3 stack and shoot and rollers and intake and shi
  chassis.set_drive_pid(1, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-68, TURN_SPEED);
  chassis.wait_drive();
  intakeLift.set_value(true);
  chassis.set_drive_pid(37, DRIVE_SPEED, true);
  chassis.wait_drive();
  intakeLift.set_value(false);
  pros::delay(300);
  chassis.set_drive_pid(8, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::RIGHT_SWING, -105, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(20.5, DRIVE_SPEED, true);
  chassis.wait_drive();
  pros::delay(200);
  chassis.set_swing_pid(ez::LEFT_SWING, -176, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-41, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(1200);
  intake = 0;
  // shotBlocker.set_value(true);
  // itnake and shoot 3 discs
  chassis.set_drive_pid(36, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-310, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(56, 100, true);
  chassis.wait_drive();
  chassis.set_turn_pid(-220, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(6, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(400);
  intake = 0;
  chassis.set_drive_pid(-6, DRIVE_SPEED, true);
  chassis.wait_drive();
  intake = 75;
  pros::delay(400);
  intake = 0;
  pros::delay(200);
  intake = 75;
  pros::delay(400);
  intake = 0;
  // shotBlocker.set_value(true);
  // 2nd to last 3 stack
  chassis.set_turn_pid(-311, TURN_SPEED);
  chassis.wait_drive();
  intakeLift.set_value(true);
  chassis.set_drive_pid(25, DRIVE_SPEED, true);
  chassis.wait_drive();
  intakeLift.set_value(false);
  pros::delay(300);
  chassis.set_drive_pid(37, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::RIGHT_SWING, -264, TURN_SPEED);
  chassis.wait_drive();
  chassis.set_drive_pid(-36, DRIVE_SPEED, true);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(1200);
  intake = 0;
  // shotBlocker.set_value(true);
  // last 3 stack
  chassis.set_drive_pid(1, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::LEFT_SWING, -243, TURN_SPEED);
  chassis.wait_drive();
  intakeLift.set_value(true);
  chassis.set_drive_pid(34, DRIVE_SPEED, true);
  chassis.wait_drive();
  intakeLift.set_value(false);
  pros::delay(900);
  chassis.set_drive_pid(-34, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::LEFT_SWING, -265, TURN_SPEED);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 75;
  pros::delay(1200);
  intake = 0;
  // shotBlocker.set_value(true);
  // last disc
  chassis.set_swing_pid(ez::LEFT_SWING, -227, TURN_SPEED);
  chassis.wait_drive();
  skillSpeed = false;
  oneSpeed = true;
  chassis.set_drive_pid(47, DRIVE_SPEED, true);
  chassis.wait_drive();
  chassis.set_swing_pid(ez::LEFT_SWING, -250, TURN_SPEED);
  chassis.wait_drive();
  // shotBlocker.set_value(false);
  intake = 127;
  pros::delay(200);
  intake = 0;
  // shotBlocker.set_value(true);
  chassis.set_swing_pid(ez::RIGHT_SWING, -130, DRIVE_SPEED);
  chassis.wait_drive();
  expansion1.set_value(true);
  chassis.set_drive_pid(-27, DRIVE_SPEED, true);
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
