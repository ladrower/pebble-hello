#include <pebble.h>
#include <stdlib.h>
#include <time.h>

static Window *window;
static TextLayer *text_layer;
static AppTimer *app_timer;

static bool answered = false;
static char problem[7];
static char answer[4];
static const uint32_t PROBLEM_INTERVAL = 300000;
static const uint32_t ANSWER_INTERVAL = 5000;

static void update_problem() {  
  uint8_t a = rand() % 98 + 2;
  uint8_t b = rand() % 98 + 2;
  int16_t result;
  char sign;
  switch (rand() % 5) {
    default:
      sign = '+';
      result = a + b;
      break;
    case 0:
      sign = '-';
      result = a - b;
      break;
    case 1:
      if (a > 50) {
        a = a % 8 + 2; 
      } else {
        b = b % 8 + 2; 
      }
      sign = 'x';
      result = a * b;
      break;
    case 2:
      if (a > 50) {
        result = a % 8 + 2;
        b = b % 31 + 2; 
      } 
      if (a <= 50 || (result * b > 99 && b > 9)) {
        result = a % 31 + 2;
        b = b % 8 + 2;
      }
      a = result * b; 
    
    
      sign = '/';
      break;
  }

  snprintf(problem, 7, "%d%c%d", a, sign, b);
  snprintf(answer, 4, "%d", result);
  text_layer_set_text(text_layer, problem);
}

static void set_problem(void *data) {
  update_problem();
  app_timer = app_timer_register(PROBLEM_INTERVAL, set_problem, data);
  answered = false;
}

static void proceed () {
  uint32_t interval;
  if (answered) {
    interval = PROBLEM_INTERVAL;
    update_problem();
  } else {
    interval = ANSWER_INTERVAL;
    text_layer_set_text(text_layer, answer);
  }
  answered = !answered;
  
  if (!app_timer_reschedule(app_timer, interval)) {
    app_timer = app_timer_register(interval, set_problem, NULL);
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  proceed();
}

static void click_config_provider(void *context) {
  //window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  //window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  text_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  srand((unsigned) time(NULL));
  set_problem(NULL);
}

static void main_window_unload(Window *window) {
  app_timer_cancel(app_timer);
  text_layer_destroy(text_layer);
}

void init () {
  window = window_create();
  
  window_set_click_config_provider(window, click_config_provider);
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_set_background_color(window, GColorBlack);
  
  window_stack_push(window, true);
}

void deinit () {
  window_destroy(window);
}

int main () {
  init();
  app_event_loop();
  deinit();
  return 0;
}
