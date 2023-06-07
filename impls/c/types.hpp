#pragma once

typedef long long integer_t;

typedef enum types_enum_t {
  VAL_INTEGER,
  VAL_STRING,
} types_enum_t;

typedef struct data_type_t data_type_t;
struct data_type_t {
  types_enum_t type;
  union {
    integer_t number;
    const char *string;
  };
};

typedef struct data_type_node_t data_type_node_t;
struct data_type_node_t {
  data_type_t value;
  data_type_node_t *next;
}; 
