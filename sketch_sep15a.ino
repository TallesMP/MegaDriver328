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

struct Object spawnObstacle(){
  struct Object obj;
  uint8_t rand = (uint8_t) random(0,3);
  obj.x = rand;
  obj.y = 0;
  obj.type = "A";
  return obj;
}

int cycle = 0;
struct Object obj1 = spawnObstacle();

void setup() {
  lcd.init();
  lcd.backlight();
}

void loop() {
  cycle++;
  drawObject(obj1.x,obj1.y, obj1.type);
  drawObject(1,19, 'C');
  delay(15);
}
