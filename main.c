
#include<stdio.h>
#include<ctype.h>
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
typedef struct jsonPair jsonPair;

typedef struct {
  jsonNode *data;
  size_t length;
}jsonArray;

typedef struct {
  jsonPair *data;
  size_t length;
}jsonObject;

typedef union{
  double jsonNum;
  jsonString jsonStr;
  jsonArray jsonArr;
  jsonObject jsonObj;
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

struct jsonPair{
  jsonString key;
  jsonNode value;
};

typedef struct {
    const char* input;
    size_t pos;
    size_t length;
} Lexer;

typedef struct {
  Lexer lexer;
  Token current;
} Parser;

jsonNode create_str(const char *data, size_t len) {
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

jsonNode create_obj(size_t len) {
  jsonObject loc_obj;
  loc_obj.data = malloc(len * sizeof(jsonPair));
  loc_obj.length = len;
  jsonNode new_node;
  new_node.type = JsonObj;
  new_node.value.jsonObj = loc_obj;
  return new_node;
}

jsonNode create_num(double value) {
  jsonNode new_node;
  new_node.type = JsonNum;
  new_node.value.jsonNum = value;
  return new_node;
}

jsonNode create_literal(jsonType type) {
  jsonNode new_node;
  new_node.type = type;
  return new_node;
}

void json_free(jsonNode *node) {
  if (node==NULL) return;

  switch(node->type){
    case JsonStr: free(node->value.jsonStr.data);
                  break;
    case JsonArr: for(size_t i=0; i<node->value.jsonArr.length;i++)
                    json_free(&node->value.jsonArr.data[i]);
                  free(node->value.jsonArr.data);
                  break;
    case JsonObj: for(size_t i=0; i<node->value.jsonObj.length;i++) {
                    free(node->value.jsonObj.data[i].key.data);
                    json_free(&node->value.jsonObj.data[i].value);
                  }
                  free(node->value.jsonObj.data);
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
      case ',' : lexer->pos++;
                 tok.type = TOK_COMMA;
                 return tok;
      case '"' :  lexer->pos++;
                   size_t start = lexer->pos;
                  while(lexer->pos < lexer->length && lexer->input[lexer->pos] != '"') {
                        lexer->pos++;
                  }
                  if(lexer->pos >= lexer->length) {
                      printf("undetermined string");
                      exit(1);
                  }
                   size_t len = lexer->pos - start;
                   tok.type = TOK_STRING;
                   tok.value.str.data = (char *)&lexer->input[start];
                   tok.value.str.length = len;
                   lexer->pos++;
                   return tok;
    }

    if(c == '-' || isdigit(c)) {
      char *end;
      tok.value.value = strtod(&lexer->input[lexer->pos], &end);
      if(end == &lexer->input[lexer->pos]) {
        fprintf(stderr, "Invalid number\n");
        exit(1);
      }
      tok.type = TOK_NUMBER;
      lexer->pos = end - lexer->input;
      return tok;
    }

    if(strncmp(&lexer->input[lexer->pos], "true", 4) == 0) {
      lexer->pos += 4;
      tok.type = TOK_TRUE;
      return tok;
    }

    if(strncmp(&lexer->input[lexer->pos], "false", 5) == 0) {
      lexer->pos += 5;
      tok.type = TOK_FALSE;
      return tok;
    }

    if(strncmp(&lexer->input[lexer->pos], "null", 4) == 0) {
      lexer->pos += 4;
      tok.type = TOK_NULL;
      return tok;
    }
    
    fprintf(stderr, "Unexpected character: %c\n", c);
    exit(1);
  }

void parser_next(Parser *parser) {
  parser->current = next_token(&parser->lexer);
}

void parse_error(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}

jsonNode parse_value(Parser *parser);
jsonNode parse_array(Parser *parser);
jsonNode parse_object(Parser *parser);

jsonNode parse_value(Parser *parser) {
  Token tok = parser->current;

  switch(tok.type) {
    case TOK_STRING: parser_next(parser);
                     return create_str(tok.value.str.data, tok.value.str.length);
    case TOK_NUMBER: parser_next(parser);
                     return create_num(tok.value.value);
    case TOK_TRUE: parser_next(parser);
                   return create_literal(JsonTrue);
    case TOK_FALSE: parser_next(parser);
                    return create_literal(JsonFalse);
    case TOK_NULL: parser_next(parser);
                   return create_literal(JsonNull);
    case TOK_LBRACKET: return parse_array(parser);
    case TOK_LBRACE: return parse_object(parser);
    default: parse_error("Expected JSON value");
  }

  return create_literal(JsonNull);
}

jsonNode parse_array(Parser *parser) {
  jsonNode arr = create_arr(0);
  parser_next(parser);

  if(parser->current.type == TOK_RBRACKET) {
    parser_next(parser);
    return arr;
  }

  while(1) {
    jsonNode item = parse_value(parser);
    arr.value.jsonArr.data = realloc(arr.value.jsonArr.data, (arr.value.jsonArr.length + 1) * sizeof(jsonNode));
    arr.value.jsonArr.data[arr.value.jsonArr.length] = item;
    arr.value.jsonArr.length++;

    if(parser->current.type == TOK_COMMA) {
      parser_next(parser);
      continue;
    }

    if(parser->current.type == TOK_RBRACKET) {
      parser_next(parser);
      return arr;
    }

    parse_error("Expected comma or closing bracket in array");
  }
}

jsonNode parse_object(Parser *parser) {
  jsonNode obj = create_obj(0);
  parser_next(parser);

  if(parser->current.type == TOK_RBRACE) {
    parser_next(parser);
    return obj;
  }

  while(1) {
    if(parser->current.type != TOK_STRING) {
      parse_error("Expected string key in object");
    }

    jsonString key;
    key.length = parser->current.value.str.length;
    key.data = malloc(key.length);
    memcpy(key.data, parser->current.value.str.data, key.length);
    parser_next(parser);

    if(parser->current.type != TOK_COLON) {
      parse_error("Expected colon after object key");
    }

    parser_next(parser);
    jsonNode value = parse_value(parser);

    obj.value.jsonObj.data = realloc(obj.value.jsonObj.data, (obj.value.jsonObj.length + 1) * sizeof(jsonPair));
    obj.value.jsonObj.data[obj.value.jsonObj.length].key = key;
    obj.value.jsonObj.data[obj.value.jsonObj.length].value = value;
    obj.value.jsonObj.length++;

    if(parser->current.type == TOK_COMMA) {
      parser_next(parser);
      continue;
    }

    if(parser->current.type == TOK_RBRACE) {
      parser_next(parser);
      return obj;
    }

    parse_error("Expected comma or closing brace in object");
  }
}

jsonNode parse_json(const char *input) {
  Parser parser;
  parser.lexer.input = input;
  parser.lexer.pos = 0;
  parser.lexer.length = strlen(input);
  parser_next(&parser);

  jsonNode root = parse_value(&parser);
  if(parser.current.type != TOK_EOF) {
    json_free(&root);
    parse_error("Unexpected token after JSON value");
  }

  return root;
}

void print_indent(int indent) {
  for(int i=0; i<indent; i++) {
    putchar(' ');
  }
}

void print_json_string(jsonString str) {
  putchar('"');
  fwrite(str.data, 1, str.length, stdout);
  putchar('"');
}

void print_json(jsonNode *node, int indent) {
  switch(node->type) {
    case JsonNum: printf("%g", node->value.jsonNum);
                  break;
    case JsonStr: print_json_string(node->value.jsonStr);
                  break;
    case JsonNull: printf("null");
                   break;
    case JsonTrue: printf("true");
                   break;
    case JsonFalse: printf("false");
                    break;
    case JsonArr: printf("[");
                  for(size_t i=0; i<node->value.jsonArr.length; i++) {
                    if(i == 0) printf("\n");
                    print_indent(indent + 2);
                    print_json(&node->value.jsonArr.data[i], indent + 2);
                    if(i + 1 < node->value.jsonArr.length) printf(",");
                    printf("\n");
                  }
                  if(node->value.jsonArr.length > 0) print_indent(indent);
                  printf("]");
                  break;
    case JsonObj: printf("{");
                  for(size_t i=0; i<node->value.jsonObj.length; i++) {
                    if(i == 0) printf("\n");
                    print_indent(indent + 2);
                    print_json_string(node->value.jsonObj.data[i].key);
                    printf(": ");
                    print_json(&node->value.jsonObj.data[i].value, indent + 2);
                    if(i + 1 < node->value.jsonObj.length) printf(",");
                    printf("\n");
                  }
                  if(node->value.jsonObj.length > 0) print_indent(indent);
                  printf("}");
                  break;
  }
}

char *read_file(const char *path) {
  FILE *file = fopen(path, "rb");
  if(file == NULL) {
    perror(path);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  if(length < 0) {
    perror(path);
    fclose(file);
    exit(1);
  }
  rewind(file);

  char *data = malloc((size_t)length + 1);
  size_t read_len = fread(data, 1, (size_t)length, file);
  if(read_len != (size_t)length) {
    perror(path);
    fclose(file);
    free(data);
    exit(1);
  }

  data[length] = '\0';
  fclose(file);
  return data;
}

int main() {
  char *input = read_file("test.json");
  jsonNode root = parse_json(input);

  print_json(&root, 0);
  printf("\n");

  json_free(&root);
  free(input);
  return 0;
}
