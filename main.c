#include<stdio.h>
#include<stddef.h>
#include<stdlib.h>
#include<string.h>

typedef enum{
  JsonNum,
  JsonStr,
  JsonNull,
  JsonTrue,
  JsonFalse,
  JsonObj,
  JsonArr,
}jsonType;

typedef struct {
  char *data;
  size_t length;
}jsonString;

typedef struct jsonNode jsonNode;   

typedef struct {
  jsonNode *data;
  size_t length;
}jsonArray;

typedef union{
  double jsonNum;
  jsonString jsonStr;
  jsonArray jsonArr;
}jsonValue;

struct jsonNode{
  jsonType type;
  jsonValue value;
};

jsonNode create_str(char *data, size_t len) {
    jsonString loc_str;
    loc_str.data = malloc(len);
    loc_str.length = len;
    memcpy(loc_str.data,data,len);
    jsonNode new_node;
    new_node.type = JsonStr;
    new_node.value.jsonStr = loc_str;
    
    return new_node;
}

jsonNode create_arr(size_t len) {
  jsonArray loc_arr;
  loc_arr.data = malloc(len * sizeof(jsonNode));
  loc_arr.length = len;
  jsonNode new_node;
  new_node.type = JsonArr;
  new_node.value.jsonArr = loc_arr;
  return new_node;
}

void json_free(jsonNode *node) {
  if (node==NULL) return;

  switch(node->type){
    case JsonStr: free(node->value.jsonStr.data);
                  break;
    case JsonArr: for(int i=0; i>)
                    json_free(&element);
                  free(array.data);
    default: break;
  }
}

int main() {
   printf("Hello World\n");
   return 0;
}
