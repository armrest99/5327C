#include "main.h"
#include <cmath>
#include <cstdlib>
#include <string>

const float PI = 3.1415926;


pros::Motor flywheel(16, pros::E_MOTOR_GEARSET_06, 1,
                     pros::E_MOTOR_ENCODER_COUNTS);
pros::Motor flywheel2(17, pros::E_MOTOR_GEARSET_06, 0,
                     pros::E_MOTOR_ENCODER_COUNTS);
pros::Motor indexer(13, pros::E_MOTOR_GEARSET_06, 1,
                     pros::E_MOTOR_ENCODER_COUNTS);
pros::Motor intake(12, pros::E_MOTOR_GEARSET_06, 0,
                   pros::E_MOTOR_ENCODER_COUNTS);
pros::ADIDigitalOut expansion('H', false);
pros::ADIDigitalOut expansion1('G', false);

pros::Controller controller(pros::E_CONTROLLER_MASTER);

