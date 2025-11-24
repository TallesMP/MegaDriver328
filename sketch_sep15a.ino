#include <LiquidCrystal_I2C.h>

#define LCD_WIDTH 20
#define LCD_HEIGHT 4
#define MAX_OBSTACLES 50
#define GAME_DELAY 100
#define SPAWN_CHANCE_NORMAL 20 
#define  SPAWN_CHANCE_WITH_SIDEROFAGO 15 

byte CARacter[]       = { B00000, B10001, B01100, B11011, B11011, B01100, B10001, B00000 };
byte plane[]          = { B00000, B01100, B01101, B11111, B01101, B01100, B00000, B00000 };
byte bomb[]           = { B00000, B10101, B01110, B11111, B01110, B10101, B00000, B00000 };
byte siderofago[]     = { B00010, B11111, B01011, B11111, B11111, B01011, B11111, B00010 };
byte siderofago_corpo[] = { B00100, B11111, B11111, B11011, B11111, B11111, B11111, B00100 };

uint8_t blockTick = 0;

const int buttonLeftPin  = 2;
const int buttonRightPin = 4;

enum ObjectType : byte {
  OBJ_NONE = 0,
  OBJ_CAR,
  OBJ_OBSTACLE_BOMB,
  OBJ_OBSTACLE_PLANE
};

struct Object {
  uint8_t x, y;
  ObjectType type;
  uint8_t speed;
};

struct Siderofago {
  bool active = false;
  uint8_t line = 0;
  int headPos = 0;      // posição da cabeça (aumenta da esquerda para direita)
  uint8_t length = 0;   // tamanho do corpo (sem contar a cabeça)
  uint8_t tickCount = 0;
};

LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);
Object car = {1, LCD_WIDTH - 1, OBJ_CAR, 0};
Object obstacles[MAX_OBSTACLES];
Siderofago siderofagoObj;

bool gameOver = false;
unsigned long startTime = 0;
unsigned long score = 0;

unsigned long lastSiderofagoTime = 0;
const unsigned long SIDEROFAGO_COOLDOWN = 10000; // milissegundos 

void handleButtons() {
  if (digitalRead(buttonLeftPin) == HIGH && car.x > 0) car.x--;
  if (digitalRead(buttonRightPin) == HIGH && car.x < LCD_HEIGHT - 1) car.x++;
}

bool isColliding(uint8_t x, uint8_t y) {
  return (car.x == x && car.y == y);
}

void drawObject(const Object& obj) {
  lcd.setCursor(LCD_WIDTH - 1 - obj.y, obj.x);
  switch (obj.type) {
    case OBJ_CAR:              lcd.write(0); break;
    case OBJ_OBSTACLE_BOMB:    lcd.write(1); break;
    case OBJ_OBSTACLE_PLANE:   lcd.write(2); break;
  }
}

void drawSiderofago() {
  if (!siderofagoObj.active) return;

  // Cabeça
  int headCol = LCD_WIDTH - 1 - siderofagoObj.headPos;
  if (headCol >= 0 && headCol < LCD_WIDTH) {
    lcd.setCursor(headCol, siderofagoObj.line);
    lcd.write(4);
  }

  // Corpo
  for (int i = 1; i <= siderofagoObj.length; i++) {
    int col = LCD_WIDTH - 1 - (siderofagoObj.headPos - i);
    if (col >= 0 && col < LCD_WIDTH) {
      lcd.setCursor(col, siderofagoObj.line);
      lcd.write(5);
    }
  }
}

void updateObstacles() {
  blockTick++;
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type == OBJ_NONE) continue;

    if (isColliding(obstacles[i].x, obstacles[i].y)) {
      gameOver = true;
      return;
    }

    if (obstacles[i].type == OBJ_OBSTACLE_BOMB) {
      if (blockTick % 2 == 0) obstacles[i].y++;
    } else {
      obstacles[i].y++;
    }

    if (obstacles[i].y >= LCD_WIDTH) obstacles[i].type = OBJ_NONE;
  }
}

void updateSiderofago() {
  if (!siderofagoObj.active) return;

  siderofagoObj.tickCount++;
  if (siderofagoObj.tickCount % 1 == 0) {
    siderofagoObj.headPos++;

    // Colisão com a cabeça
    int headCol = LCD_WIDTH - 1 - siderofagoObj.headPos;
    if (headCol >= 0 && headCol < LCD_WIDTH) {
      if (isColliding(siderofagoObj.line, siderofagoObj.headPos)) {
        gameOver = true;
        return;
      }
    }

    // Desativa somente quando a cauda saiu completamente
    if (siderofagoObj.headPos >= LCD_WIDTH + siderofagoObj.length) {
      siderofagoObj.active = false;
    }
  }
}

int getCurrentSpawnChance() {
  if (siderofagoObj.active) {
    return SPAWN_CHANCE_WITH_SIDEROFAGO; 
  } else {
    return SPAWN_CHANCE_NORMAL;
  }
}

void addObstacle() {
  if (random(100) < getCurrentSpawnChance()) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
      if (obstacles[i].type == OBJ_NONE) {
        obstacles[i].x = random(0, LCD_HEIGHT);
        obstacles[i].y = 0;
        obstacles[i].type = (random(100) < 50) ? OBJ_OBSTACLE_BOMB : OBJ_OBSTACLE_PLANE;
        break;
      }
    }
  }

  if (millis() - lastSiderofagoTime > SIDEROFAGO_COOLDOWN && random(600) < 3) {
    siderofagoObj.active = true;
    siderofagoObj.line = random(0, LCD_HEIGHT);
    siderofagoObj.headPos = 0;
    siderofagoObj.length = random(20, 41);
    siderofagoObj.tickCount = 0;
    lastSiderofagoTime = millis();
  }
}

void showStartScreen() {
  lcd.clear();
  lcd.setCursor(4, 0);  lcd.print("ATMEGADRIVER");
  lcd.setCursor(3, 2);  lcd.print("Press a button");
  lcd.setCursor(6, 3);  lcd.print("to START");

  while (digitalRead(buttonLeftPin) == LOW && digitalRead(buttonRightPin) == LOW) {
    delay(50);
  }
  lcd.clear();
  startTime = millis();
}

void showGameOverScreen() {
  score = (millis() - startTime) / 100;
  lcd.clear();
  lcd.setCursor(5, 1); lcd.print("GAME OVER!");
  lcd.setCursor(5, 2); lcd.print("Score: ");
  lcd.print(score);
}

void setup() {
  lcd.init();
  lcd.backlight();
  randomSeed(analogRead(0));

  pinMode(buttonLeftPin,  INPUT);
  pinMode(buttonRightPin, INPUT);

  for (int i = 0; i < MAX_OBSTACLES; i++) obstacles[i].type = OBJ_NONE;

  lcd.createChar(0, CARacter);
  lcd.createChar(1, bomb);
  lcd.createChar(2, plane);
  lcd.createChar(4, siderofago);
  lcd.createChar(5, siderofago_corpo);

  showStartScreen();
}

void loop() {
  if (gameOver) {
    showGameOverScreen();
    while (true);
  }

  lcd.clear();

  handleButtons();
  updateObstacles();
  updateSiderofago();
  addObstacle();

  drawObject(car);
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type != OBJ_NONE) drawObject(obstacles[i]);
  }
  drawSiderofago();

  delay(GAME_DELAY);
}
