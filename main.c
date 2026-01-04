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

enum TokenType {
  TOK_LBRACE,    // {
  TOK_RBRACE,    // }
  TOK_LBRACKET,  // [
  TOK_RBRACKET,  // ]
  TOK_COLON,     // :
  TOK_COMMA,     // ,
  TOK_STRING,
  TOK_NUMBER,
  TOK_TRUE,
  TOK_FALSE,
  TOK_NULL,
  TOK_EOF
};


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

typedef struct {
  enum TokenType type;
  union {
    jsonString str;
    double value;
  } value;
} Token;

struct jsonNode{
  jsonType type;
  jsonValue value;
};

typedef struct {
    const char* input;
    size_t pos;
    size_t length;
} Lexer;

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
    case JsonArr: for(int i=0; i<node->value.jsonArr.length;i++)
                    json_free(&node->value.jsonArr.data[i]);
                  free(node->value.jsonArr.data);
                  break;
    default: break;
  }
}

Token next_token(Lexer *lexer) {
    Token tok;
    while (lexer->pos < lexer->length && (lexer->input[lexer->pos] == ' '  ||
                                          lexer->input[lexer->pos] == '\n' ||
                                          lexer->input[lexer->pos] == '\t' || 
                                          lexer->input[lexer->pos] == '\r')) {
          lexer->pos++;
    }
    if(lexer->pos >= lexer->length) { 
      tok.type = TOK_EOF; return tok;
    }
    char c = lexer->input[lexer->pos];
    switch(c){
      case '{' : lexer->pos++;
                 tok.type = TOK_LBRACE;
                 return tok;
      case '}' : lexer->pos++;
                 tok.type = TOK_RBRACE;
                 return tok;
      case '[' : lexer->pos++;
                 tok.type = TOK_LBRACKET;
                 return tok;
      case ']' : lexer->pos++;
                 tok.type = TOK_RBRACKET;
                 return tok;
      case ':' : lexer->pos++;
                 tok.type =TOK_COLON;
                 return tok;
      case '"' : while(int i=0;i<lexer->pos;i++) 
    }
    
    fprintf(stderr, "Unexpected character: %c\n", c);
    exit(1);
  }

jsonNode parse_value() {

}
jsonNode parse_array();
jsonNode parse_object();


int main() {
   printf("Hello World\n");
   return 0;
}
