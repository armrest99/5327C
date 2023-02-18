// #include "debug.hpp"
#include "display/lv_core/lv_obj.h"
#include "display/lv_objx/lv_btn.h"
#include "main.h"
// #include <format>
#include <string>

#ifndef DEBUG
#define DEBUG
#endif

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    print("Clicked\n");
  } else if (event == LV_EVENT_VALUE_CHANGED) {
    print("Toggled\n");
  }
}

// auton selector
void auton_selector() {
  lv_obj_t *label;
  lv_obj_t *btn1 = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_set_event_cb(btn1, event_handler);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -40);

  label = lv_label_create();
}

void motor_check_overheat() {
  // left
  pros::Motor_Group left_wheels({1, 12, 11});
  pros::Motor_Group right_wheels({14, 19, 13});
  std::vector<std::int32_t> left_temps;
  std::vector<std::int32_t> right_temps;
  std::array<lv_obj_t *, 3> left_buttons;
  for (std::size_t i = 0; i < left_temps.size(); i++) {
    left_buttons[i] = lv_label_create(lv_scr_act(), NULL);
  }
  std::array<lv_obj_t *, 3> right_buttons;
  for (std::size_t i = 0; i < left_temps.size(); i++) {
    right_buttons[i] = lv_label_create(lv_scr_act(), NULL);
  }

  // this stuff goes into opcontrol
  while (true) {
    left_temps = left_wheels.are_over_temp();
    right_temps = right_wheels.are_over_temp();
    for (std::size_t i = 0; i < left_temps.size(); i++) {
      if (left_temps[i] == 0) {
        lv_label_set_text(left_temps[i], "motor " + std::to_string(i) + ": ok");
      } else if (left_temps[i] == 1) {
        lv_label_set_text(left_temps[i], "#ff0000 motor " + std::to_string(i) +
                                             " overheating!#");
      }
    }
    // definitely not duplication of code
    for (std::size_t i = 0; i < left_temps.size(); i++) {
      if (left_temps[i] == 0) {
        lv_label_set_text(left_temps[i], "motor " + std::to_string(i) + ": ok");
      } else if (left_temps[i] == 1) {
        lv_label_set_text(left_temps[i], "#ff0000 motor" + std::to_string(i) +
                                             ": overheating!#");
      }
    }
  }
}

// checklist
// void lv_ex_checkbox_1(void) {
//   // list of strings for checklist
//   // pain
//   vector<string> checklist = {"do stuff"};
// }
