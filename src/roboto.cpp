#include "main.h"
#include "pros/adi.hpp"
#include <cmath>
#include <cstdlib>
#include <string>
#define LEFT_WHEEL_FRONT_PORT 16
#define LEFT_WHEEL_BACK_PORT 17
#define LEFT_WHEEL_MIDDLE_PORT 18

#define RIGHT_WHEEL_FRONT_PORT 13
#define RIGHT_WHEEL_BACK_PORT 15
#define RIGHT_WHEEL_MIDDLE_PORT 14
#define LEFT_ROT_PORT 1
#define LEFT_ROT_PORT2 2
#define BACK_ROT_PORT 3
#define BACK_ROT_PORT2 4
const float PI = 3.1415926;

pros::Motor left_wheel_front(LEFT_WHEEL_FRONT_PORT);
pros::Motor left_wheel_back(LEFT_WHEEL_BACK_PORT);
pros::Motor left_wheel_middle(LEFT_WHEEL_MIDDLE_PORT);

pros::Motor right_wheel_front(RIGHT_WHEEL_FRONT_PORT);
pros::Motor right_wheel_back(RIGHT_WHEEL_BACK_PORT);
pros::Motor right_wheel_middle(RIGHT_WHEEL_MIDDLE_PORT);
pros::Imu inertial(3);
pros::Optical optical_sensor(4);
pros::ADIEncoder left_rot(LEFT_ROT_PORT, LEFT_ROT_PORT2, false);
pros::ADIEncoder back_rot(BACK_ROT_PORT, BACK_ROT_PORT2, false);
pros::Motor flywheel(6, pros::E_MOTOR_GEARSET_06, 1, 
                    pros::E_MOTOR_ENCODER_COUNTS);
pros::Motor intake(20, pros::E_MOTOR_GEARSET_06, 0,
                   pros::E_MOTOR_ENCODER_COUNTS);
pros::ADIDigitalOut expansion( {{ 2, 'g'}});
pros::ADIDigitalOut expansion1( {{ 2, 'h'}});
// pros::ADIDigitalOut expansionBottom('A', false);
pros::ADIDigitalOut angleChanger( {{ 2, 'a'}});
pros::ADIDigitalOut actualIntakeLift('E', false);
pros::ADIDigitalOut transmission( {{ 2, 'c'}});
pros::ADIAnalogIn sensor ('D');
// pros::Optical optical_sensor();



pros::Controller controller(pros::E_CONTROLLER_MASTER);

