#include "display/lv_core/lv_obj.h"
#include "display/lv_objx/lv_chart.h"
#include "main.h"

typedef struct {
  lv_obj_t *chart;
  lv_chart_series_t *series1;
  lv_chart_series_t *series2;
  pros::Motor *motor1;
  pros::Motor *motor2;
} flywheelGraphData;

void update(void *args);

flywheelGraphData setup_graph(pros::Motor *motor1, pros::Motor *motor2);
