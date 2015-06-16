#include <pebble.h>

Window *s_station_window;	
//menu layer for selecting the stations
static MenuLayer *menu_layer;
//global things
static GRect bounds;
static Layer *station_window_layer;


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

void showStationWindow() {

  window_stack_push(s_station_window, true);	
  	
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

static void station_window_unload(Window *window) {


}

void initStationSelectWindow() {
	s_station_window = window_create();
  	#ifdef PBL_SDK_2
		window_set_fullscreen(s_station_window, true);
  	#endif

	window_set_background_color(s_station_window, COLOR_FALLBACK(GColorBlue, GColorWhite));
	window_set_window_handlers(s_station_window, (WindowHandlers) {
		.load = station_window_load,
		.unload = station_window_unload,
	});	
}

