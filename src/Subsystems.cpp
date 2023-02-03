#include "EZ-Template/util.hpp"
#include "Subsystems.hpp"
#include "pros/adi.hpp"
#include "pros/misc.h"
#include "pros/motors.h"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
pros::Motor catapult(10, pros::E_MOTOR_GEARSET_36, true, pros::E_MOTOR_ENCODER_DEGREES);
pros::Motor Intake(1, pros::E_MOTOR_GEARSET_06, false, pros::E_MOTOR_ENCODER_DEGREES);
pros::ADIDigitalIn catalimit('H');
pros::ADIDigitalOut endgamerelease ('G');
bool endgametoggle = false;
bool intakeToggle = false;
bool cata_override = false;
bool state = true;

void Catacontrol(){
    if (catalimit.get_value() == true && !master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
        catapult.move_voltage(0);
    }else if (catalimit.get_value() == true && master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
        catapult.move_voltage(12000);
    }else if (catalimit.get_value() == false) {
        catapult.move_voltage(12000);
        Intake.move_voltage(0);
    }
}
void intakeControl(){
if (master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1)) {
    intakeToggle = !intakeToggle;
}else if (intakeToggle == true && !master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && state == true) {
Intake.move_voltage(12000);
}else if (intakeToggle == false) {
Intake.move_voltage(0);
}else if (intakeToggle == true && master.get_digital(pros::E_CONTROLLER_DIGITAL_L2) && state == true) {
    Intake.move_voltage(-12000);
}else if(intakeToggle == true && state == false){
    Intake.move_voltage(0);
}
}
void endgame(){
    if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)){
        endgamerelease.set_value(true);
    }
}

void autoncatashoot(){
    while (true) {
    if(catalimit.get_value() == true){
        catapult.move_velocity(12000);
    }else if(catalimit.get_value() == false) {
    catapult.move_velocity(0);
    break;
    }
    }
}
void autoncataprime(){
    while (true) {
    if(catalimit.get_value() == true){
        catapult.move_velocity(0);
        break;
    }else if(catalimit.get_value() == false) {
    catapult.move_velocity(12000);
    }
    }
}
void autoncata(){
    while (true) {
     autoncatashoot();
    pros::delay(100);
    autoncataprime();
    break;
    }
}
void setIntake(int speed){
    Intake.move_voltage(speed);
}

void cata_task_fn() {
  
  while (true) {

    if ((catalimit.get_value() == false) && (state == false)) {
      // move catapult down until its reached loading position
      catapult = 127;

    } else if (!cata_override && catalimit.get_value()) {
      catapult = 0;
      state = true;
    }

    pros::delay(10);
  }
}

void fire() {
  cata_override = true;
  catapult = 127;
  pros::delay(500);
  cata_override = false;
  state = false;
}

void drivercata(){
    if (master.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
    fire();
    }
}
