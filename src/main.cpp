#include "main.h"
#include "autons.hpp"
#include "roboto/roboto.hpp"
#include "display/lvgl.h"
#include "pros/apix.h"
#include "display/lv_themes/lv_theme_alien.h"
// #include <format>
#include <string>
#include <map>

#ifndef DEBUG
#define DEBUG
#endif


/////
// For instalattion, upgrading, documentations and tutorials, check out website!
// https://ez-robotics.github.io/EZ-Template/
/////

// Chassis constructor
// note: this is only for auton
Drive chassis(
    // Left Chassis Ports (negative port will reverse it!)
    //   the first port is the sensored port (when trackers are not used!)
    {16, -17, 18}
    // Right Chassis Ports (negative port will reverse it!)
    //   the first port is the sensored port (when trackers are not used!)
    ,{-13, 14, -15}

    // IMU Port
    ,3

    // Wheel Diameter (Remember, 4" wheels are actually 4.125!)
    //    (or tracking wheel diameter)
    ,4

    // Cartridge RPM
    //   (or tick per rotation if using tracking wheels)
    ,600

    // External Gear Ratio (MUST BE DECIMAL)
    //    (or gear ratio of tracking wheel)
    // eg. if your drive is 84:36 where the 36t is powered, your RATIO would
    // be 2.333. eg. if your drive is 36:60 where the 60t is powered, your RATIO
    // would be 0.6.
    ,2

    // Uncomment if using tracking wheels
    /*
    // Left Tracking Wheel Ports (negative port will reverse it!)
    // ,{1, 2} // 3 wire encoder
    // ,8 // Rotation sensor
    // Right Tracking Wheel Ports (negative port will reverse it!)
    // ,{-3, -4} // 3 wire encoder
    // ,-9 // Rotation sensor
    */

    // Uncomment if tracking wheels are plugged into a 3 wire expander
    // 3 Wire Port Expander Smart Port
    // ,1
);
static  lv_res_t leftAutonVariable(lv_obj_t * btn);
static  lv_res_t checklistVar(lv_obj_t * btn);
static  lv_res_t chartVar(lv_obj_t * btn);
static  lv_res_t back(lv_obj_t * btn);


//Needed Variables
double temp = flywheel.get_temperature();
double templf = left_wheel_front.get_temperature();
double templm = left_wheel_middle.get_temperature();
double templb = left_wheel_back.get_temperature();
double temprf = right_wheel_front.get_temperature();
double temprb = right_wheel_back.get_temperature();
double temprm = right_wheel_middle.get_temperature();
double voltint = intake.get_voltage();
double volt = flywheel.get_voltage();
double voltlf = left_wheel_front.get_voltage();
double voltlm = left_wheel_middle.get_voltage();
double voltlb = left_wheel_back.get_voltage();
double voltrf = right_wheel_front.get_voltage();
double voltrb = right_wheel_back.get_voltage();
double voltrm = right_wheel_middle.get_voltage();
double tempint = intake.get_temperature();
int flySpeed = 0;
int inSpeed = 0;
int indexSpeed = 0;
bool goingDown = false;
bool r1Engaged = false;
bool r2Engaged = false;
bool l1Engaged = false;
bool l2Engaged = false;
bool aEngaged = false;
bool tbhToggle = true;
int angleOffset;
double tbhDrive = 250.0;
double prev_errorDrive = 0.0;
double flyDriveD = 0.0;
int weeds;


/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series1 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart1 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series3 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series4 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart2 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series5 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series6 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart3 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series7 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series8 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart4 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series9 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series10 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart5 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series11 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series12 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart6 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series13 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series14 = lv_chart_add_series(chart, LV_COLOR_RED);
lv_obj_t * chart7 = lv_chart_create(lv_scr_act(), NULL); 
lv_chart_series_t *series15 = lv_chart_add_series(chart, LV_COLOR_BLUE);
lv_chart_series_t *series16 = lv_chart_add_series(chart, LV_COLOR_RED);
const char* ConvertDoubleToString(double value){
    std::stringstream ss ;
    ss << value;
    const char* str = ss.str().c_str();
    return str;
}  


