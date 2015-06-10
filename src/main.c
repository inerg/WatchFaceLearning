#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer  *s_date_layer;
static GFont s_time_font;
static BitmapLayer  *s_background_layer;
static GBitmap  *s_background_bitmap;


static void update_time()  {
  //Gets the tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  int seconds = tick_time->tm_sec;
  int minutes = tick_time->tm_min;
  int hours = tick_time->tm_hour;
  
  if(minutes == 59) {
    //Sets the ticker to seconds for the last minute for the transition to happen at the last second
      //tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    if(seconds == 59) {  
      if((hours % 2) == 1) {
        //Slides off to the left at the bottom
        //GRect
      }
      else {
        
      }
    }
  }
  //Create a buffer for time
  static char time_buffer[] = "00:00";
  
  //Create a buffer for the date
  static char date_buffer[] = "jan01";
  
  //Insert the current hour and minutes into the buffer
  if(clock_is_24h_style()==true)  {
    //Use 24 hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  }  else {
    //Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  strftime(date_buffer, sizeof"jan01", "%h%d", tick_time);
  
  //Put this time into the text layer
  text_layer_set_text(s_time_layer, time_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
}

static void main_window_load(Window *window)  {
  //Create Gbitmap and then set it as the base BitmapLayer for the window
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  
  
  //TIME LAYER
  //Creates the text layer setting the background colour, text colour,
  //and starting text
  s_time_layer = text_layer_create(GRect(5, 52, 139, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  //Setup GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_48));
  
  
  //Add some style to the watchface time
  //Apply font to text layer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  
  //Sets the text layer as a child to the base window layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  
  //DATE LAYER
  s_date_layer = text_layer_create(GRect(5, 110, 139, 50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "jan 01");
  
  
  //Add some style to the watchface date
  //Apply font to text layer
  text_layer_set_font(s_date_layer, s_time_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
  
  //Sets the text layer as a child to the base window layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window)  {
  //Removes the time text layer when closing
  text_layer_destroy(s_time_layer);
  //Removes the date text layer when closing
  text_layer_destroy(s_date_layer);
  //Removes the font for the date and time
  fonts_unload_custom_font(s_time_font);
  //destroys GBitmap (image)
  gbitmap_destroy(s_background_bitmap);
  //Destroys bitmap layer (layer created for image)
  bitmap_layer_destroy(s_background_layer);
}

//Animation
void on_animation_stopped(Animation *anim, bool finished, void *context)  {
  property_animation_destroy((PropertyAnimation*) anim);
}

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay)  {
  //Declaring the animation
  PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
  
  //Filling the animations propertys
  animation_set_duration((Animation*) anim, duration);
  animation_set_delay((Animation*) anim, delay);
  
  //Does something to help free memory
  AnimationHandlers handlers = {  
    .stopped = (AnimationStoppedHandler) on_animation_stopped};
  animation_set_handlers((Animation*) anim, handlers, NULL);
  
  //Start animation
  animation_schedule((Animation*) anim);
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