#include "json.h"
#include "gtest/gtest.h"
using namespace std;

#define BUF_SIZE 1024

int json_save(const JSON *json, const char *fname);
int read_file(string *str, const char *fname);
void print_json(const JSON* json, FILE *fp, int seek_cnt);
void print_seek(FILE *fp, int seek_cnt);

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

int read_file(string *str, const char *fname)
{
    FILE *fp;
    long len;
    long realsize;
    char *buf;

    fp = fopen(fname, "r");
    if (!fp) {
        fprintf(stderr, "open file [%s] failed, errno: %d\n"
            , fname, errno);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    if (len <= 0) {
        fclose(fp);
        fprintf(stderr, "ftell failed, errno: %d\n", errno);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    buf = (char *)malloc(len + 1);
    if (!buf) {
        fclose(fp);
        fprintf(stderr, "malloc(%ld) failed\n", len + 1);
        return -1;
    }
    realsize = fread(buf, 1, len, fp);
    fclose(fp);

    buf[realsize] = '\0';
    *str = buf;
    free(buf);
    return 0;
}

void print_seek(FILE *fp, int seek_cnt)
{
    for(int i = 0; i < seek_cnt; i++)
        fputs(" ", fp);
}

void print_json(const JSON* json, FILE *fp, int seek_cnt)
{
	assert(json != NULL);
	int type = json->type;
	char buf[BUF_SIZE];

	print_seek(fp, seek_cnt);
    if(type == JSON_Bool)
    {
        sprintf(buf, "%s\n", json->type_value.bol ? "true" : "false");
        fputs(buf, fp);
    }
    else if(type == JSON_Number)
    {
        sprintf(buf, "%d\n", json->type_value.num);
        fputs(buf, fp);
    }
    else if(type == JSON_String)
    {
        sprintf(buf, "%s\n", json->type_value.str);
        fputs(buf, fp);
    }
    if(type == JSON_Array)
    {
        int cnt = json->type_value.ary->ary.size();
        for(int i = 0; i < cnt; i++)
        {
            fputs("  - ", fp);
            print_json(json->type_value.ary->ary[i], fp, 0);
            if(i != cnt - 1)
                print_seek(fp, seek_cnt);
        }
    }
    if(type == JSON_Object)
    {
        int cnt = json->type_value.obj->obj.size();
        for(int i = 0; i < cnt; i++)
        {
            sprintf(buf, "  %s:", json->type_value.obj->obj[i]->key);
            fputs(buf, fp);
            if((json->type_value.obj->obj[i]->val) && (json->type_value.obj->obj[i]->val->type == JSON_Array ||
               json->type_value.obj->obj[i]->val->type == JSON_Object))
            {
                  fputs("\n", fp);
                  print_json(json->type_value.obj->obj[i]->val, fp, seek_cnt+2);
            }
            else
            {
                fputs(" ", fp);
                print_json(json->type_value.obj->obj[i]->val, fp, 0);
            }
            if(i != cnt - 1)
                print_seek(fp, seek_cnt);

        }
    }
}


int json_save(const JSON *json, const char *fname)
{
    assert(json != NULL && fname != NULL);
    assert(strlen(fname) > 0);

    FILE *fp = fopen(fname, "w");
    if (!fp)
    {
        fprintf(stderr, "open file [%s] failed, errno: %d\n"
            , fname, errno);
        return -1;
    }
    print_json(json, fp, 0);
    fclose(fp);
    return 0;
}

// const JSON* json_get_value(const JSON *json, const char *key);
TEST(json_get_value, key1)
{
    JSON *str;
    string result;
    const char *expect = "hello json\n";

    JSON *obj = json_new_object();
    str = json_new_str("hello json");
    json_key_value(obj, "message", str);
    const JSON* str1 = json_get_value(obj, "message");

    EXPECT_EQ(0, json_save(str1, "./file/json_get_value.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_get_value.yml"));

    ASSERT_EQ(result, expect);
    free_json(obj);
}


TEST(json_get_value, key2)
{
    JSON *str;
    string result;
    const char *expect = "";

    JSON *obj = json_new_object();
    str = json_new_str("hello json");
    json_key_value(obj, "message", str);
    const JSON* str1 = json_get_value(obj, "skajhf");
    if(str1 == NULL) result = "";
    ASSERT_EQ(result, expect);
    free_json(obj);
}

TEST(json_get_value, key3)
{
    string result;
    const char *expect = "  - sll\n  - false\n  - 1\n  - yanfayiban\n";

    JSON *json = json_new_object();

    JSON *val_str = json_new_str("sll");
    JSON *val_bool = json_new_bool(false);
    JSON *val_int1 = json_new_int(1);
    JSON *val_str2 = json_new_str("yanfayiban");

    JSON *val_ary = json_new_array();
    json_array_add(val_ary, val_str);
    json_array_add(val_ary, val_bool);
    json_array_add(val_ary, val_int1);
    json_array_add(val_ary, val_str2);

    json_key_value(json, "message", val_ary);

    const JSON* str1 = json_get_value(json, "message");

    EXPECT_EQ(0, json_save(str1, "./file/json_get_value.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_get_value.yml"));

    ASSERT_EQ(result, expect);
    free_json(json);
}

//const JSON *json_get_index(const JSON *json, U32 idx);

TEST(json_get_index, idx1)
{
    string result;
    const char *expect = "hello json\n";

    JSON *ary = json_new_array();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_array_add(ary, val_str);
    json_array_add(ary, val_int);
    json_array_add(ary, val_bol);

    const JSON *json = json_get_index(ary, 0);

    EXPECT_EQ(0, json_save(json, "./file/json_get_idx.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_get_idx.yml"));

    ASSERT_EQ(result, expect);
    free_json(ary);
}

TEST(json_get_index, idx2)
{
    string result;
    const char *expect = "1009\n";

    JSON *ary = json_new_array();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_array_add(ary, val_str);
    json_array_add(ary, val_int);
    json_array_add(ary, val_bol);

    const JSON *json = json_get_index(ary, 1);

    EXPECT_EQ(0, json_save(json, "./file/json_get_idx.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_get_idx.yml"));

    ASSERT_EQ(result, expect);
    free_json(ary);
}


TEST(json_get_index, idx3)
{
    string result = "slhf";
    const char *expect = "";

    JSON *ary = json_new_array();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_array_add(ary, val_str);
    json_array_add(ary, val_int);
    json_array_add(ary, val_bol);

    const JSON *json = json_get_index(val_str, 2);

    if(json == NULL) result = "";

    ASSERT_EQ(result, expect);
    free_json(ary);
}

TEST(json_get_index, idx4)
{
    string result = "slf";
    const char *expect = "";

    JSON *ary = json_new_array();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_array_add(ary, val_str);
    json_array_add(ary, val_int);
    json_array_add(ary, val_bol);

    const JSON *json = json_get_index(ary, 5);

    if(json == NULL) result = "";
    ASSERT_EQ(result, expect);
    free_json(ary);
}

//int json_get_int(const JSON *json, const char *key, int def);

TEST(json_get_int, key1)
{
    int result;
    int expect = 1009;

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_int(obj, "chinese", 0);
    ASSERT_EQ(result, expect);
    free_json(obj);
}

TEST(json_get_int, key2)
{
    int result = 879879;
    int expect = 0;

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_int(obj, "enable", 0);
    ASSERT_EQ(result, expect);
    free_json(obj);
}


TEST(json_get_int, key3)
{
    int result = 887;
    int expect = 0;

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_int(obj, "easkfhkafh", 0);
    ASSERT_EQ(result, expect);
    free_json(obj);
}

//BOOL json_get_bool(const JSON *json, const char *key);

TEST(json_get_bool, key1)
{
    bool result;
    bool expect = true;

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_bool(obj, "enable");

    ASSERT_EQ(result, expect);
    free_json(obj);
}

TEST(json_get_bool, key2)
{
    bool result = true;
    bool expect = false;

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_bool(obj, "message");

    ASSERT_EQ(result, expect);
    free_json(obj);
}


TEST(json_get_bool, key3)
{
    bool result = true;
    bool expect = false;

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_bool(obj, "messages");

    ASSERT_EQ(result, expect);
    free_json(obj);
}

//const char *json_get_str(const JSON *json, const char *key, const char *def);
TEST(json_get_str, key1)
{
    string result;
    const char *expect = "hello json";

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_str(obj, "message", "");

    ASSERT_EQ(result, expect);
    free_json(obj);
}

TEST(json_get_str, key2)
{
    string result;
    const char *expect = "";

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_str(obj, "messages", "");

    ASSERT_EQ(result, expect);
    free_json(obj);
}

TEST(json_get_str, key3)
{
    string result;
    const char *expect = "";

    JSON *obj = json_new_object();
    JSON *val_str = json_new_str("hello json");
    JSON *val_int = json_new_int(1009);
    JSON *val_bol = json_new_bool(true);

    json_key_value(obj, "message", val_str);
    json_key_value(obj, "chinese", val_int);
    json_key_value(obj, "enable", val_bol);


    result = json_get_str(obj, "enable", "");

    ASSERT_EQ(result, expect);
    free_json(obj);
}

//BOOL json_set_int(JSON *json, const char *key, int val);
TEST(json_set_int, obj1)
{
    JSON *json;
    string result;
    const char *expect = "  key: 12\n  name: 1009\n";

    json = json_new_object();
    json_set_int(json, "key", 12);
    json_set_int(json, "name", 13);
    json_set_int(json, "name", 1009);

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}


TEST(json_set_int, obj2)
{
    JSON *json;
    string result;
    const char *expect = "  key: sahf\n";

    json = json_new_object();
    json_set_str(json, "key", "sahf");
    json_set_int(json, "key", 12);

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}

//BOOL json_set_bool(JSON *json, const char *key, BOOL val);
TEST(json_set_bool, obj1)
{
    JSON *json;
    string result;
    const char *expect = "  key: false\n";

    json = json_new_object();
    json_set_bool(json, "key", true);
    json_set_bool(json, "key", false);

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}

TEST(json_set_bool, obj2)
{
    JSON *json;
    string result;
    const char *expect = "  key: sfsfsf\n";

    json = json_new_object();
    json_set_str(json, "key", "sfsfsf");
    json_set_bool(json, "key", false);

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}

//BOOL json_set_str(JSON *json, const char *key, const char *val);
TEST(json_set_str, obj1)
{
    JSON *json;
    string result;
    const char *expect = "  key: hello\n";

    json = json_new_object();
    json_set_str(json, "key", "helsdfdsflo");
    json_set_str(json, "key", "hello");

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}

TEST(json_set_str, obj2)
{
    JSON *json;
    string result;
    const char *expect = "  key: false\n";

    json = json_new_object();
    json_set_bool(json, "key", false);
    json_set_str(json, "key", "hello");

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}

TEST(json_test_all, obj)
{
    string result;
    const char *expect = "  id: 12\n  name: sll\n  status:\
 false\n  message:\n    - hahahahaha.....\n    - 1\n  score:\n    chinese:\
 100\n    english: 99\n    enable: false\n    class: yanfayiban\n";

    JSON *json = json_new_object();
    JSON *val_str = json_new_str("sll");
    JSON *val_int = json_new_int(12);
    JSON *val_bool = json_new_bool(false);
    JSON *val_str1 = json_new_str("hahahahaha.....");
    JSON *val_int1 = json_new_int(1);
    JSON *val_int2 = json_new_int(100);
    JSON *val_int3 = json_new_int(99);
    JSON *val_bool2 = json_new_bool(false);
    JSON *val_str2 = json_new_str("yanfayiban");

    JSON *val_ary = json_new_array();
    json_array_add(val_ary, val_str1);
    json_array_add(val_ary, val_int1);

    JSON *val_obj1 = json_new_object();
    json_key_value(val_obj1, "chinese", val_int2);
    json_key_value(val_obj1, "english", val_int3);
    json_key_value(val_obj1, "enable", val_bool2);
    json_key_value(val_obj1, "class", val_str2);

    json_key_value(json, "id", val_int);
    json_key_value(json, "name", val_str);
    json_key_value(json, "status", val_bool);
    json_key_value(json, "message", val_ary);
    json_key_value(json, "score", val_obj1);

    EXPECT_EQ(0, json_save(json, "./file/json_set_obj.yml"));
    EXPECT_EQ(0, read_file(&result, "./file/json_set_obj.yml"));
    ASSERT_EQ(result, expect);
    free_json(json);
}