void lv_tutorial_objects(void)
{
  
  static lv_style_t style_new;                         /*Styles can't be local variables*/
  lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/        /*Fully round corners*/
  style_new.body.main_color = LV_COLOR_BLACK;          /*White main color*/
  style_new.body.grad_color = LV_COLOR_GREEN;           /*Blue gradient color*/
  style_new.body.shadow.color = LV_COLOR_SILVER;       /*Light gray shadow color*/
  style_new.body.shadow.width = 8;                     /*8 px shadow*/
  style_new.body.border.width = 0;                     /*2 px border width*/
  style_new.text.color = LV_COLOR_WHITE;                 /*Red text color */
  style_new.text.letter_space = 0.5;                    /*10 px letter space*/
  
  lv_obj_t * scr = lv_page_create(NULL, NULL);
  lv_scr_load(scr);
  lv_obj_set_style(scr, &style_new);
  /*Create a label on the button (the 'label' variable can be reused)*/
  lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn1, 0);
  lv_obj_set_x(btn1, 10);
  lv_obj_set_style(btn1, &style_new);
  lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn2, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn2, 20);
  lv_obj_set_x(btn2, 170);
  lv_obj_set_style(btn2, &style_new);
  lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label2, "Left Wheel Front (16) info:");
  lv_obj_set_y(label2, 320);
  lv_obj_set_style(label2, &style_new);
  lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label3, ConvertDoubleToString(templf));
  lv_obj_set_y(label3, 320);
  lv_obj_set_x(label3, 220);
  lv_obj_t * label4 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label4, "Left Wheel Middle (17) info:");
  lv_obj_set_y(label4, 340);
  lv_obj_set_style(label4, &style_new);
  lv_obj_t * label5 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label5, ConvertDoubleToString(templm));
  lv_obj_set_y(label5, 340);
  lv_obj_set_x(label5, 220);
  lv_obj_t * label6 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label6, "Left Wheel Back (18) info:");
  lv_obj_set_y(label6, 360);
  lv_obj_set_style(label6, &style_new);
  lv_obj_t * label7 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label7, ConvertDoubleToString(templb));
  lv_obj_set_y(label7, 360);
  lv_obj_set_x(label7, 220);
  lv_obj_t * label8 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label8, "Right Wheel Front (13) info:");
  lv_obj_set_y(label8, 380);
  lv_obj_set_style(label8, &style_new);
  lv_obj_t * label9 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label9, ConvertDoubleToString(temprf));
  lv_obj_set_y(label9, 380);
  lv_obj_set_x(label9, 220);
  lv_obj_t * label10 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label10, "Right Wheel Middle (14) info:");
  lv_obj_set_y(label10, 400);
  lv_obj_set_style(label10, &style_new);
  lv_obj_t * label11 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label11, ConvertDoubleToString(temprm));
  lv_obj_set_y(label11, 400);
  lv_obj_set_x(label11, 220);
  lv_obj_t * label12 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label12, "Right Wheel Back (15) info:");
  lv_obj_set_y(label12, 420);
  lv_obj_set_style(label12, &style_new);
  lv_obj_t * label13 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label13, ConvertDoubleToString(temprb));
  lv_obj_set_y(label13, 420);
  lv_obj_set_x(label13, 220);
  lv_obj_t * label14 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label14, "Intake (1) info:");
  lv_obj_set_y(label14, 280);
  lv_obj_set_style(label14, &style_new);
  lv_obj_t * label20 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label20, "Temperature:");
  lv_obj_set_y(label20, 260);
  lv_obj_set_x(label20, 0);
  lv_obj_set_style(label20, &style_new);
  lv_obj_t * label15 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label15, ConvertDoubleToString(tempint));
  lv_obj_set_y(label15, 280);
  lv_obj_set_x(label15, 220);
  lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label1, "Flywheel (6) info:");
  lv_obj_set_y(label1, 300);
  lv_obj_set_style(label1, &style_new);
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, ConvertDoubleToString(temp));
  lv_obj_set_x(label, 220);
  lv_obj_set_y(label, 300);
  
  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "LEFT AUTON 1");
  label = lv_label_create(btn2, NULL);
  lv_label_set_text(label, "SOLO AWP");
  lv_obj_t * btn3 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn3, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn3, 20);
  lv_obj_set_x(btn3, 330);
  lv_obj_set_style(btn3, &style_new);
  label = lv_label_create(btn3, NULL);
  lv_label_set_text(label, "LEFT AUTON 2");
  lv_obj_t * btn4 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn4, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn4, 150);
  lv_obj_set_x(btn4, 10);
  lv_obj_set_style(btn4, &style_new);
  label = lv_label_create(btn4, NULL);
  lv_label_set_text(label, "RIGHT AUTON 1");
  lv_obj_t * btn5 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn5, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn5, 150);
  lv_obj_set_x(btn5, 330);
  lv_obj_set_style(btn5, &style_new);
  label = lv_label_create(btn5, NULL);
  lv_label_set_text(label, "RIGHT AUTON 2");
  lv_obj_t * btn6 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn6, LV_BTN_ACTION_CLICK, checklistVar); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn6, 150);
  lv_obj_set_x(btn6, 170);
  lv_obj_set_style(btn6, &style_new);
  label = lv_label_create(btn6, NULL);
  lv_label_set_text(label, "CHECKLIST");
  lv_obj_t * btn7 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
  lv_btn_set_action(btn7, LV_BTN_ACTION_CLICK, chartVar); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn7, 260);
  lv_obj_set_x(btn7, 330);
  lv_obj_set_style(btn7, &style_new);
  label = lv_label_create(btn7, NULL);
  lv_btn_set_fit(btn7, true, false);
  lv_label_set_text(label, "Watt and RPM Charts");
  lv_obj_set_size(btn7, 100, 180);
  lv_obj_set_size(label, 20, 20);
}


