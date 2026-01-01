#include<stdio.h>

typedef enum{
  JsonNum,
  JsonStr,
  JsonNull,
  JsonTrue,
  JsonFalse,
  JsonObj,
  JsonArr,
}jsonType;

typedef union{
  int jsonInt;
  char jsonStr[30];
}jsonValue;

struct jsonNode {
  jsonType type;
  jsonValue value;
};

int main() {
   printf("Hello World\n");
   return 0;
}
