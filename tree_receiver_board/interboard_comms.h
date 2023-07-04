// This will have all the ESP-Now stuff that needs to be shared between
// all the boards. This will live in the libraries/ directory and be
// symlinked into all the specific board directories where we need it
// (create the symlink with `ln -s ../libaries/interboard_comms.h interboard_comms.h`
//  from the directory where you want it)

const uint8_t led_board_address[] = {0xC8, 0xF0, 0x9E, 0xF8, 0x6A, 0x60};
const uint8_t vessicle_1_address[] = {0xC8, 0xF0, 0x9E, 0xF3, 0x37, 0x8C};
const uint8_t vessicle_2_address[] = {0xC8, 0xF0, 0x9E, 0xF3, 0x37, 0x8C};


typedef struct Message {
  enum EventType {BUTTON_PRESSED, SET_COLOR};
  enum Sender {V1, V2, Tree};

  unsigned long time_sent;
  int hue;
  EventType event;
  Sender sender;
} message;


