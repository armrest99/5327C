#include "roboto/debug.hpp"
#include "display/lv_core/lv_obj.h"
#include "display/lv_objx/lv_chart.h"
#include "main.h"
#include <cstddef>
#include <string>

void update(void *args) {
  flywheelGraphData *argsStruct = (flywheelGraphData *)args;
  pros::Motor flywheelMotor1 = *(argsStruct->flywheel);
  pros::Motor flywheelMotor2 = *(argsStruct->flywheel2);
  while (true) {
    double motorRPM1 = flywheelMotor1.get_actual_velocity();
    double motorRPM2 = flywheelMotor2.get_actual_velocity();
    printf("flywheel 1: %f, flywheel 2: %f\n", motorRPM1, motorRPM2);
    lv_chart_set_next(argsStruct->chart, argsStruct->series1, motorRPM1);
    lv_chart_set_next(argsStruct->chart, argsStruct->series2, motorRPM2);
    pros::delay(100);
  }
}

// todo: add pros task
flywheelGraphData setup_graph(pros::Motor *flywheel, pros::Motor *flywheel2) {
  // printf("Beginning setup_graph");
  lv_obj_t *chart = lv_chart_create(lv_scr_act(), nullptr);
  lv_obj_set_size(chart, 500, 200);
  lv_obj_align(chart, nullptr, LV_ALIGN_CENTER, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series1 = lv_chart_add_series(chart, LV_COLOR_BLUE);
  lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);

  lv_chart_set_range(chart, 0, 650);
  return flywheelGraphData{chart, series1, series2, flywheel, flywheel2};
}
