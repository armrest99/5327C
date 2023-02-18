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
pros::ADIDigitalOut expansion(2, 'H');
pros::ADIDigitalOut expansion1(2, 'C');
// pros::ADIDigitalOut expansionBottom('A', false);
pros::ADIDigitalOut intakeLift(2, 'G');
pros::ADIDigitalOut transmission(2, 'F');
pros::ADIAnalogIn sensor ('D');
// pros::Optical optical_sensor();



pros::Controller controller(pros::E_CONTROLLER_MASTER);

