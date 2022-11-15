#include "display/lv_core/lv_obj.h"
#include "display/lv_objx/lv_chart.h"
#include "main.h"

typedef struct {
  lv_obj_t *chart;
  
  pros::Motor *motor1;
  pros::Motor *motor2;
} flywheelGraphData;

flywheelGraphData setup_graph(pros::Motor *motor1, pros::Motor *motor2);
