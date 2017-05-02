#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_hour_layer = NULL;
static TextLayer *s_link_layer = NULL;
static TextLayer *s_minute_layer = NULL;

static char *hour_text[] = {
  "tolv",
  "ett",
  "två",
  "tre",
  "fyra",
  "fem",
  "sex",
  "sju",
  "åtta",
  "nio",
  "tio",
  "elva",
};

static char *link_text[] = {
  "",
  "över",
  "över",
  "över",
  "över",
  "i",
  "",
  "över",
  "i",
  "i",
  "i",
  "i",
};

static char *minute_text[] = {
  "",
  "fem",
  "tio",
  "kvart",
  "tjugo",
  "fem",
  "",
  "fem",
  "tjugo",
  "kvart",
  "tio",
  "fem",
};

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  static char hour_display[20];
  static char link_display[10];
  static char minute_display[20];
  
  int hour = tick_time->tm_hour % 12;
  int seconds = tick_time->tm_min * 60 + tick_time->tm_sec;
  int diff = seconds % 300;
  if (diff < 150) { 
    seconds -= diff;
  } else {
    seconds += 300 - diff;
  }
  if (seconds > 1800) {
    hour = (hour + 1) % 12;
  }
  if (seconds == 3600) {
    seconds = 0;
  }
  int minute = seconds / 300;
  
  if (minute >= 5 && minute <= 7) {
    snprintf(hour_display, 20, "halv %s", hour_text[hour]);
  } else {
    strncpy(hour_display, hour_text[hour], 10);
  }
  
  strncpy(link_display, link_text[minute], 10);
  strncpy(minute_display, minute_text[minute], 20);
  
  text_layer_set_text(s_hour_layer, hour_display);
  text_layer_set_text(s_link_layer, link_display);
  text_layer_set_text(s_minute_layer, minute_display);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_hour_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(98, 90), bounds.size.w, 40));
  s_link_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 50), bounds.size.w, 40));
  s_minute_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(18, 10), bounds.size.w, 40));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_hour_layer, GColorClear);
  text_layer_set_text_color(s_hour_layer, GColorBlack);
  text_layer_set_text(s_hour_layer, "");
  text_layer_set_font(s_hour_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);

  text_layer_set_background_color(s_link_layer, GColorClear);
  text_layer_set_text_color(s_link_layer, GColorBlack);
  text_layer_set_text(s_link_layer, "lite");
  text_layer_set_font(s_link_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(s_link_layer, GTextAlignmentCenter);

  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorBlack);
  text_layer_set_text(s_minute_layer, "Vänta");
  text_layer_set_font(s_minute_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_link_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_link_layer);
  text_layer_destroy(s_minute_layer);
}

static void init(void) {
	s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit(void) {
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
