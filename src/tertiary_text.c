#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x71, 0xBB, 0xDC, 0xBD, 0xED, 0xB1, 0x44, 0xAD, 0xBA, 0x39, 0x4D, 0xB5, 0xBD, 0x5E, 0xE5, 0x69 }
PBL_APP_INFO_SIMPLE(MY_UUID, "Tertiary Text", "VGMoose & CCiollar", 1 /* App version */);

#define TOP 0
#define MID 1
#define BOT 2

Window window;

TextLayer textLayer;
TextLayer wordsYouWrite;

TextLayer buttons1[3];
TextLayer buttons2[3];
TextLayer buttons3[3];
TextLayer* bbuttons[] = {buttons1, buttons2, buttons3};

//Set of enumerated keys with names and values corresponding android app keys for communication
enum {
  SELECT_KEY = 0x03,
  UP_KEY = 0x01,
  DOWN_KEY = 0x02,
  DATA_KEY = 0x0
};

bool menu = false;

// Here are the three cases, or sets
char caps[] =    "ABCDEFGHIJKLM NOPQRSTUVWXYZ";
char letters[] = "abcdefghijklm nopqrstuvwxyz";
char numsym[] = "1234567890!?-'\"$()&*+#:@/,.";

// the below three strings just have to be unique, abc - xyz will be overwritten with the long strings above
char* btext1[] = {"abc", "def", "ghi"};
char* btext2[] = {"jkl", "m n", "opq"};
char* btext3[] = {"rst", "uvw", "xyz"};
char** btexts[] = {btext1, btext2, btext3};

// These are the actual sets that are displayed on each button, also need to be unique
char set1[3] = "  a";
char set2[3] = "  b";
char set3[3] = "  c";
char* setlist[] = {set1, set2, set3};

char* cases[] = {"CAP", "low", "#@1"};

int cur_set = 1;
bool blackout = false;

void drawSides();
void drawMenu();
void set_menu();

char* rotate_text[] = {caps, letters, numsym};
void next();

char* master = letters;

char text_buffer[60];
int pos = 0;
int top, end, size;





// This function changes the next case/symbol set.
void change_set(int s, bool lock)
{
    int count = 0;
    master = rotate_text[s];
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<3; j++)
        {
            for (int k=0; k<3; k++)
            {
                btexts[i][j][k] = master[count];
                count++;
            }
        }
    }
    
    menu = false;
    if (lock) cur_set = s;
    
    drawSides();
}

void next()
{
    top = 0;
    end = 26;
    size = 27;
}

// These are to prevent missed clicks on a hold
void up_long_release_handler(ClickRecognizerRef recognizer, Window *window) {}
void select_long_release_handler(ClickRecognizerRef recognizer, Window *window) {}
void down_long_release_handler(ClickRecognizerRef recognizer, Window *window) {}

void clickButton(int b)
{
    if (!blackout)
    {
        if (menu)
        {
            change_set(b, false);
            return;
        }
        
        if (size > 3)
        {
            size /= 3;
            if (b == TOP)
                end -= 2*size;
            else if (b == MID)
            {
                top += size;
                end -= size;
            }
            else if (b == BOT)
                top += 2*size;
        }
        else
        {
            text_buffer[pos++] = master[top+b];
            text_layer_set_text(&wordsYouWrite, text_buffer);
            change_set(cur_set, false);
            next();
        }
    
        drawSides();
    }
    
}

// Modify these common button handlers
void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
    
    clickButton(TOP);
   
}

void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
   
    clickButton(MID);
}

void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    clickButton(BOT);
}

bool common_long(int b)
{
    if (menu)
    {
        change_set(b, true);
        return true;
    }
    return false;
}

void up_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    if (common_long(TOP)) return;
    
    set_menu();
    
}

void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  (void)recognizer;
  (void)window;
    
    if (common_long(MID)) return;
    
    blackout = !blackout;

    if (blackout)
        text_layer_set_background_color(&textLayer, GColorBlack);
    else
         text_layer_set_background_color(&textLayer, GColorClear);

    //Send select key to test app
    
    //Create a key-value pair
    Tuplet value = TupletInteger(DATA_KEY, SELECT_KEY);
  
    //Construct the dictionary
    DictionaryIterator *iter;
    app_message_out_get(&iter);

    //If it failed
    if (iter == NULL){
      return;
    }

    //Write to dictionary
    dict_write_cstring(iter, DATA_KEY, text_buffer );
    dict_write_end(iter);

    //Send the dictionary and release the buffer
    app_message_out_send();
    app_message_out_release();
}


void out_sent_handler(DictionaryIterator *sent, void *context) {
        //Notify the watch app user the send was successful
  //text_layer_set_text(&textLayer, "Send successful!");
}

/**
        * Handler for AppMessage send failed
        */
static void out_fail_handler(DictionaryIterator* failed, AppMessageResult reason, void* context) {
        //Notify the watch app user that the send operation failed
  //text_layer_set_text(&textLayer, "Send Failed");
}

