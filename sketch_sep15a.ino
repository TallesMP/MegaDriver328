#include <LiquidCrystal_I2C.h>

#define LCD_WIDTH 20
#define LCD_HEIGHT 4
#define MAX_OBSTACLES 20
#define GAME_DELAY 200
#define SPAWN_CHANCE 20

// Definição do caractere personalizado
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

const int buttonLeftPin = 2;
const int buttonRightPin = 4;

typedef unsigned char uint8_t;
typedef signed char int8_t;

LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);

struct Object {
  uint8_t x;
  uint8_t y;
  byte type;  // Altere para byte, que irá armazenar o índice do caractere personalizado
};

struct Object car = {1, LCD_WIDTH - 1, 0};  // O índice 0 será o carro, conforme criado com createChar()
struct Object obstacles[MAX_OBSTACLES];

int buttonLeftState = 0;
int buttonRightState = 0;
bool gameOver = false;

void handleButtons(struct Object *car) {
  buttonLeftState = digitalRead(buttonLeftPin);
  buttonRightState = digitalRead(buttonRightPin);

  if (buttonLeftState == HIGH && car->x > 0) {
    car->x--;
  } 
  else if (buttonRightState == HIGH && car->x < LCD_HEIGHT - 1) {
    car->x++;
  }
}

void drawObject(const struct Object& obj) {
  lcd.setCursor(LCD_WIDTH - 1 - obj.y, obj.x);
  lcd.write(obj.type);  // Exibe o caractere personalizado
}

bool isColliding(const struct Object& a, const struct Object& b) {
  return a.x == b.x && a.y == b.y;
}

struct Object spawnObstacle() {
  struct Object obj;
  obj.x = (uint8_t)random(0, LCD_HEIGHT);
  obj.y = 0;
  obj.type = '#';  // Obstáculo genérico
  return obj;
}

void updateObstacles() {
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type == 0) continue;

    // Verifica colisão
    if (isColliding(car, obstacles[i])) {
      gameOver = true;
      return;
    }

    // Move obstáculo
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

void showStartScreen() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("ATMEGADRIVER");
  lcd.setCursor(3, 2);
  lcd.print("Press a button");
  lcd.setCursor(6, 3);
  lcd.print("to START");

  // Espera botão
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
    obstacles[i].type = 0;
  }

  // Cria o caractere personalizado com o índice 0
  lcd.createChar(0, CARacter);

  showStartScreen();
}

void loop() {
  if (gameOver) {
    showGameOverScreen();
    while (true); // trava o jogo aqui pra sempre
  }

  lcd.clear();

  handleButtons(&car);
  updateObstacles();
  addObstacle();

  // Desenha tudo
  drawObject(car);
  for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (obstacles[i].type != 0) drawObject(obstacles[i]);
  }

  delay(GAME_DELAY);
}

