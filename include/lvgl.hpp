#include "pros/apix.h"
static lv_res_t startAutonRoute(lv_obj_t *btn);
static lv_res_t openChecklist(lv_obj_t *btn);
static lv_res_t openCharts(lv_obj_t *btn);
static lv_res_t goBack(lv_obj_t *btn);

void initializeChartsSeries(std::vector<lv_obj_t *> &charts,
                            std::vector<std::string> &chartNames,
                            std::vector<lv_chart_series_t *> &series);

const char *convertDoubleToString(double value);

lv_obj_t *createHomeScreen(void);

void create_styles(void);

lv_obj_t *create_checklist();

// get rid of this
void lv_charts1(void);

void update_graph(const std::vector<double> &stats,
                  const std::vector<lv_obj_t *> &charts,
                  const std::vector<lv_chart_series_t *> &series);
