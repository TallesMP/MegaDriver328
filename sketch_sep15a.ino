#include <LiquidCrystal_I2C.h>

typedef unsigned char uint8_t;
typedef signed char int8_t;
LiquidCrystal_I2C lcd(0x27,20,4);

struct Object {   
  uint8_t x;           
  uint8_t y;
  char type;
};

void drawObject(uint8_t x, uint8_t y, char object){
  lcd.setCursor(19 - y, x);
  lcd.print(object);
}

bool isColliding(uint8_t ax, uint8_t ay, uint8_t bx, uint8_t by) {
  if (ax == bx && ay == by){
    return true;
  }
  return false;
}

struct Object spawnObstacle(int cycle){
  struct Object obj;
  obj.x = (uint8_t) cycle % 4;
  obj.y = 0;
  obj.type = 'A';
  return obj;
}

int cycle = 0;
struct Object obj1 = spawnObstacle(cycle);

struct Object car = {2,19, 'C'};
struct Object obstacles[20];
obstacles[0] = obj1;

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  lcd.clear();


  for (int i; i < sizeof(obstacles) / sizeof(obstacles[0]);i++){
    struct Object obstacle = obstacles[i];
    drawObject(obstacle.x,obstacle.y++, obstacle.type);
  }
  drawObject(car.x,car.y,car.type);
  cycle++;
  delay(1);
}