/**
        *       Handler for received AppMessage
        */
static void in_received_handler(DictionaryIterator* iter, void* context) {

        //Create a tuple from the received dictionary using the key     
  //Tuple *in_tuple = dict_find(iter, DATA_KEY);

        //If the tuple was successfully obtained
  //if (in_tuple)

        //Show the recieved string on the watch!
    //text_layer_set_text(&textLayer, in_tuple->value->cstring);
}

/**
        * Handler for received message dropped
        */
void in_drop_handler(void *context, AppMessageResult reason) {
        //Notify the watch app user that the recieved message was dropped
      //  text_layer_set_text(&textLayer, "Message dropped!");
}

void down_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
    (void)recognizer;
    (void)window;
    
    if (common_long(BOT)) return;
    
    // delete or cancel when back is held
    
    if (size==27 && pos>0 && !blackout)
    {
        text_buffer[--pos] = ' ';
        text_layer_set_text(&wordsYouWrite, text_buffer);
    }
    else
    {
        next();
        drawSides();
    }

}

void set_menu()
{
    if (!blackout)
    {
        menu = true;
        drawMenu();
    }
}

// This usually won't need to be modified

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;
    
    config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
    config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;
    config[BUTTON_ID_UP]->long_click.handler = (ClickHandler) up_long_click_handler;
    config[BUTTON_ID_UP]->long_click.release_handler = (ClickHandler) up_long_release_handler;
    
    config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
    config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 100;
    config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;
    config[BUTTON_ID_SELECT]->long_click.release_handler = (ClickHandler) select_long_release_handler;
    
    config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
    config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
    config[BUTTON_ID_DOWN]->long_click.handler = (ClickHandler) down_long_click_handler;
    config[BUTTON_ID_DOWN]->long_click.release_handler = (ClickHandler) down_long_release_handler;

}

void drawMenu()
{
    for (int i=0; i<3; i++)
    {
        text_layer_set_text(&bbuttons[i][i!=2], " ");
        text_layer_set_text(&bbuttons[i][2], " ");
        
        text_layer_set_text(&bbuttons[i][i==2], cases[i]);
        text_layer_set_font(&bbuttons[i][0], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    }
}


// This method draws the characters on the right side near the buttons
void drawSides()
{
    if (size==27) // first click (full size)
    {
        // update all 9 labels to their proper values
        for (int h=0; h<3; h++)
        {
            for (int i=0; i<3; i++)
            {
                text_layer_set_text(&bbuttons[h][i], btexts[h][i]);
                text_layer_set_background_color(&bbuttons[h][i], GColorClear);
                text_layer_set_font(&bbuttons[h][i], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
            }

        }
    }
    else if (size==9)   // second click
    {
        
        for (int i=0; i<3; i++)
        {
            text_layer_set_text(&bbuttons[i][i!=2], " ");
            text_layer_set_text(&bbuttons[i][2], " ");
            
            text_layer_set_text(&bbuttons[i][i==2], btexts[top/9][i]);
            text_layer_set_font(&bbuttons[i][i==2], fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        }
                
    } else if (size == 3)
    {
        for (int i=0; i<3; i++)
        {
            setlist[i][2] = master[top+i];
            text_layer_set_text(&bbuttons[i][i==2], setlist[i]);

        }
    } 
    
}

void initSidesAndText()
{
    text_layer_init(&wordsYouWrite, GRect(10, 0, 100, 150));
    text_layer_set_background_color(&wordsYouWrite, GColorClear);
    text_layer_set_font(&wordsYouWrite, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    layer_add_child(&window.layer, &wordsYouWrite.layer);


    for (int i = 0; i<3; i++)
    {
        text_layer_init(&buttons1[i], GRect(115, 12*i, 100, 100));
        text_layer_init(&buttons2[i], GRect(115, 12*i+50, 100, 100));
        text_layer_init(&buttons3[i], GRect(115, 12*i+100, 100, 100));
    }
    
    for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
            layer_add_child(&window.layer, &bbuttons[i][j].layer);

}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Text Input");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&FONT_DEMO_RESOURCES);

  text_layer_init(&textLayer, window.layer.frame);
//  text_layer_set_text(&textLayer, text_buffer);
    text_layer_set_background_color(&textLayer, GColorClear);

  text_layer_set_font(&textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(&window.layer, &textLayer.layer);
    
    initSidesAndText();
    drawSides();
    
  // Attach our desired button functionality
  window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
    
}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    .messaging_info = {
      //Buffer Size
      .buffer_sizes = {
        .inbound = 64,
        .outbound = 16,
      },

      .default_callbacks.callbacks = {
        .out_sent = out_sent_handler,
        .out_failed = out_fail_handler,
        .in_received = in_received_handler,
        .in_dropped = in_drop_handler,
      },
   }
   };
  change_set(1, true);
  next();
  app_event_loop(params, &handlers);
}
