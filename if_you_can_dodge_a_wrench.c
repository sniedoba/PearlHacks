/*

   IF YOU CAN DODGE A WRENCH, YOU CAN DODGE A BRICK.

*/

#include <pebble.h>
#include<time.h>
#define BOX_SIZE 30
#define TITLE_BAR_SIZE 16

#define ANIM_DURATION 5000
#define ANIM_DELAY 200

#define MATH_PI 3.141592653589793238462
#define DISC_DENSITY 0.1
#define ACCEL_RATIO 0.04
#define ACCEL_STEP_MS 50

typedef struct Vec2d {
 double x;
 double y;
} Vec2d;

typedef struct Disc {
#ifdef PBL_COLOR
 GColor color;
#endif
 Vec2d pos;
 Vec2d vel;
 double mass;
 double radius;
} Disc;

static int score = 0;
static Window *s_main_window;
static Window *alien_main_window;
static GBitmap *alien_bitmap;
static BitmapLayer *emoji_bitmap_layer;

static Layer *s_disc_layer;
static InverterLayer *s_box_layer;
static InverterLayer *s_box_layer2;
static TextLayer *s_text_layer;
static PropertyAnimation *s_box_animation;
static PropertyAnimation *s_box_animation2;

static Disc disc;
static GRect window_frame;
static int track = 1;
static int track2 = 1;

static int s_current_stage = 0;
static int s_current_stage2 = 0;

static int num;
static int num2;

// Function prototype
static void next_animation();
static void next_animation2();

//Finishes loading the window (game over image)
static void end_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(alien_main_window);
  GRect bounds = layer_get_bounds(window_layer);
  alien_bitmap = gbitmap_create_with_resource(RESOURCE_ID_GAME2);
  emoji_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(emoji_bitmap_layer, alien_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(emoji_bitmap_layer));
}

static void end_window_unload(Window *window){
  bitmap_layer_destroy(emoji_bitmap_layer);
  gbitmap_destroy(alien_bitmap);
}


static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
 // Free the animation
 property_animation_destroy(s_box_animation);
  
 // Schedule the next one, unless the app is exiting
 if (finished) {
   next_animation();
 }
}

static void anim_stopped_handler2(Animation *animation, bool finished, void *context) {
 // Free the animation
 property_animation_destroy(s_box_animation2);

 // Schedule the next one, unless the app is exiting
 if (finished) {
   next_animation2();
 }
}

//Executes when game is over, puts game over window
static void game_over() {
  window_stack_push(alien_main_window, true);
}

//controls movement of one block across screen to left
static void next_animation() {
  
 // Determine start and finish positions
 GRect start, finish;
 srand(time(NULL));
 int val = rand();
 
 //if gone all across screen then go to new random y-location, or else check for collision 
 if (track){
   num =  val % 100 + 10;
 } else {
   score++;
   int vall = disc.pos.y - num;
   if (vall < 31 && vall > -2){
       game_over();
       text_layer_set_text(s_text_layer, "DEAD");
   }
  }
 GRect frame = window_frame;
  
  //goes to x-location of the circle and checks to see if collide then continues
  if (track){
   switch (s_current_stage){
     default:
     start = GRect(144 - BOX_SIZE, num, BOX_SIZE-20, BOX_SIZE);
     finish = GRect((int)frame.size.w/10, num, BOX_SIZE-20, BOX_SIZE);
     track = 0;
     break;
   }
   } else {
     switch (s_current_stage) {
     default:

     start = GRect((int)frame.size.w/10, num, BOX_SIZE-20, BOX_SIZE);
     finish = GRect(-BOX_SIZE, num, BOX_SIZE-20, BOX_SIZE);
     track = 1;
     break;
     }
   }

 // Schedule the next animation
 s_box_animation = property_animation_create_layer_frame(inverter_layer_get_layer(s_box_layer), &start, &finish);
 animation_set_duration((Animation*)s_box_animation, ANIM_DURATION);
 animation_set_delay((Animation*)s_box_animation, ANIM_DELAY);
 animation_set_curve((Animation*)s_box_animation, AnimationCurveEaseInOut);
 animation_set_handlers((Animation*)s_box_animation, (AnimationHandlers) {
   .stopped = anim_stopped_handler
 }, NULL);
 animation_schedule((Animation*)s_box_animation);

 // Increment stage and wrap
 s_current_stage = (s_current_stage + 1) % 4;
}

