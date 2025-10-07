#include <LiquidCrystal_I2C.h>

#define LCD_WIDTH 20
#define LCD_HEIGHT 4
#define MAX_OBSTACLES 20
#define GAME_DELAY 200
#define SPAWN_CHANCE 20

const int buttonLeftPin = 2;
const int buttonRightPin = 3;

typedef unsigned char uint8_t;
typedef signed char int8_t;

LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);

struct Object {
  uint8_t x;
  uint8_t y;
  char type;
};

struct Object car = {1, LCD_WIDTH - 1, 'C'};
struct Object obstacles[MAX_OBSTACLES];

int buttonLeftState = 0;
int buttonRightState = 0;


pinMode(buttonLeftPin, INPUT);
pinMode(buttonRightPin, INPUT);

void handleButtons() {
  buttonLeftState = digitalRead(buttonLeftPin);
  buttonRightState = digitalRead(buttonRightPin);
  
  if (buttonLeftState == HIGH && car.x > 0) {
    car.x--;
  }
  else if (buttonRightState == HIGH && car.x < LCD_HEIGHT - 1) {
    car.x++;
  }
}

void drawObject(const struct Object& obj) {
  lcd.setCursor(LCD_WIDTH - 1 - obj.y, obj.x);
  lcd.print(obj.type);
}

bool isColliding(const struct Object& a, const struct Object& b) {
  return a.x == b.x && a.y == b.y;
}

struct Object spawnObstacle() {
  struct Object obj;
  obj.x = (uint8_t)random(0, LCD_HEIGHT);
  obj.y = 0;
  obj.type = 'A';
  return obj;
}

void updateObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type == 0) continue;

    if (isColliding(car, obstacles[i])) {
      drawObject(obstacles[i]);
      return;
    }

    drawObject(obstacles[i]);
    if (obstacles[i].y >= LCD_WIDTH - 1) {
      obstacles[i].type = 0;
    } else {
      obstacles[i].y++;
    }
  }
}

void addObstacle() {
  if (random(100) < SPAWN_CHANCE) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
      if (obstacles[i].type == 0) {
        obstacles[i] = spawnObstacle();
        break;
      }
    }
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  randomSeed(analogRead(0));
  obstacles[0] = spawnObstacle();
}

void loop() {
  lcd.clear();
  
  handleButtons();
  updateObstacles();
  drawObject(car);
  addObstacle();
  
  delay(GAME_DELAY);
}