void lv_tutorial_objects1(void){
  static lv_style_t style_new;                         /*Styles can't be local variables*/
  lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/        /*Fully round corners*/
  style_new.body.main_color = LV_COLOR_WHITE;          /*White main color*/
  style_new.body.grad_color = LV_COLOR_WHITE;           /*Blue gradient color*/
  style_new.body.shadow.color = LV_COLOR_SILVER;       /*Light gray shadow color*/
  style_new.body.shadow.width = 8;                     /*8 px shadow*/
  style_new.body.border.width = 0;                     /*2 px border width*/
  style_new.text.color = LV_COLOR_TEAL;                 /*Red text color */
  style_new.text.letter_space = 0;
  
}
void lv_checklist(void){
  static lv_style_t style_new;                         /*Styles can't be local variables*/
  lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/        /*Fully round corners*/
  style_new.body.main_color = LV_COLOR_BLACK;          /*White main color*/
  style_new.body.grad_color = LV_COLOR_BLUE;           /*Blue gradient color*/
  style_new.body.shadow.color = LV_COLOR_SILVER;       /*Light gray shadow color*/
  style_new.body.shadow.width = 8;                     /*8 px shadow*/
  style_new.body.border.width = 0;                     /*2 px border width*/
  style_new.text.color = LV_COLOR_WHITE;                 /*Red text color */
  style_new.text.letter_space = 1;
  lv_obj_t * scr = lv_page_create(NULL, NULL);
  lv_scr_load(scr);
  lv_obj_set_style(scr, &style_new);
  lv_obj_t * label = lv_label_create(NULL, NULL);
  lv_obj_t * btn3 = lv_btn_create(lv_scr_act(), NULL);
  lv_btn_set_action(btn3, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_y(btn3, 20);
  lv_obj_set_x(btn3, 10);
  lv_obj_set_style(btn3, &lv_style_pretty);
  lv_obj_set_size(btn3, 200, 30);
  label = lv_label_create(btn3, NULL);
  lv_label_set_text(label, "1. Pneumatics asf");
  lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
  lv_btn_set_action(btn, LV_BTN_ACTION_CLICK, leftAutonVariable); /*Set function to be called when the button is released*/
  lv_obj_set_style(btn, &lv_style_pretty);
  lv_obj_set_size(btn, 200, 30);
  label = lv_label_create(btn, NULL);
  lv_label_set_text(label, "2. Wires asf");
  lv_obj_align(btn, btn3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20); 
  lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
  lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, back); /*Set function to be called when the button is released*/
  lv_obj_set_style(btn1, &lv_style_pretty);
  lv_obj_set_size(btn1, 200, 30);
  label = lv_label_create(btn1, NULL);
  lv_label_set_text(label, "Back");
  lv_obj_set_x(btn1, 330);
  lv_btn_set_fit(btn1, true, true); 
}
void lv_charts(void){
  static lv_style_t style_new;                         /*Styles can't be local variables*/
  lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/        /*Fully round corners*/
  style_new.body.main_color = LV_COLOR_BLACK;          /*White main color*/
  style_new.body.grad_color = LV_COLOR_RED;           /*Blue gradient color*/
  style_new.body.shadow.color = LV_COLOR_SILVER;       /*Light gray shadow color*/
  style_new.body.shadow.width = 8;                     /*8 px shadow*/
  style_new.body.border.width = 0;                     /*2 px border width*/
  style_new.text.color = LV_COLOR_WHITE;                 /*Red text color */
  style_new.text.letter_space = 1;
  
  lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label1, "Flywheel:");
  lv_obj_set_y(label1, 50);
  lv_obj_set_style(label1, &style_new);
  lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart, 100, 100);
  lv_obj_align(chart, label1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
  lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series1 = lv_chart_add_series(chart, LV_COLOR_BLUE);
  lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);

  lv_chart_set_range(chart, 1, 600);
  lv_chart_set_point_count(chart, 100);
  lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
  lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, back); /*Set function to be called when the button is released*/
  lv_obj_set_style(btn1, &lv_style_pretty);
  lv_obj_set_size(btn1, 200, 30);
  label1 = lv_label_create(btn1, NULL);
  lv_label_set_text(label1, "Back");
  lv_obj_set_x(btn1, 330);
  lv_btn_set_fit(btn1, true, true); 
  lv_obj_t * label2 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label2, "Left Front:");
  lv_obj_set_y(label2, 70);
  lv_obj_set_x(label2, 130);
  lv_obj_set_style(label2, &style_new);
  lv_obj_t * chart1 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart1, 100, 100);
  lv_obj_align(chart1, label2, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_chart_set_type(chart1, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series3 = lv_chart_add_series(chart1, LV_COLOR_BLUE);
  lv_chart_series_t *series4 = lv_chart_add_series(chart1, LV_COLOR_RED);

  lv_chart_set_range(chart1, 1, 600);
  lv_chart_set_point_count(chart1, 100);

  lv_obj_t * label3 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label3, "Left Middle:");
  lv_obj_set_y(label3, 70);
  lv_obj_set_x(label3, 240);
  lv_obj_set_style(label3, &style_new);
  lv_obj_t * chart2 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart2, 100, 100);
  lv_obj_align(chart2, label3, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_chart_set_type(chart2, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series5 = lv_chart_add_series(chart2, LV_COLOR_BLUE);
  lv_chart_series_t *series6 = lv_chart_add_series(chart2, LV_COLOR_RED);

  lv_chart_set_range(chart2, 1, 600);
  lv_chart_set_point_count(chart2, 100);

  lv_obj_t * label4 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label4, "Left Back:");
  lv_obj_set_y(label4, 70);
  lv_obj_set_x(label4, 370);
  lv_obj_set_style(label4, &style_new);
  lv_obj_t * chart3 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart3, 100, 100);
  lv_obj_align(chart3, label4, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_chart_set_type(chart3, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series7 = lv_chart_add_series(chart3, LV_COLOR_BLUE);
  lv_chart_series_t *series8 = lv_chart_add_series(chart3, LV_COLOR_RED);

  lv_chart_set_range(chart3, 1, 600);
  lv_chart_set_point_count(chart3, 100);

  lv_obj_t * label5 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label5, "Intake:");
  lv_obj_set_y(label5, 200);
  lv_obj_set_x(label5, 0);
  lv_obj_set_style(label5, &style_new);
  lv_obj_t * chart4 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart4, 100, 100);
  lv_obj_align(chart4, label5, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
  lv_chart_set_type(chart4, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series9 = lv_chart_add_series(chart4, LV_COLOR_BLUE);
  lv_chart_series_t *series10 = lv_chart_add_series(chart4, LV_COLOR_RED);

  lv_chart_set_range(chart4, 1, 600);
  lv_chart_set_point_count(chart4, 100);

  lv_obj_t * label6 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label6, "Right Front:");
  lv_obj_set_y(label6, 200);
  lv_obj_set_x(label6, 130);
  lv_obj_set_style(label6, &style_new);
  lv_obj_t * chart5 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart5, 100, 100);
  lv_obj_align(chart5, label6, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_chart_set_type(chart5, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series11 = lv_chart_add_series(chart5, LV_COLOR_BLUE);
  lv_chart_series_t *series12 = lv_chart_add_series(chart5, LV_COLOR_RED);

  lv_chart_set_range(chart5, 1, 600);
  lv_chart_set_point_count(chart5, 100);

  lv_obj_t * label7 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label7, "Right Middle:");
  lv_obj_set_y(label7, 200);
  lv_obj_set_x(label7, 240);
  lv_obj_set_style(label7, &style_new);
  lv_obj_t * chart6 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart6, 100, 100);
  lv_obj_align(chart6, label7, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_chart_set_type(chart6, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series13 = lv_chart_add_series(chart6, LV_COLOR_BLUE);
  lv_chart_series_t *series14 = lv_chart_add_series(chart6, LV_COLOR_RED);

  lv_chart_set_range(chart6, 1, 600);
  lv_chart_set_point_count(chart6, 100);

  lv_obj_t * label8 = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label8, "Right Back:");
  lv_obj_set_y(label8, 200);
  lv_obj_set_x(label8, 370);
  lv_obj_set_style(label8, &style_new);
  lv_obj_t * chart7 = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
  // lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/                                           /*Set the line width*/

  /*Add a RED data series and set some points*/
  lv_obj_set_size(chart7, 100, 100);
  lv_obj_align(chart7, label8, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
  lv_chart_set_type(chart7, LV_CHART_TYPE_LINE);

  lv_chart_series_t *series15 = lv_chart_add_series(chart7, LV_COLOR_BLUE);
  lv_chart_series_t *series16 = lv_chart_add_series(chart7, LV_COLOR_RED);

  lv_chart_set_range(chart7, 1, 600);
  lv_chart_set_point_count(chart7, 100);
}
void lv_charts1 (void){
  static lv_style_t style_new;                         /*Styles can't be local variables*/
  lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/        /*Fully round corners*/
  style_new.body.main_color = LV_COLOR_BLACK;          /*White main color*/
  style_new.body.grad_color = LV_COLOR_RED;           /*Blue gradient color*/
  style_new.body.shadow.color = LV_COLOR_SILVER;       /*Light gray shadow color*/
  style_new.body.shadow.width = 8;                     /*8 px shadow*/
  style_new.body.border.width = 0;                     /*2 px border width*/
  style_new.text.color = LV_COLOR_RED;                 /*Red text color */
  style_new.text.letter_space = 1;
  lv_obj_t * scr = lv_page_create(NULL, NULL);
  lv_scr_load(scr);
  lv_obj_set_style(scr, &style_new);
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "Wattage");
  lv_obj_set_y(label, 0);
}
void lv_charts2 (void){
  static lv_style_t style_new;                         /*Styles can't be local variables*/
  lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/        /*Fully round corners*/
  style_new.body.main_color = LV_COLOR_BLACK;          /*White main color*/
  style_new.body.grad_color = LV_COLOR_RED;           /*Blue gradient color*/
  style_new.body.shadow.color = LV_COLOR_SILVER;       /*Light gray shadow color*/
  style_new.body.shadow.width = 8;                     /*8 px shadow*/
  style_new.body.border.width = 0;                     /*2 px border width*/
  style_new.text.color = LV_COLOR_BLUE;                 /*Red text color */
  style_new.text.letter_space = 1;
  lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(label, "RPM");
  lv_obj_set_x(label, 80);
  lv_obj_set_y(label, 20);
  lv_obj_set_style(label, &style_new);
}
void initialize() {
  lv_tutorial_objects();
  lv_init();
  
  
  transmission.set_value(true);
  expansion.set_value(false);
  expansion1.set_value(false);
  intakeLift.set_value(false);
  actualIntakeLift.set_value(false);
  
  // Print our branding over your terminal :D
  // lv_obj_set_size(chart, 2000, 200);
  // lv_obj_align(chart, nullptr, LV_ALIGN_CENTER, 0, 0);
  // lv_chart_set_type(chart, LV_CHART_TYPE_LINE);

  // lv_chart_series_t *series2 = lv_chart_add_series(chart, LV_COLOR_RED);

  // lv_chart_set_range(chart, 4200, 5200);
  // lv_chart_set_point_count(chart, 1000);
  // ez::print_ez_template();

  pros::delay(500); // Stop the user from doing anything while legacy ports configure.

  // Configure your chassis controls
  // chassis.toggle_modify_curve_with_controller(true); // Enables modifying the
  // controller curve with buttons on the joysticks
  // // chassis.set_active_brake(0.1); // Sets the active brake kP. We recommend
  // 0.1. chassis.set_curve_default(0, 0); // Defaults for curve. If using tank,
  // only the first parameter is used. (Comment this line out if you have an SD
  // card!)
  void default_constants(); // Set the drive to your own constants from autons.cpp!
  void exit_condition_defaults(); // Set the exit conditions to your own
                                  // constants from autons.cpp!

  // These are already defaulted to these buttons, but you can change the
  // left/right curve buttons here! chassis.set_left_curve_buttons
  // (pros::E_CONTROLLER_DIGITAL_LEFT, pros::E_CONTROLLER_DIGITAL_RIGHT); // If
  // using tank, only the left side is used.
  // chassis.set_right_curve_buttons(pros::E_CONTROLLER_DIGITAL_Y,
  // pros::E_CONTROLLER_DIGITAL_A);

  // Autonomous Selector using LLEMU
  // ez::as::auton_selector.add_autons({
  //   Auton("Example Drive\n\nDrive forward and come back.", drive_example),

  // Initialize chassis and auton selector
  // chassis.initialize();
  // ez::as::initialize();
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {
  // . . .
}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() { 
  initialize(); 
}
void runFlywheelDrive(double velocity) { 
  flywheel.move_voltage(velocity); 
}
void autoFlywheelDrive(double velocity) {
  // double velocity = *velo;
  // runFlywheel(velocity);

  double change = .15;
  double currentVelo = flywheel.get_actual_velocity();
  double error = velocity - currentVelo;
  double output = flyDriveD + change * error;
  if (error * prev_errorDrive < 0) {
    output = 0.5 * (output + tbhDrive);
    tbhDrive = output;
  }
  lv_chart_set_next(chart, series1, flywheel.get_actual_velocity());
  lv_chart_set_next(chart, series2, flywheel.get_current_draw());
  lv_chart_set_next(chart1, series3, left_wheel_front.get_actual_velocity());
  lv_chart_set_next(chart1, series4, left_wheel_front.get_current_draw());
  lv_chart_set_next(chart2, series5, left_wheel_middle.get_actual_velocity());
  lv_chart_set_next(chart2, series6, left_wheel_middle.get_current_draw());
  lv_chart_set_next(chart3, series7, left_wheel_back.get_actual_velocity());
  lv_chart_set_next(chart3, series8, left_wheel_back.get_current_draw());
  lv_chart_set_next(chart4, series9, intake.get_actual_velocity());
  lv_chart_set_next(chart4, series10, intake.get_current_draw());
  lv_chart_set_next(chart5, series11, right_wheel_front.get_actual_velocity());
  lv_chart_set_next(chart5, series12, right_wheel_front.get_current_draw());
  lv_chart_set_next(chart6, series13, right_wheel_middle.get_actual_velocity());
  lv_chart_set_next(chart6, series14, right_wheel_middle.get_current_draw());
  lv_chart_set_next(chart7, series15, right_wheel_back.get_actual_velocity());
  lv_chart_set_next(chart7, series16, right_wheel_back.get_current_draw());
  controller.print(1, 1, "flywheelspeed %f", currentVelo);
  runFlywheelDrive(output);
  flyDriveD = output;
  prev_errorDrive = error;
}

void toggleIntake() {
  if (inSpeed == 0) {
    inSpeed = 1;
    intake.move_velocity(-600);
  } else if (inSpeed == 1 && goingDown == false) {
    inSpeed = 2;
    intake = 0;
  } else if (inSpeed == 2) {
    inSpeed = 1;
    goingDown = true;
    intake.move_velocity(600);
  } else if (inSpeed == 1 && goingDown == true) {
    inSpeed = 0;
    goingDown = false;
    intake = 0;
  }
}

// void toggleIndexer() {
// 	if(indexSpeed == 0){
// 		indexer.move_velocity(125);
//     pros::delay(310);
//     indexer = 0;
//     flywheel = 100;
//     flywheel2 = 100;
//     pros::delay(100);
//     indexer.move_velocity(125);
//     pros::delay(600);
//     indexer = 0;

// 	}
// }
// void toggleIndexerFar() {
// 		indexer.move_velocity(125);
//     pros::delay(300);
//     indexer = 0;
//     pros::delay(12000);
//     indexer.move_velocity(125);
//     pros::delay(300);
//     indexer = 0;
//     pros::delay(12000);
//     indexer.move_velocity(125);
//     pros::delay(300);
//     indexer = 0;
// }
// void flywheel_taskDrive(void* param){
//   double *v = (double*) param;
//   bool twoSpeed = true;
//   //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false,
//   TIMEOUT_MAX))); while(true){//pros::Task::notify_take(true,
//   TIMEOUT_MAX)==1){//pros::Task::notify_take(true, TIMEOUT_MAX)){
//     //pros::lcd::set_text(1,std::to_string(pros::Task::notify_take(false,
//     TIMEOUT_MAX))); if (twoSpeed) {
//       pros::lcd::set_text(2,"a");
//       autoFlywheelDrive(5200);
//     }
//     else if (!twoSpeed) {
//       pros::lcd::set_text(2,"b");
//       autoFlywheelDrive(200);
//     }
//     //autoFlywheel(v);
//   }
// }
/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  inertial.reset();
  chassis.reset_pid_targets();               // Resets PID targets to 0
  chassis.reset_gyro();                      // Reset gyro position to 0
  chassis.reset_drive_sensor();              // Reset drive sensors to 0
  chassis.set_drive_brake(MOTOR_BRAKE_HOLD); // Set motors to hold.  This helps
                                             // autonomous consistency.

  // Skills(); // Calls selected auton from autonomous selector.
  // left_awp();
  leftAuton();
  // rightAuton();
}
void motor_check_overheat() {
  // left
}

