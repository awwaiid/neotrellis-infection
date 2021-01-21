
#include "Adafruit_NeoTrellis.h"

#define Y_DIM 8 //number of rows of key
#define X_DIM 8 //number of columns of keys

#define BRIGHTNESS 10

// Create a matrix of trellis panels
Adafruit_NeoTrellis trellis_array[Y_DIM/4][X_DIM/4] = {
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x2F) },
  { Adafruit_NeoTrellis(0x30), Adafruit_NeoTrellis(0x32) }
};

// Pass this matrix to the multitrellis object
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)trellis_array, Y_DIM/4, X_DIM/4);

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t wheel(byte wheel_pos) {
  if(wheel_pos < 85) {
    return seesaw_NeoPixel::Color(wheel_pos * 3, 255 - wheel_pos * 3, 0);
  } else if(wheel_pos < 170) {
    wheel_pos -= 85;
    return seesaw_NeoPixel::Color(255 - wheel_pos * 3, 0, wheel_pos * 3);
  } else {
    wheel_pos -= 170;
    return seesaw_NeoPixel::Color(0, wheel_pos * 3, 255 - wheel_pos * 3);
  }
  return 0;
}

int currentLed[X_DIM * Y_DIM];

int board[X_DIM][Y_DIM] = {
  { 1, 0, 0, 0, 0, 0, 0, 2 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 2, 0, 0, 0, 0, 0, 0, 1 }
};

int current_player = 1;
int has_selected = 0;
int selected_x;
int selected_y;

int distance(int x1, int y1, int x2, int y2) {
  return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

// qsort requires you to create a sort function
int sort_desc(const void *cmp1, const void *cmp2) {
  // Need to cast the void * to int *
  int a = *((int *)cmp1);
  int b = *((int *)cmp2);
  // The comparison
  // return a > b ? -1 : (a < b ? 1 : 0);
  // A simpler, probably faster way:
  return b - a;
}

int sort_board() {
  // for(int x = 0; x < 8; x++) {
  //   qsort(board[x], 8, sizeof(board[x][0]), sort_desc);
  // }

  int player1_count = 0;
  int player2_count = 0;
  int blank_count   = 0;
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      if(board[x][y] == 0) blank_count++;
      if(board[x][y] == 1) player1_count++;
      if(board[x][y] == 2) player2_count++;
    }
  }
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      if(player1_count > 0) {
        board[x][y] = 1;
        player1_count--;
      } else if(blank_count > 0) {
        board[x][y] = 0;
        blank_count--;
      } else if(player2_count > 0) {
        board[x][y] = 2;
        player2_count--;
      }
    }
  }
}

void set_brightness(int level) {
	for (int x = 0; x < X_DIM / 4; x++) {
		for (int y = 0; y < Y_DIM / 4; y++) {
		  trellis_array[y][x].pixels.setBrightness(level);
		}
	}
}

int draw_board() {
  int valid_move_count = 0;
  int current_player_piece_count = 0;
  int empty_space_count = 0;
  for(int board_x = 0; board_x < 8; board_x++) {
    for(int board_y = 0; board_y < 8; board_y++) {
      empty_space_count++;
      Serial.print(board[board_x][board_y]);
      Serial.print(" ");
      if(board[board_x][board_y] == 0) {
        if(has_selected && distance(board_x, board_y, selected_x, selected_y) == 1) {
          set_brightness(20);
          trellis.setPixelColor(board_x, board_y, wheel( current_player * 100 - 20 ));
          valid_move_count++;
        } else if(has_selected && distance(board_x, board_y, selected_x, selected_y) == 2) {
          set_brightness(20);
          trellis.setPixelColor(board_x, board_y, wheel( current_player * 100 - 50));
          valid_move_count++;
        } else {
          trellis.setPixelColor(board_x, board_y, 0x000000);
        }
      } else {
        if(board[board_x][board_y] == current_player) {
          current_player_piece_count++;
        }
        if(has_selected && selected_x == board_x && selected_y == board_y) {
          set_brightness(200);
          trellis.setPixelColor(board_x, board_y, wheel( board[board_x][board_y] * 100 ));
        } else {
          set_brightness(20);
          trellis.setPixelColor(board_x, board_y, wheel( board[board_x][board_y] * 100 ));
        }
      }
    }
    Serial.println("");
  }
  trellis.show();

  // Game over!
  if(has_selected && valid_move_count == 0 || current_player_piece_count == 0 || empty_space_count == 0) {
    sort_board();
  }

  return valid_move_count;
}

void mutate_neighbors(int current_player, int x, int y) {
  for(int i = x - 1; i <= x + 1; i++) {
    if(i < 0) continue;
    if(i > 7) continue;
    for(int j = y - 1; j <= y + 1; j++) {
      if(j < 0) continue;
      if(j > 7) continue;
      if(board[i][j] > 0) {
        board[i][j] = current_player;
      }
    }
  }
}

// Define a callback for key presses
TrellisCallback blink(keyEvent evt) {

  // Rising == push, Falling == release
  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
    Serial.print("Player: ");
    Serial.print(current_player);

    int x = evt.bit.NUM % 8;
    int y = evt.bit.NUM / 8;

    Serial.print(" x: ");
    Serial.print(x);
    Serial.print(" y: ");
    Serial.print(y);

    Serial.print(" current: ");
    Serial.println(board[x][y]);

    if(!has_selected && board[x][y] == current_player) {
      has_selected = 1;
      selected_x = x;
      selected_y = y;
    } else if(has_selected && board[x][y] == 0) {
      // Open square
      int dist = distance(selected_x, selected_y, x, y);
      if(dist == 1) {
        // spread
        board[x][y] = current_player;
        has_selected = 0;
        mutate_neighbors(current_player, x, y);
        current_player = current_player % 2 + 1;
      } else if(dist == 2) {
        // jump
        board[selected_x][selected_y] = 0;
        board[x][y] = current_player;
        has_selected = 0;
        mutate_neighbors(current_player, x, y);
        current_player = current_player % 2 + 1;
      } else {
        // invalid; un-select
        has_selected = 0;
      }
    } else {
      // invalid; un-select
      has_selected = 0;
    }

    draw_board();

  } else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING) {
    // trellis.setPixelColor(evt.bit.NUM, 0); // Off falling
  }

  return 0;
}

void setup() {
  Serial.begin(9600);

  if(!trellis.begin()){
    Serial.println("Failed to begin trellis!");
    while(1);
  } else {
    Serial.println("Trellis started!");
  }

  set_brightness(BRIGHTNESS);

  // Fancy random initialization display
  for(int n = 0; n < 10; n++) {
    for(int i=0; i < Y_DIM * X_DIM; i++) {
      trellis.setPixelColor(i, random(0x1000000));
    }
    trellis.show();
    delay(50);
  }

  // Hook up callbacks for every button
  for(int y=0; y<Y_DIM; y++){
    for(int x=0; x<X_DIM; x++){
      // activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, blink);
      trellis.setPixelColor(x, y, 0x000000); // Addressed with x,y
      trellis.show(); // Show all LEDs
    }
    delay(5);
  }

  draw_board();
}

void loop() {
  trellis.read();
  delay(20);
}
