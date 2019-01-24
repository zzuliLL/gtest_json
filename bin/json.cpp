#include "json.h"

using namespace std;

void json_key_value(JSON *json, const char *key, value *val)
{
    assert(json != NULL && key != NULL && val != NULL);
    assert(json->type == JSON_Object);

	key_value *cur = new key_value();
    int len = strlen(key);
    cur->key = (char*)malloc(sizeof(char)*(len+1));
    if(cur->key == NULL)
    {
        fputs("malloc key error!\n", stderr);
        return ;
    }
    strncpy(cur->key, key, len+1);
    if(len >= 0) cur->key[len] = '\0';

    cur->val = val;
    json->type_value.obj->obj.push_back(cur);
}

void json_array_add(JSON *json, value *val)
{
    assert(json != NULL && val != NULL);
    assert(json->type == JSON_Array);
    json->type_value.ary->ary.push_back(val);
}

JSON* json_new_int(int value_Num)
{
    JSON *json = new value();
    json->type_value.num = value_Num;
    json->type = JSON_Number;
    return json;
}

JSON* json_new_bool(bool value_Bool)
{
    JSON *json = new value();
    json->type_value.bol = value_Bool;
    json->type = JSON_Bool;
    return json;
}

JSON* json_new_str(const char *value_Str)
{
    assert(value_Str != NULL);
    JSON *json = new value();

    json->type = JSON_String;
    int len = strlen(value_Str);
    json->type_value.str = (char*)malloc(sizeof(char)*(len+1));
    if(json->type_value.str == NULL)
    {
        fputs("malloc key error!\n", stderr);
        return NULL;
    }
    strncpy(json->type_value.str, value_Str, len+1);
    if(len >= 0)
        json->type_value.str[len] = '\0';
    return json;
}

JSON* json_new_array()
{
	JSON *json = NULL;
	json = new JSON();
	json->type = JSON_Array;
	json->type_value.ary = new Array();
	return json;
}

JSON* json_new_object()
{
	JSON *json = NULL;
	json = new JSON();
	json->type = JSON_Object;
	json->type_value.obj = new Object();
	//json->type_value.obj->obj.clear();
	return json;
}

void free_json(JSON* json)
{
    assert(json != NULL);
	int type = json->type;

    if(type == JSON_String)
    {
        free(json->type_value.str);
    }
    if(type == JSON_Array)
    {
        int cnt = json->type_value.ary->ary.size();
        for(int i = 0; i < cnt; i++)
        {
            free_json(json->type_value.ary->ary[i]);
        }
        delete json->type_value.ary;
    }
    if(type == JSON_Object)
    {
        int cnt = json->type_value.obj->obj.size();
        for(int i = 0; i < cnt; i++)
        {
            free(json->type_value.obj->obj[i]->key);
            free_json(json->type_value.obj->obj[i]->val);
        }
        delete json->type_value.obj;
    }
    free(json);
}

const JSON* json_get_value(const JSON *json, const char *key)
{
	assert(json != NULL && key != NULL);
	assert(strlen(key) > 0);

	int cnt = json->type_value.obj->obj.size();

	for(int i = 0; i < cnt; i++)
    {
        char *json_key = json->type_value.obj->obj[i]->key;
        if(strcmp(key, json_key) == 0)
        {
            return json->type_value.obj->obj[i]->val;
        }
    }
	return NULL;
}

/*
*Array index
*/
const JSON *json_get_index(const JSON *json, U32 idx)
{
	assert(json != NULL && idx >= 0);
	if(json->type != JSON_Array)
        return NULL;
	if(idx >= json->type_value.ary->ary.size()) return NULL;
	return json->type_value.ary->ary[idx];
}

int json_get_int(const JSON *json, const char *key, int def)
{
	assert(json != NULL && key != NULL);

	const JSON *cur;
	cur = json_get_value(json, key);
	if(cur == NULL || cur->type != JSON_Number)
	{
		fputs("json_get_value() error\n", stderr);
		return def;
	}
	return cur->type_value.num;
}


BOOL json_get_bool(const JSON *json, const char *key)
{
	assert(json != NULL && key != NULL);
	const JSON *cur;
	cur = json_get_value(json, key);
	if(cur == NULL || cur->type != JSON_Bool)
	{
		fputs("json_get_value() error\n", stderr);
		return false;
	}
	return cur->type_value.bol;
}


const char *json_get_str(const JSON *json, const char *key, const char *def)
{
	assert(json != NULL && key != NULL);
	const JSON *cur;
	cur = json_get_value(json, key);
	if(cur == NULL || cur->type != JSON_String)
	{
		fputs("json_get_value() error\n", stderr);
		return def;
	}
	return cur->type_value.str;
}

BOOL json_set_int(JSON *json, const char *key, int val)
{
	assert(json != NULL && key != NULL);
	assert(json->type == JSON_Object);

	JSON *cur;
	cur = (JSON*)json_get_value(json, key);
	if(cur == NULL)
    {
        JSON *val_int = json_new_int(val);
        json_key_value(json, key, val_int);
        return true;
    }
	if(cur->type != JSON_Number)
	{
		fputs("json_get_value() error\n", stderr);
		return false;
	}
	cur->type_value.num = val;
	return true;
}

BOOL json_set_bool(JSON *json, const char *key, BOOL val)
{
	assert(json != NULL && key != NULL);
	assert(json->type == JSON_Object);
	JSON *cur;
	cur = (JSON*)json_get_value(json, key);
	if(cur == NULL)
    {
        JSON *val_bol = json_new_bool(val);
        json_key_value(json, key, val_bol);
        return true;
    }
	if(cur->type != JSON_Bool)
	{
		fputs("json_get_value() error\n", stderr);
		return false;
	}

	cur->type_value.bol = val;
	return true;
}

BOOL json_set_str(JSON *json, const char *key, const char *val)
{
	assert(json != NULL && key != NULL && val != NULL);
	assert(json->type == JSON_Object);
	assert(strlen(val) > 0);

	JSON *cur;
	cur = (JSON*)json_get_value(json, key);
	if(cur == NULL)
    {
        JSON *val_str = json_new_str(val);
        json_key_value(json, key, val_str);
        return true;
    }
	if(cur->type != JSON_String)
	{
		fputs("json_get_value() error\n", stderr);
		return false;
	}
	free(cur->type_value.str);
	int len = strlen(val);
	cur->type_value.str = (char*)malloc(sizeof(char)*(len+1));
	if(cur->type_value.str == NULL)
	{
		fputs("malloc() error\n", stderr);
		return false;
	}
	strncpy(cur->type_value.str, val, len+1);
	if(len >= 0)cur->type_value.str[len] = '\0';
	return true;
}



