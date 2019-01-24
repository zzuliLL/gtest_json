#include<bits/stdc++.h>

#define JSON_Bool 1
#define JSON_NULL 2
#define JSON_Number 3
#define JSON_String 4
#define JSON_Array 5
#define JSON_Object 6

#define BOOL bool
#define U32 unsigned int
#define KEY_MAX 1024
#define VALUE_MAX 1024

struct Object;
struct Array;
struct key_value;
struct value;

typedef struct value JSON;

struct value
{
    int type;
    union type_value
    {
        int num;
        bool bol;
        char *str;
        Array *ary;
        Object *obj;
    }type_value;
};

struct key_value
{
    char *key;
    value *val;
};

struct Array
{
    std::vector<value*> ary;
};

struct Object
{
    std::vector<key_value*> obj;
};

void json_key_value(JSON *json, const char *key, value *val);
void json_array_add(JSON *json, value *val);
JSON* json_new_int(int value_Num);
JSON* json_new_bool(bool value_Bool);
JSON* json_new_str(const char *value_Str);
JSON* json_new_array();
JSON* json_new_object();
void free_json(JSON* json);


const JSON* json_get_value(const JSON *json, const char *key);
const JSON *json_get_index(const JSON *json, U32 idx);
int json_get_int(const JSON *json, const char *key, int def);
BOOL json_get_bool(const JSON *json, const char *key);
const char *json_get_str(const JSON *json, const char *key, const char *def);
BOOL json_set_int(JSON *json, const char *key, int val);
BOOL json_set_bool(JSON *json, const char *key, BOOL val);
BOOL json_set_str(JSON *json, const char *key, const char *val);


