#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
static BitmapLayer  *s_background_layer;
static GBitmap  *s_background_bitmap;

static void update_time()  {
  //Gets the tm structure(still trying to figure out what tm is)
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  //Create a buffer
  static char buffer[] = "00:00";
  
  //Insert the current hour and minutes into the buffer
  if(clock_is_24h_style()==true)  {
    //Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  }  else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  //Put this time into the text layer
  text_layer_set_text(s_time_layer, buffer);
}

static void main_window_load(Window *window)  {
  //Create Gbitmap and then set it as the base BitmapLayer for the window
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  
  
  //Creates the text layer setting the background colour, text colour,
  //and starting text
  s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  //Setup GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  
  
  //Add some style to the watchface
  //Apply font to text layer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //Sets the text layer as a child to the base window layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window)  {
  //Removes the text layer when closing
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_time_font);
  //destroys GBitmap (image)
  gbitmap_destroy(s_background_bitmap);
  //Destroys bitmap layer (layer created for image)
  bitmap_layer_destroy(s_background_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)  {
  update_time();
}

static void init()  {
  //Creates window element and gives it a pointer.
  s_main_window = window_create();  

  //Sets what handles a user entering or leaving the window
  window_set_window_handlers(s_main_window, (WindowHandlers)  {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  //Shows the window with animations being enabled
  window_stack_push(s_main_window, true);
  
  //Ensures the time is displayed correctly at start
  update_time();
  
  //Registering the TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit()  {
  //Removes the window from memory
  window_destroy(s_main_window);
}

int main(void)  {
  init();
  app_event_loop();
  deinit();
}