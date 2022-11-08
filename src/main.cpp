#include "main.h"
#include "autons.hpp"
#include "roboto/roboto.hpp"

/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////


// Chassis constructor
Drive chassis (
  // Left Chassis Ports (negative port will reverse it!)
  //   the first port is the sensored port (when trackers are not used!)
  {-8, -14}
  // Right Chassis Ports (negative port will reverse it!)
  //   the first port is the sensored port (when trackers are not used!)
  ,{9, 6}

  // IMU Port
  ,21

  // Wheel Diameter (Remember, 4" wheels are actually 4.125!)
  //    (or tracking wheel diameter)
  ,4

  // Cartridge RPM
  //   (or tick per rotation if using tracking wheels)
  ,600

  // External Gear Ratio (MUST BE DECIMAL)
  //    (or gear ratio of tracking wheel)
  // eg. if your drive is 84:36 where the 36t is powered, your RATIO would be 2.333.
  // eg. if your drive is 36:60 where the 60t is powered, your RATIO would be 0.6.
  ,2.333

  // Uncomment if using tracking wheels
  /*
  // Left Tracking Wheel Ports (negative port will reverse it!)
  // ,{1, 2} // 3 wire encoder
  // ,8 // Rotation sensor
  // Right Tracking Wheel Ports (negative port will reverse it!)
  // ,{-3, -4} // 3 wire encoder
  // ,-9 // Rotation sensor
  */

  // Uncomment if tracking wheels are plugged into a 3 wire expander
  // 3 Wire Port Expander Smart Port
  // ,1
);
int flySpeed = 0;
int inSpeed = 0;
int indexSpeed = 0;
bool goingDown = false;
bool r1Engaged = false;
bool r2Engaged = false;
bool l1Engaged = false;
bool l2Engaged = false;
bool aEngaged = false;
bool tbhToggle = true;
int angleOffset;
double tbhDrive = 0.0;
double prev_errorDrive = 0.0;
double flyDriveD = 0.0;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  // Print our branding over your terminal :D
  ez::print_ez_template();

  pros::delay(500); // Stop the user from doing anything while legacy ports configure.

  // Configure your chassis controls
  chassis.toggle_modify_curve_with_controller(true); // Enables modifying the controller curve with buttons on the joysticks
  chassis.set_active_brake(0); // Sets the active brake kP. We recommend 0.1.
  chassis.set_curve_default(0, 0); // Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)
  void default_constants(); // Set the drive to your own constants from autons.cpp!
  void exit_condition_defaults(); // Set the exit conditions to your own constants from autons.cpp!

  // These are already defaulted to these buttons, but you can change the left/right curve buttons here!
  // chassis.set_left_curve_buttons (pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT); // If using tank, only the left side is used.
  // chassis.set_right_curve_buttons(pros::E_CONTROLLER_DIGITAL_Y,    pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  // ez::as::auton_selector.add_autons({
  //   Auton("Example Drive\n\nDrive forward and come back.", drive_example),

  // Initialize chassis and auton selector
  chassis.initialize();
  //ez::as::initialize();
}


/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}



/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
  // . . .
}
void runFlywheelDrive(double velocity) {
  flywheel.move_voltage(velocity);
  flywheel2.move_voltage(velocity);
}
void autoFlywheelDrive(double velocity) {
    //double velocity = *velo;
    //runFlywheel(velocity);
    double change = .25;
    double currentVelo = flywheel.get_actual_velocity(); 
    double error = velocity - currentVelo; 
    double output = flyDriveD + change * error;
    if (error * prev_errorDrive < 0) {
        output = 0.5 * (output + tbhDrive);
        tbhDrive = output;
    }
    controller.print(1, 1, "flywheelspeed %f", currentVelo);
    runFlywheelDrive(output);
    flyDriveD = output;
    prev_errorDrive = error;
}
void toggleIntake() {
	if(inSpeed == 0){
		inSpeed = 1;
		intake = -127;
	}
	else if(inSpeed == 1 && goingDown == false){
		inSpeed = 2;
		intake = 0;
	}
	else if(inSpeed == 2){
		inSpeed = 1;
		goingDown = true;
		intake = 127;
	}
	else if(inSpeed == 1 && goingDown == true){
		inSpeed = 0;
		goingDown = false;
		intake = 0;
	}
}
void toggleIndexer() {
	if(indexSpeed == 0){
		indexer.move_relative(1750,600);
	}
}
// void flywheel_taskDrive(void* param){
//   double *v = (double*) param;
//   bool twoSpeed = true;
//   //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
//   while(true){//pros::Task::notify_take(true, TIMEOUT_MAX)==1){//pros::Task::notify_take(true, TIMEOUT_MAX)){
//     //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false, TIMEOUT_MAX)));
//     if (twoSpeed) {
//       pros::lcd::set_text(2,"a");
//       autoFlywheelDrive(550);
//     }
//     else if (!twoSpeed) {
//       pros::lcd::set_text(2,"b");
//       autoFlywheelDrive(50);
//     }
//     //autoFlywheel(v);
//   }
// }
/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  pros::lcd::initialize();
  chassis.reset_pid_targets(); // Resets PID targets to 0
  chassis.reset_gyro(); // Reset gyro position to 0
  chassis.reset_drive_sensor(); // Reset drive sensors to 0
  chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps autonomous consistency.

  leftAuton(); // Calls selected auton from autonomous selector.
}




/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  // This is preference to what you like to drive on.
  chassis.set_drive_brake(MOTOR_BRAKE_COAST);
  // if (tbhToggle){
  //   pros::Task fly = pros::Task(flywheel_taskDrive, (void*) 1);
  // }
  bool l2;
  bool buttonB;
  while (true) {
    buttonB = controller.get_digital(pros::E_CONTROLLER_DIGITAL_B);
    l2 = controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2);
    autoFlywheelDrive(325);
    controller.print(1, 1, "speed %f", flywheel.get_actual_velocity());
    
    chassis.tank(); // Tank control

    if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1) && l1Engaged == false){
			toggleIntake();
			l1Engaged = true;
		}
    
    else if (!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)){
			l1Engaged = false;
		}

    if(l2){
      intake = -35;
    }

    if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && r1Engaged == false){
			toggleIndexer();
			r1Engaged = true;
		}
    else if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)){
			r1Engaged = false;
		}
    if (buttonB) {
      expansion.set_value(true);
      expansion1.set_value(true);
    }



    // chassis.arcade_standard(ez::SPLIT); // Standard split arcade
    // chassis.arcade_standard(ez::SINGLE); // Standard single arcade
    // chassis.arcade_flipped(ez::SPLIT); // Flipped split arcade
    // chassis.arcade_flipped(ez::SINGLE); // Flipped single arcade

    // . . .
    // Put more user control code here!
    // . . .

    pros::delay(ez::util::DELAY_TIME); // This is used for timer calculations!  Keep this ez::util::DELAY_TIME
  }
}