//controls animation of second rectangle flying across screen to left
static void next_animation2() {
  
 // Determine start and finish positions
 GRect start, finish;
  int val = rand();
  
  //if done moving across screen, goes to new random y-location
  if (track2){
   num2 = val % 140 + 10;
    //num = 50;
  } else {
    int vall = disc.pos.y - num2;
    if (vall < 31 && vall > -2){
       game_over();
       text_layer_set_text(s_text_layer, "DEAD");
     }
  }
 GRect frame = window_frame;
  
  //Pauses when get to x-location of circle and checks for collision
  if (track2){
   switch (s_current_stage2){
     default:
     start = GRect(144 - BOX_SIZE, num2, BOX_SIZE-20, BOX_SIZE);
     finish = GRect((int)frame.size.w/10, num2, BOX_SIZE-20, BOX_SIZE);
     track2 = 0;
     break;
   }
   } else {
     switch (s_current_stage2) {
     default:

     start = GRect((int)frame.size.w/10, num2, BOX_SIZE-20, BOX_SIZE);
     finish = GRect(-BOX_SIZE, num2, BOX_SIZE-20, BOX_SIZE);
     track2 = 1;
     break;
     }
   }

 // Schedule the next animation
 s_box_animation2 = property_animation_create_layer_frame(inverter_layer_get_layer(s_box_layer2), &start, &finish);
 animation_set_duration((Animation*)s_box_animation2, ANIM_DURATION+1000);
 animation_set_delay((Animation*)s_box_animation2, ANIM_DELAY+20);
 animation_set_curve((Animation*)s_box_animation2, AnimationCurveEaseInOut);
 animation_set_handlers((Animation*)s_box_animation2, (AnimationHandlers) {
   .stopped = anim_stopped_handler2
 }, NULL);
 animation_schedule((Animation*)s_box_animation2);

 // Increment stage and wrap
 s_current_stage2 = (s_current_stage2 + 1) % 4;
}

static double disc_calc_mass(Disc *disc) {
 return MATH_PI * disc->radius * disc->radius * DISC_DENSITY;
}

//initializes the disc information
static void disc_init(Disc *disc) {
 GRect frame = window_frame;
 disc->pos.x = frame.size.w/10;
 disc->pos.y = frame.size.h/2;
 disc->vel.y = 0;
 disc->radius = 3;
 disc->mass = disc_calc_mass(disc);
#ifdef PBL_COLOR
 disc->color = GColorFromRGB(rand() % 255, rand() % 255, rand() % 255);
#endif
}

static void disc_apply_force(Disc *disc, Vec2d force) {
 disc->vel.y += force.y / disc->mass / 5;
}

static void disc_apply_accel(Disc *disc, AccelData accel) {
 disc_apply_force(disc, (Vec2d) {
   .y = -accel.y * ACCEL_RATIO
 });
}

static void disc_update(Disc *disc) {
 double e = 0.5;

 if ((disc->pos.y - disc->radius < 0 && disc->vel.y < 0)
   || (disc->pos.y + disc->radius > window_frame.size.h && disc->vel.y > 0)) {
   disc->vel.y = -disc->vel.y * e;
 }

 disc->pos.y += disc->vel.y / 2;
}

static void disc_draw(GContext *ctx, Disc *disc) {
  #ifdef PBL_COLOR
   graphics_context_set_fill_color(ctx, disc->color);
  #else
   graphics_context_set_fill_color(ctx, GColorWhite);
  #endif
   graphics_fill_circle(ctx, GPoint(disc->pos.x, disc->pos.y), disc->radius);
}

static void disc_layer_update_callback(Layer *me, GContext *ctx) {
 disc_draw(ctx, &disc);
}

static void timer_callback(void *data) {
 AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };
 accel_service_peek(&accel);

 Disc *disc2 = &disc;
 disc_apply_accel(disc2, accel);
 disc_update(disc2);

 layer_mark_dirty(s_disc_layer);

 app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void main_window_load(Window *window) {
 Layer *window_layer = window_get_root_layer(window);

 GRect frame = window_frame = layer_get_frame(window_layer);

 // Create InverterLayer
 s_box_layer = inverter_layer_create(GRect(0, 0, BOX_SIZE-20, BOX_SIZE));
 layer_add_child(window_layer, inverter_layer_get_layer(s_box_layer));
 s_box_layer2 = inverter_layer_create(GRect(0, 0, BOX_SIZE-20, BOX_SIZE));
 layer_add_child(window_layer, inverter_layer_get_layer(s_box_layer2));

 s_disc_layer = layer_create(frame);
 layer_set_update_proc(s_disc_layer, disc_layer_update_callback);
 layer_add_child(window_layer, s_disc_layer);

  s_text_layer = text_layer_create(GRect(0, 0, 1100, 15));
  text_layer_set_text(s_text_layer, "Select to Start");
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

 disc_init(&disc);
}

static void main_window_unload(Window *window) {
 // Destroy InverterLayer
 inverter_layer_destroy(s_box_layer);
 inverter_layer_destroy(s_box_layer2);
 layer_destroy(s_disc_layer);
 text_layer_destroy(s_text_layer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
 // Start animation loop
 text_layer_set_text(s_text_layer, "ALIVE");
 next_animation();
 next_animation2();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void init(void) {
 // Create main Window
 s_main_window = window_create();
 window_set_background_color(s_main_window, GColorBlack);
 window_set_click_config_provider(s_main_window, click_config_provider);
 window_set_window_handlers(s_main_window, (WindowHandlers) {
   .load = main_window_load,
   .unload = main_window_unload,
 });
 window_stack_push(s_main_window, true);

  alien_main_window = window_create();
  window_set_fullscreen(alien_main_window, true);
  window_set_background_color(alien_main_window, GColorBlack);
  window_set_window_handlers(alien_main_window, (WindowHandlers) {
    .load = end_window_load,
    .unload = end_window_unload,
  });
  
 accel_data_service_subscribe(0, NULL);

 app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

static void deinit(void) {
 // Stop any animation in progress
 animation_unschedule_all();

 // Destroy main Window
 window_destroy(s_main_window); 
 window_destroy(alien_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