/*
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
 
void opcontrol() {
  
  // This is preference to what you like to drive on.
  // if (tbhToggle){
  //   pros::Task fly = pros::Task(flywheel_taskDrive, (void*) 1);
  // }
  // flywheelGraphData *argsStruct = (flywheelGraphData *)args;
  bool l2;
  bool buttonB;
  int test;
  
  // this stuff goes into opcontrol
  
  while (true) {
    // lv_tutorial_objects();
    /*Initialize the alien theme
     * 210: a green HUE value
     * NULL: use the default font (LV_FONT_DEFAULT)*/
    
    
    
    actualIntakeLift.set_value(false);
    intakeLift.set_value(true);
    expansion1.set_value(false);
    expansion.set_value(false);
    chassis.set_drive_brake(MOTOR_BRAKE_COAST);
    float up_down = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    float left_right = master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_X);
    float turnX = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    float turnY = master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
    transmission.set_value(true);

    autoFlywheelDrive(500);
    // pros::delay(100);
    isAuton = false;
    buttonB = controller.get_digital(pros::E_CONTROLLER_DIGITAL_B);
    l2 = controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2);

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
      transmission.set_value(false);
      left_wheel_front.move(-up_down);
      left_wheel_middle.move(-up_down);
      left_wheel_back.move(-up_down);

      right_wheel_front.move(-turnY);
      right_wheel_middle.move(-turnY);
      right_wheel_back.move(-turnY);
    } 
      if (!controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
      transmission.set_value(true);
      left_wheel_front.move(up_down);
      left_wheel_middle.move(up_down);
      left_wheel_back.move(up_down);

      right_wheel_front.move(turnY);
      right_wheel_middle.move(turnY);
      right_wheel_back.move(turnY);
    }

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1) && r1Engaged == false) {
      toggleIntake();
      r1Engaged = true;
    } 
    else if (!controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)){
      r1Engaged = false;
    }
    // if(controller.get_digital(pros::E_CONTROLLER_DIGITAL_A) && aEngaged ==
    // false){ 	aEngaged = true;
    // }
    // else if(!controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)){
    // 	aEngaged = false;
    // }
    // if (buttonB) {
    //   expansionBottom.set_value(true);
    // }
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_Y)) {
      expansion.set_value(true);
      // expansionBottom.set_value(true);
    }
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_RIGHT)) {
      expansion1.set_value(true);
      // expansionBottom.set_value(true);
    }
    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN)){
      intakeLift.set_value(false);
    }
    // chassis.arcade_standard(ez::SPLIT); // Standard split arcade
    // chassis.arcade_standard(ez::SINGLE); // Standard single arcade
    // chassis.arcade_flipped(ez::SPLIT); // Flipped split arcade
    // chassis.arcade_flipped(ez::SINGLE); // Flipped single arcade

    // . . .
    // Put more user control code here!
    // . . .

    // Tank control

    pros::delay(ez::util::DELAY_TIME); // This is used for timer calculations!
                                       // Keep this ez::util::DELAY_TIME
  }
}
static lv_res_t leftAutonVariable(lv_obj_t * btn)
{
  lv_tutorial_objects1();
  return LV_RES_OK;
}
static lv_res_t checklistVar(lv_obj_t * btn)
{
  lv_checklist();
  return LV_RES_OK;
}
static lv_res_t chartVar(lv_obj_t * btn)
{
  lv_charts1();
  lv_charts2();
  lv_charts();
  return LV_RES_OK;
}
static lv_res_t back(lv_obj_t * btn)
{
  lv_tutorial_objects();
  return LV_RES_OK;
}