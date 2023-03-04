#include "lvgl.hpp"
#include "main.h"
#include "pros/apix.h"

static lv_obj_t *checklistPage = create_checklist();
static lv_obj_t *chartPage = createHomeScreen();

void initializeChartsSeries(std::vector<lv_obj_t *> &charts,
                            std::vector<string> &chartNames,
                            std::vector<lv_chart_series_t *> &series) {
  for (int i = 0; i < 8; i++) {
    lv_obj_t *chart = lv_chart_create(chartPage, NULL);
    lv_obj_set_size(chart, 100, 100);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_range(chart, 1, 600);
    lv_chart_set_point_count(chart, 100);
    lv_chart_series_t *series1 = lv_chart_add_series(chart, LV_COLOR_BLUE);
    lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);

    const char *chart_name = chartNames.at(i).c_str();
    lv_obj_t *label = lv_label_create(chart, NULL);
    lv_label_set_text(label, chart_name);

    lv_obj_align(chart, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    charts.push_back(chart);
    series.push_back(series1);
    series.push_back(series2);
  }
  static lv_style_t style1;
  static lv_style_t style2;
  style1.body.main_color = LV_COLOR_BLACK;    /*White main color*/
  style1.body.grad_color = LV_COLOR_RED;      /*Blue gradient color*/
  style1.body.shadow.color = LV_COLOR_SILVER; /*Light gray shadow color*/
  style1.body.shadow.width = 8;               /*8 px shadow*/
  style1.body.border.width = 0;               /*2 px border width*/
  style1.text.color = LV_COLOR_BLUE;          /*Red text color */
  style1.text.letter_space = 1;
  lv_obj_t *rpmLabel = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(rpmLabel, "RPM");
  lv_obj_set_x(rpmLabel, 80);
  lv_obj_set_y(rpmLabel, 20);
  lv_obj_set_style(rpmLabel, &style1);

  style2.body.main_color = LV_COLOR_BLACK;    /*White main color*/
  style2.body.grad_color = LV_COLOR_RED;      /*Blue gradient color*/
  style2.body.shadow.color = LV_COLOR_SILVER; /*Light gray shadow color*/
  style2.body.shadow.width = 8;               /*8 px shadow*/
  style2.body.border.width = 0;               /*2 px border width*/
  style2.text.color = LV_COLOR_RED;           /*Red text color */
  style2.text.letter_space = 1;
  lv_obj_t *scr = lv_page_create(NULL, NULL);
  lv_scr_load(scr);
  lv_obj_set_style(scr, &style2);
  lv_obj_t *wattageLabel = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(wattageLabel, "Wattage");
  lv_obj_set_y(wattageLabel, 0);
}

const char *convertDoubleToString(double value) {
  std::stringstream ss;
  ss << value;
  const char *str = ss.str().c_str();
  return str;
}

lv_obj_t *createHomeScreen() {
  static lv_obj_t *homePage = lv_page_create(NULL, NULL);
  lv_scr_load(homePage);
  std::vector<std::string> temperatureLabels = {"Intake (1)",
                                                "Flywheel (6)",
                                                "Left Wheel Front (16)",
                                                "Left Wheel Middle (17)",
                                                "Left Wheel Back (18)",
                                                "Right Wheel Front (13)",
                                                "Right Wheel Middle (14)",
                                                "Right Wheel Back (15)"};

  // Temperature label sections

  lv_obj_t *temperatureHeaderLabel = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(temperatureHeaderLabel, "Temperature");
  lv_obj_set_x(temperatureHeaderLabel, 0);
  lv_obj_set_y(temperatureHeaderLabel, 260);
  // initialize temperature labels
  for (int i = 0; i < temperatureLabels.size(); i++) {
    const char *label_c_string = temperatureLabels[i].c_str();
    std::size_t y_coord = 280 + i * 20;
    lv_obj_t *mechanism_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(mechanism_label, label_c_string);
    // lv_obj_set_style(mechanism_label, &style_new);
    lv_obj_set_x(mechanism_label, 220);
    lv_obj_set_y(mechanism_label, y_coord);

    lv_obj_t *temperature_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_x(temperature_label, 280);
    lv_obj_set_y(temperature_label, y_coord);
    // double temperature = temperatures.at(i);
    // lv_label_set_text(temperature_label, convertDoubleToString(temperature));
  }

  std::vector<std::string> buttonNames = {
      "Left Auton 1",  "Solo AWP",  "Left Auton 2",       "Right Auton 1",
      "Right Auton 2", "Checklist", "Watt and RPM Charts"};
  // vector of functions
  lv_res_t (*callbacks[7])(lv_obj_t * button) = {
      &startAutonRoute, &startAutonRoute, &startAutonRoute, &startAutonRoute,
      &startAutonRoute, &openCharts,      &openCharts};
  for (int i = 0; i < buttonNames.size(); i++) {
    const char *label_c_string = buttonNames.at(i).c_str();
    lv_obj_t *button = lv_btn_create(homePage, NULL);
    // lv_obj_set_style(button, &style_new);
    lv_btn_set_action(button, LV_BTN_ACTION_CLICK, *callbacks[i]);

    // set x and y coordinates

    lv_obj_t *label = lv_label_create(button, NULL);
    lv_label_set_text(label, label_c_string);
  }
  return homePage;
}

