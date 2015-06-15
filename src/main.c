#include <pebble.h>

Window *s_intro_window;	
Window *s_station_window;	

static GBitmap *s_loading_bitmap;
static BitmapLayer *s_bitmap_layer;
static TextLayer *s_text_layer;
static char status_text_buffer[] = "";

//menu layer for selecting the stations
static MenuLayer *menu_layer;
//global things
static GRect bounds;
static Layer *intro_window_layer;
static Layer *station_window_layer;

void showStationWindow();

// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1
};


// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return 3;
}

// This is the menu item draw callback where you specify what each item should look like
static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
      // Use the row to specify which item we'll draw
	switch (cell_index->row) {
		case 0:
          // This is a basic menu item with a title and subtitle
		menu_cell_basic_draw(ctx, cell_layer, "Basic Item 1", "With a subtitle", NULL);
		break;
		case 1:
          // This is a basic menu item with a title and subtitle
		menu_cell_basic_draw(ctx, cell_layer, "Basic Item 2", "With a subtitle", NULL);
		break;
		case 2:
          // This is a basic menu item with a title and subtitle
		menu_cell_basic_draw(ctx, cell_layer, "Basic Item 3", "With a subtitle", NULL);
		break;

	}
}


// Write message to buffer & send
void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
	
	dict_write_end(iter);
	app_message_outbox_send();
}

static void setStatusText(char *status_text_buffer) {
	text_layer_set_text(s_text_layer, status_text_buffer);
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
	}
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {

	APP_LOG(APP_LOG_LEVEL_DEBUG,"Up pressed!");
	//send_message();
	showStationWindow();
}

static void click_config_provider(void *context) {
  // Register the ClickHandlers
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);

}

void showStationWindow() {


  window_stack_push(s_station_window, true);	
  	
}

static void intro_window_load(Window *window) {
	intro_window_layer = window_get_root_layer(window);
	
	bounds = layer_get_bounds(intro_window_layer);

	s_loading_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LOADING_IMAGE);

	s_bitmap_layer = bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(s_bitmap_layer, s_loading_bitmap);

	s_text_layer = text_layer_create(GRect(0, bounds.size.h/2 + 20 , bounds.size.w , bounds.size.h/2-20));
	text_layer_set_text_color(s_text_layer, GColorWhite);
	text_layer_set_background_color(s_text_layer, GColorClear);
	text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));  
	text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
	text_layer_set_overflow_mode(s_text_layer, GTextOverflowModeWordWrap);

	setStatusText("loading...");
	layer_add_child(intro_window_layer, bitmap_layer_get_layer(s_bitmap_layer));
	layer_add_child(intro_window_layer, text_layer_get_layer(s_text_layer));

}


static void intro_window_unload(Window *window) {
	bitmap_layer_destroy(s_bitmap_layer);
	gbitmap_destroy(s_loading_bitmap);

	text_layer_destroy(s_text_layer);
}

static void station_window_unload(Window *window) {


}

static void station_window_load(Window *window) {
  station_window_layer = window_get_root_layer(window);	
  GRect sbounds = layer_get_bounds(station_window_layer);
	  // Create the menu layer
  menu_layer = menu_layer_create(sbounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    //.get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    //.get_header_height = menu_get_header_height_callback,
   // .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    //.select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, s_station_window);

  // Add it to the window for display
  layer_add_child(station_window_layer, menu_layer_get_layer(menu_layer));
  
}

void initLoadingWindow() {
	s_intro_window = window_create();
  	#ifdef PBL_SDK_2
		window_set_fullscreen(s_intro_window, true);
  	#endif

	window_set_background_color(s_intro_window, COLOR_FALLBACK(GColorBlue, GColorBlack));
	window_set_window_handlers(s_intro_window, (WindowHandlers) {
		.load = intro_window_load,
		.unload = intro_window_unload,
	});
}

void initStationSelectWindow() {
	s_station_window = window_create();
  	#ifdef PBL_SDK_2
		window_set_fullscreen(s_station_window, true);
  	#endif

	window_set_background_color(s_station_window, COLOR_FALLBACK(GColorBlue, GColorBlack));
	window_set_window_handlers(s_station_window, (WindowHandlers) {
		.load = station_window_load,
		.unload = station_window_unload,
	});	
}

void showIntroWindow() {
	window_stack_push(s_intro_window, true);	
	window_set_click_config_provider(s_intro_window, click_config_provider);
}

void init(void) {
	initLoadingWindow();
	initStationSelectWindow();
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);

	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

	//showIntroWindow();
	showStationWindow();

}

void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(s_intro_window);
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}