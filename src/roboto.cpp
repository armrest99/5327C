#include "main.h"
#include "pros/adi.hpp"
#include <cmath>
#include <cstdlib>
#include <string>

const float PI = 3.1415926;


pros::Motor flywheel(6, pros::E_MOTOR_GEARSET_06, 0,
                     pros::E_MOTOR_ENCODER_COUNTS);
pros::Motor intake(1, pros::E_MOTOR_GEARSET_06, 0,
                   pros::E_MOTOR_ENCODER_COUNTS);
pros::ADIDigitalOut expansion( {{ 2, 'a'}});
pros::ADIDigitalOut expansion1( {{ 2, 'h'}});
// pros::ADIDigitalOut expansionBottom('A', false);
pros::ADIDigitalOut intakeLift( {{ 2, 'g'}});
pros::ADIDigitalOut actualIntakeLift({{2, 'd'}});
pros::ADIDigitalOut transmission( {{ 2, 'c'}});
pros::ADIAnalogIn sensor ('D');
// pros::Optical optical_sensor();



pros::Controller controller(pros::E_CONTROLLER_MASTER);

