#include <pebble.h>

Window *s_main_window;	

static GBitmap *s_loading_bitmap;
static BitmapLayer *s_bitmap_layer;
static TextLayer *s_text_layer;
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};

// Write message to buffer & send
void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
	
	dict_write_end(iter);
  app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "NReceived Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "NReceived Message: %s", tuple->value->cstring);
	}}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

  APP_LOG(APP_LOG_LEVEL_DEBUG,"Up pressed!");
  send_message();
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  s_loading_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOADING_IMAGE);
  
  s_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_loading_bitmap);
  
  s_text_layer = text_layer_create(GRect(0, bounds.size.h/2, bounds.size.w , bounds.size.h/2));
  text_layer_set_text_color(s_text_layer, GColorWhite);
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(s_text_layer, "Loading...");
  text_layer_set_overflow_mode(s_text_layer, GTextOverflowModeWordWrap);
  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  
}

static void main_window_unload(Window *window) {
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_loading_bitmap);

  text_layer_destroy(s_text_layer);
}

void init(void) {
	s_main_window = window_create();
  #ifdef PBL_SDK_2
    window_set_fullscreen(s_main_window, true);
  #endif
  
  window_set_background_color(s_main_window, COLOR_FALLBACK(GColorBlue, GColorBlack));
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
	window_stack_push(s_main_window, true);
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  window_set_click_config_provider(s_main_window, click_config_provider);

}

void deinit(void) {
 
	app_message_deregister_callbacks();
	window_destroy(s_main_window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}