#ifndef ROBOTO_H_
#define ROBOTO_H_

#include "main.h"
#include <iostream>

extern pros::Controller controller;
extern pros::Motor flywheel;
extern pros::Motor flywheel2;
extern pros::Motor indexer;
extern pros::Motor intake;
extern pros::ADIDigitalOut expansion;
extern pros::ADIDigitalOut expansion1;
extern pros::ADIDigitalOut expansionBottom;
extern pros::ADIDigitalOut intakeLift;
extern pros::ADIDigitalOut actualIntakeLift;
extern pros::ADIDigitalOut transmission;
extern pros::ADIAnalogIn sensor;
extern float currentX;
extern float currentY;
extern float pastFwd;
extern float pastLeftRight;
extern bool isAuton;
extern pros::Optical optical_sensor; 

extern void reset();
extern void driveStop();
extern void move(float voltage);
extern void rotate(float angle);
extern void drive(float targetX, float targetY, float targetAngle);
extern void rotateLeft(float targetAngle);
extern void rotateRight(float targetAngle);
extern void rotateLeftAbsolute(float turnAngle);
extern void rotateRightAbsolute(float turnAngle);


#endif