void create_styles(void) {
  static lv_style_t checklistStyle; /*Styles can't be local variables*/
  /*Copy a built-in style as a starting point*/    /*Fully round corners*/
  checklistStyle.body.main_color = LV_COLOR_WHITE; /*White main color*/
  checklistStyle.body.grad_color = LV_COLOR_WHITE; /*Blue gradient color*/
  checklistStyle.body.shadow.color =
      LV_COLOR_SILVER;                       /*Light gray shadow color*/
  checklistStyle.body.shadow.width = 8;      /*8 px shadow*/
  checklistStyle.body.border.width = 0;      /*2 px border width*/
  checklistStyle.text.color = LV_COLOR_TEAL; /*Red text color */
  checklistStyle.text.letter_space = 0;

  static lv_style_t prettyStyle;
  prettyStyle.body.main_color = LV_COLOR_BLACK;    /*White main color*/
  prettyStyle.body.grad_color = LV_COLOR_BLUE;     /*Blue gradient color*/
  prettyStyle.body.shadow.color = LV_COLOR_SILVER; /*Light gray shadow color*/
  prettyStyle.body.shadow.width = 8;               /*8 px shadow*/
  prettyStyle.body.border.width = 0;               /*2 px border width*/
  prettyStyle.text.color = LV_COLOR_WHITE;         /*Red text color */
  prettyStyle.text.letter_space = 1;

  // style_new.body.main_color = LV_COLOR_BLACK;    /*White main color*/
  // style_new.body.grad_color = LV_COLOR_RED;      /*Blue gradient color*/
  // style_new.body.shadow.color = LV_COLOR_SILVER; /*Light gray shadow color*/
  // style_new.body.shadow.width = 8;               /*8 px shadow*/
  // style_new.body.border.width = 0;               /*2 px border width*/
  // style_new.text.color = LV_COLOR_WHITE;         /*Red text color */
  // style_new.text.letter_space = 1;
}

lv_obj_t *create_checklist() {
  static lv_obj_t *checklistPage = lv_page_create(NULL, NULL);
  std::vector<std::string> checklistItems = {"Pneumatics asf", "Wires asf"};
  // lv_obj_set_style(checklistPage, &checklistStyle);
  for (int i = 0; i < checklistItems.size(); i++) {
    lv_obj_t *button = lv_btn_create(checklistPage, NULL);
    lv_obj_set_size(button, 200, 30);
    lv_obj_set_x(button, 10);
    lv_obj_set_y(button, 20 * (i + 1));
    lv_btn_set_action(button, LV_BTN_ACTION_CLICK, NULL);

    const char *button_text = checklistItems.at(i).c_str();
    lv_obj_t *label = lv_label_create(button, NULL);
    lv_label_set_text(label, button_text);
  }
  lv_obj_t *backButton = lv_btn_create(lv_scr_act(), NULL);
  lv_btn_set_action(
      backButton, LV_BTN_ACTION_CLICK,
      goBack); /*Set function to be called when the button is released*/
  lv_obj_set_style(backButton, &lv_style_pretty);
  lv_obj_set_size(backButton, 200, 30);
  lv_obj_t *backLabel = lv_label_create(backButton, NULL);
  lv_label_set_text(backLabel, "Back");
  lv_obj_set_x(backButton, 330);
  lv_btn_set_fit(backButton, true, true);
  return checklistPage;
}

void update_graph(const std::vector<double> &stats,
                  const std::vector<lv_obj_t *> &charts,
                  const std::vector<lv_chart_series_t *> &series) {
  // currently assumes 2 series per chart
  int current_series = 0;
  for (int i = 0; i < charts.size(); i++) {
    lv_chart_set_next(charts.at(i), series.at(current_series),
                      stats.at(current_series));
    current_series += 1;
    lv_chart_set_next(charts.at(i), series.at(current_series),
                      stats.at(current_series));
    current_series += 1;
  }
}

static lv_res_t startAutonRoute(lv_obj_t *btn) {
  // currently does nothing
  return LV_RES_OK;
}
static lv_res_t openChecklist(lv_obj_t *btn) {
  lv_scr_load(checklistPage);
  // lv_checklist();
  return LV_RES_OK;
}
static lv_res_t openCharts(lv_obj_t *btn) {
  lv_scr_load(chartPage);
  return LV_RES_OK;
}
static lv_res_t goBack(lv_obj_t *btn) { return LV_RES_OK; }
