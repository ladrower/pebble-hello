#include <pebble.h>
#include <stdlib.h>

static Window *window;
static TextLayer *text_layer;

static bool idle = true;
static bool answered = false;
static char problem[6];
static char answer[4];

static void update_problem() {
  time_t t;
  srand((unsigned) time(&t));
  
  uint8_t a = rand() % 100;
  uint8_t b = rand() % 100;

  snprintf(problem, 6, "%d+%d", a, b);
  snprintf(answer, 4, "%d", a + b);
  text_layer_set_text(text_layer, problem);
}

static void proceed () {
  if (answered) {
    update_problem();
  } else {
    text_layer_set_text(text_layer, answer);
  }
  answered = !answered;
  idle = false;
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
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (idle) {
    update_problem();
  }
  idle = true;
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
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void deinit () {
  window_destroy(window);
  problem[0] = '\0';
  answer[0] = '\0';
}

int main () {
  init();
  app_event_loop();
  deinit();
  return 0;
}
