#include "main.h"
#include "autons.hpp"
#include "roboto/roboto.hpp"
#define LEFT_WHEEL_FRONT_PORT 16
#define LEFT_WHEEL_BACK_PORT 17
#define LEFT_WHEEL_MIDDLE_PORT 18

#define RIGHT_WHEEL_FRONT_PORT 13
#define RIGHT_WHEEL_BACK_PORT 15
#define RIGHT_WHEEL_MIDDLE_PORT 14
#define PI 3.1415926
/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////

// Chassis constructor
// note: this is only for auton
Drive chassis(
    // Left Chassis Ports (negative port will reverse it!)
    //   the first port is the sensored port (when trackers are not used!)
    {16, -17, 18}
    // Right Chassis Ports (negative port will reverse it!)
    //   the first port is the sensored port (when trackers are not used!)
    ,{-13, 14, -15}

    // IMU Port
    ,3

    // Wheel Diameter (Remember, 4" wheels are actually 4.125!)
    //    (or tracking wheel diameter)
    ,4

    // Cartridge RPM
    //   (or tick per rotation if using tracking wheels)
    ,600

    // External Gear Ratio (MUST BE DECIMAL)
    //    (or gear ratio of tracking wheel)
    // eg. if your drive is 84:36 where the 36t is powered, your RATIO would
    // be 2.333. eg. if your drive is 36:60 where the 60t is powered, your RATIO
    // would be 0.6.
    ,2

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
double tbhDrive = 190.0;
double prev_errorDrive = 0.0;
double flyDriveD = 0.0;
int weeds;

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
lv_obj_t *chart = lv_chart_create(lv_scr_act(), nullptr);
lv_chart_series_t *series1 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);
void initialize() {
  // Print our branding over your terminal :D
  lv_obj_set_size(chart, 500, 200);
  lv_obj_align(chart, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

  // lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);

  lv_chart_set_range(chart, 350, 450);
  lv_chart_set_point_count(chart, 1000);
  // ez::print_ez_template();

  pros::delay(500); // Stop the user from doing anything while legacy ports configure.

  // Configure your chassis controls
  // chassis.toggle_modify_curve_with_controller(true); // Enables modifying the
  // controller curve with buttons on the joysticks
  // // chassis.set_active_brake(0.1); // Sets the active brake kP. We recommend
  // 0.1. chassis.set_curve_default(0, 0); // Defaults for curve. If using tank,
  // only the first parameter is used. (Comment this line out if you have an SD
  // card!)
  void default_constants(); // Set the drive to your own constants from autons.cpp!
  void exit_condition_defaults(); // Set the exit conditions to your own
                                  // constants from autons.cpp!

  // These are already defaulted to these buttons, but you can change the
  // left/right curve buttons here! chassis.set_left_curve_buttons
  // (pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT); // If
  // using tank, only the left side is used.
  // chassis.set_right_curve_buttons(pros::E_CONTROLLER_DIGITAL_Y,
  // pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  // ez::as::auton_selector.add_autons({
  //   Auton("Example Drive\n\nDrive forward and come back.", drive_example),

  // Initialize chassis and auton selector
  // chassis.initialize();
  // ez::as::initialize();
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
  initialize(); 
}
void runFlywheelDrive(double velocity) { 
  flywheel.move_voltage(velocity); 
}
void autoFlywheelDrive(double velocity) {
  // double velocity = *velo;
  // runFlywheel(velocity);

  double change = .25;
  double currentVelo = flywheel.get_actual_velocity();
  double error = velocity - currentVelo;
  double output = flyDriveD + change * error;
  if (error * prev_errorDrive < 0) {
    output = 0.5 * (output + tbhDrive);
    tbhDrive = output;
  }
  lv_chart_set_next(chart, series1, flywheel.get_actual_velocity());
  lv_chart_set_next(chart, series2, output);
  controller.print(1, 1, "flywheelspeed %f", currentVelo);
  runFlywheelDrive(output);
  flyDriveD = output;
  prev_errorDrive = error;
}

void toggleIntake() {
  if (inSpeed == 0) {
    inSpeed = 1;
    intake.move_velocity(-600);
  } else if (inSpeed == 1 && goingDown == false) {
    inSpeed = 2;
    intake = 0;
  } else if (inSpeed == 2) {
    inSpeed = 1;
    goingDown = true;
    intake.move_velocity(600);
  } else if (inSpeed == 1 && goingDown == true) {
    inSpeed = 0;
    goingDown = false;
    intake = 0;
  }
}

// void toggleIndexer() {
// 	if(indexSpeed == 0){
// 		indexer.move_velocity(125);
//     pros::delay(310);
//     indexer = 0;
//     flywheel = 100;
//     flywheel2 = 100;
//     pros::delay(100);
//     indexer.move_velocity(125);
//     pros::delay(600);
//     indexer = 0;

// 	}
// }
// void toggleIndexerFar() {
// 		indexer.move_velocity(125);
//     pros::delay(300);
//     indexer = 0;
//     pros::delay(1400);
//     indexer.move_velocity(125);
//     pros::delay(300);
//     indexer = 0;
//     pros::delay(1400);
//     indexer.move_velocity(125);
//     pros::delay(300);
//     indexer = 0;
// }
// void flywheel_taskDrive(void* param){
//   double *v = (double*) param;
//   bool twoSpeed = true;
//   //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false,
//   TIMEOUT_MAX))); while(true){//pros::Task::notify_take(true,
//   TIMEOUT_MAX)==1){//pros::Task::notify_take(true, TIMEOUT_MAX)){
//     //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false,
//     TIMEOUT_MAX))); if (twoSpeed) {
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
  chassis.reset_pid_targets();               // Resets PID targets to 0
  chassis.reset_gyro();                      // Reset gyro position to 0
  chassis.reset_drive_sensor();              // Reset drive sensors to 0
  chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps
                                             // autonomous consistency.

  // Skills(); // Calls selected auton from autonomous selector.
  // left_awp();
  leftAuton();
  // rightAuton();
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
  // if (tbhToggle){
  //   pros::Task fly = pros::Task(flywheel_taskDrive, (void*) 1);
  // }
  // flywheelGraphData *argsStruct = (flywheelGraphData *)args;
  bool l2;
  bool buttonB;
  pros::Motor left_wheel_front(LEFT_WHEEL_FRONT_PORT);
  pros::Motor left_wheel_back(LEFT_WHEEL_BACK_PORT);
  pros::Motor left_wheel_middle(LEFT_WHEEL_MIDDLE_PORT);

  pros::Motor right_wheel_front(RIGHT_WHEEL_FRONT_PORT);
  pros::Motor right_wheel_back(RIGHT_WHEEL_BACK_PORT);
  pros::Motor right_wheel_middle(RIGHT_WHEEL_MIDDLE_PORT);
  while (true) {
    intakeLift.set_value(true);
    expansion1.set_value(false);
    expansion.set_value(false);
    chassis.set_drive_brake(MOTOR_BRAKE_COAST);
    float up_down = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    float left_right = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    float turnX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    float turnY = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
    transmission.set_value(true);

    autoFlywheelDrive(540);
    // pros::delay(100);
    isAuton = false;
    buttonB = controller.get_digital(pros::E_CONTROLLER_DIGITAL_B);
    l2 = controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2);

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
      transmission.set_value(false);
      left_wheel_front.move(-up_down);
      left_wheel_middle.move(-up_down);
      left_wheel_back.move(-up_down);

      right_wheel_front.move(-turnY);
      right_wheel_middle.move(-turnY);
      right_wheel_back.move(-turnY);
    } 
      if (!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
      transmission.set_value(true);
      left_wheel_front.move(up_down);
      left_wheel_middle.move(up_down);
      left_wheel_back.move(up_down);

      right_wheel_front.move(turnY);
      right_wheel_middle.move(turnY);
      right_wheel_back.move(turnY);
    }

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && r1Engaged == false) {
      toggleIntake();
      r1Engaged = true;
    } 
    else if (!controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)){
      r1Engaged = false;
    }
    // if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_A) && aEngaged ==
    // false){ 	aEngaged = true;
    // }
    // else if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)){
    // 	aEngaged = false;
    // }
    // if (buttonB) {
    //   expansionBottom.set_value(true);
    // }
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)) {
      expansion.set_value(true);
      // expansionBottom.set_value(true);
    }
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_LEFT)) {
      expansion1.set_value(true);
      // expansionBottom.set_value(true);
    }

    // chassis.arcade_standard(ez::SPLIT); // Standard split arcade
    // chassis.arcade_standard(ez::SINGLE); // Standard single arcade
    // chassis.arcade_flipped(ez::SPLIT); // Flipped split arcade
    // chassis.arcade_flipped(ez::SINGLE); // Flipped single arcade

    // . . .
    // Put more user control code here!
    // . . .

    // Tank control

    pros::delay(ez::util::DELAY_TIME); // This is used for timer calculations!
                                       // Keep this ez::util::DELAY_TIME
  }
}
