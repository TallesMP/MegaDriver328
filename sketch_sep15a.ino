#include <LiquidCrystal_I2C.h>

#define LCD_WIDTH 20
#define LCD_HEIGHT 4
#define MAX_OBSTACLES 20
#define GAME_DELAY 100
#define SPAWN_CHANCE 20

byte CARacter[] = { 
  B00000, 
  B10001, 
  B01100, 
  B11011, 
  B11011, 
  B01100, 
  B10001, 
  B00000 
};

byte plane[] = { 
  B00000, 
  B01100, 
  B01101, 
  B11111, 
  B01101, 
  B01100, 
  B00000, 
  B00000 
};
byte bomb[] = { B00000, B10101, B01110, B11111, B01110, B10101, B00000, B00000 };

uint8_t blockTick = 0;

const int buttonLeftPin = 2;
const int buttonRightPin = 4;

typedef unsigned char uint8_t;

enum ObjectType : byte {
  OBJ_NONE = 0,
  OBJ_CAR,
  OBJ_OBSTACLE_BOMB,
  OBJ_OBSTACLE_PLANE
};

typedef struct {
  uint8_t x;
  uint8_t y;
  ObjectType type;
  uint8_t speed;
  uint8_t tickCounter;
} Object;

LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);

Object car = {1, LCD_WIDTH - 1, OBJ_CAR, 0, 0};
Object obstacles[MAX_OBSTACLES];

int buttonLeftState = 0;
int buttonRightState = 0;
bool gameOver = false;

void handleButtons(Object *car) {
  buttonLeftState = digitalRead(buttonLeftPin);
  buttonRightState = digitalRead(buttonRightPin);

  if (buttonLeftState == HIGH && car->x > 0) {
    car->x--;
  } 
  else if (buttonRightState == HIGH && car->x < LCD_HEIGHT - 1) {
    car->x++;
  }
}

bool isColliding(const Object& a, const Object& b) {
  return a.x == b.x && a.y == b.y;
}

void drawObject(const Object& obj) {
  lcd.setCursor(LCD_WIDTH - 1 - obj.y, obj.x);

  switch (obj.type) {
    case OBJ_CAR:
      lcd.write((byte)0);
      break;
    case OBJ_OBSTACLE_BOMB:
      lcd.write((byte)1);
      break;
    case OBJ_OBSTACLE_PLANE:
      lcd.write((byte)2);
      break;
    default:
      break;
  }
}

Object spawnObstacle() {
  Object obj;
  obj.x = (uint8_t)random(0, LCD_HEIGHT);
  obj.y = 0;
  obj.tickCounter = 0;

  if (random(100) < 50) {
    obj.type = OBJ_OBSTACLE_BOMB;
    obj.speed = 2;
  } else {
    obj.type = OBJ_OBSTACLE_PLANE;
    obj.speed = 1;
  }

  return obj;
}


void updateObstacles() {
  blockTick++;

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type == OBJ_NONE) continue;

    if (isColliding(car, obstacles[i])) {
      gameOver = true;
      return;
    }

    if (obstacles[i].type == OBJ_OBSTACLE_BOMB) {
      if (blockTick % 2 == 0) {
        if (obstacles[i].y >= LCD_WIDTH - 1) {
          obstacles[i].type = OBJ_NONE;
        } else {
          obstacles[i].y++;
        }
      }
    } else if (obstacles[i].type == OBJ_OBSTACLE_PLANE) {
      obstacles[i].y++;
      if (obstacles[i].y >= LCD_WIDTH - 1) {
        obstacles[i].type = OBJ_NONE;
      }
    }

    if (isColliding(car, obstacles[i])) {
      gameOver = true;
      return;
    }
  }
}

void addObstacle() {
  if (random(100) < SPAWN_CHANCE) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
      if (obstacles[i].type == OBJ_NONE) {
        obstacles[i] = spawnObstacle();
        break;
      }
    }
  }
}

void showStartScreen() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("ATMEGADRIVER");
  lcd.setCursor(3, 2);
  lcd.print("Press a button");
  lcd.setCursor(6, 3);
  lcd.print("to START");

  while (digitalRead(buttonLeftPin) == LOW && digitalRead(buttonRightPin) == LOW) {
    delay(50);
  }

  lcd.clear();
}

void showGameOverScreen() {
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("GAME OVER!");
}

void setup() {
  lcd.init();
  lcd.backlight();
  randomSeed(analogRead(0));

  pinMode(buttonLeftPin, INPUT);
  pinMode(buttonRightPin, INPUT);

  for (int i = 0; i < MAX_OBSTACLES; i++) {
    obstacles[i].type = OBJ_NONE;
  }

  lcd.createChar(0, CARacter);
  lcd.createChar(1, bomb);
  lcd.createChar(2, plane);


  showStartScreen();
}

void loop() {
  if (gameOver) {
    showGameOverScreen();
    while (true);
  }

  lcd.clear();

  handleButtons(&car);
  updateObstacles();
  addObstacle();

  drawObject(car);
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type != OBJ_NONE) drawObject(obstacles[i]);
  }

  delay(GAME_DELAY);
}

