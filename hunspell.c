/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header,v 1.16.2.1.2.1 2007/01/01 19:32:09 iliaa Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include <hunspell/hunspell.h>
#include "php_hunspell.h"

/* True global resources - no need for thread safety here */
//static int le_hunspell;
//zend_object_handlers hunspell_object_handlers;


static ZEND_NAMED_FUNCTION(hunspell_open_dic)
{
//  Hunspell_create
}

PHP_METHOD(hunspell, __construct)
{
  ze_hunspell_object *ze_obj = NULL;
  char *aff_path, *dic_path;
  size_t aff_path_len, dic_path_len;
  Hunhandle *dic;

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &dic_path, &dic_path_len, &aff_path, &aff_path_len) == FAILURE)
  {
    return;
  }

  dic = Hunspell_create((const char *)aff_path, (const char *)dic_path);
  if (dic == NULL)
  {
    efree(aff_path);
    efree(dic_path);

    return;
  }

  ze_obj = Z_HUNSPELL_OBJ_P(getThis());
  ze_obj->aff_path_len = aff_path_len;
  ze_obj->dic_path_len = dic_path_len;

  ze_obj->aff_path = aff_path;
  ze_obj->dic_path = dic_path;
  ze_obj->dic = dic;

} /* }}} */

PHP_METHOD(hunspell, open)
{
  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;

  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }
  if (ze_obj->dic != NULL)
  {
    Hunspell_destroy(ze_obj->dic);
  }
  
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &ze_obj->dic_path, &ze_obj->dic_path_len, &ze_obj->aff_path, &ze_obj->aff_path_len) == FAILURE)
  {
    RETURN_FALSE;
  }

  ze_obj->dic = Hunspell_create((const char *)ze_obj->aff_path, (const char *)ze_obj->dic_path);
  if (ze_obj->dic == NULL)
    RETURN_FALSE;

  RETURN_TRUE;
}

PHP_METHOD(hunspell, get_encoding)
{
  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;
  char *retval;

  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }
  if (ze_obj->dic == NULL)
  {
    RETURN_FALSE;
  }
  retval = Hunspell_get_dic_encoding(ze_obj->dic);
  RETURN_STRING(retval);
}

PHP_METHOD(hunspell, close)
{

  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;

  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }

  if (ze_obj->dic == NULL)
  {
    RETURN_FALSE;
  }
  // Cannot destroy hunspell object. may be it was destroyed early?
  if ( ze_obj != NULL ) 
    Hunspell_destroy(ze_obj->dic);

  ze_obj->dic = NULL;
  RETURN_TRUE;
}

PHP_METHOD(hunspell, spell)
{
  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;
  char *word;
  int word_len;

  int retval;
  
  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }

  if (ze_obj->dic == NULL)
  {
    RETURN_FALSE;
  }

  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &word, &word_len) == FAILURE)
  {
    RETURN_FALSE;
  }
       
  retval = Hunspell_spell(ze_obj->dic, (const char *) word);
//  efree(word);
  
  RETURN_LONG(retval);
}

PHP_METHOD(hunspell, suggest)
{
  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;
  char *word;
  int word_len, i;

  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }
  else
    RETURN_FALSE;
    
  if (ze_obj->dic == NULL)
  {
    RETURN_FALSE;
  }
  
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &word, &word_len) == FAILURE)
  {
    RETURN_FALSE;
  }

//RETURN_FALSE;
  int spell_count = Hunspell_spell(ze_obj->dic, word);
  
  if (spell_count != 0)
  {
    RETURN_FALSE;
  }
  else
  {
    char **slist;
    int sl_count = Hunspell_suggest(ze_obj->dic, &slist, (const char *)word);

    if (sl_count == 0)
    {
      RETURN_FALSE;
    }
    else
    {
      // if (array_init(return_value) == FAILURE) php_error_docref(NULL, E_ERROR, "Unable to initialize array");
      array_init(return_value);
      for (i = 0; i < sl_count; ++i)
      {
        add_next_index_string(return_value, slist[i]);
      }
    }
    Hunspell_free_list(ze_obj->dic, &slist, sl_count);
  }
}


PHP_METHOD(hunspell, analyze)
{
  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;
  char *word;
  int word_len, i;

  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }
  else
    RETURN_FALSE;
    
  if (ze_obj->dic == NULL)
  {
    RETURN_FALSE;
  }
  
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &word, &word_len) == FAILURE)
  {
    RETURN_FALSE;
  }

//RETURN_FALSE;
  int spell_count = Hunspell_spell(ze_obj->dic, word);
  
  if (spell_count == 0)
  {
    RETURN_FALSE;
  }
  else
  {
    char **slist;
    int sl_count = Hunspell_analyze(ze_obj->dic, &slist, (const char *)word);

    if (sl_count == 0)
    {
      RETURN_FALSE;
    }
    else
    {
      // if (array_init(return_value) == FAILURE) php_error_docref(NULL, E_ERROR, "Unable to initialize array");
      array_init(return_value);
      for (i = 0; i < sl_count; ++i)
      {
        add_next_index_string(return_value, slist[i]);
      }
    }
    Hunspell_free_list(ze_obj->dic, &slist, sl_count);
  }
}


PHP_METHOD(hunspell, stem)
{
  zval *this = getThis();
  ze_hunspell_object *ze_obj = NULL;
  char *word;
  int word_len, i;

  if (this)
  {
    ze_obj = Z_HUNSPELL_OBJ_P(this);
  }
  else
    RETURN_FALSE;
    
  if (ze_obj->dic == NULL)
  {
    RETURN_FALSE;
  }
  
  if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &word, &word_len) == FAILURE)
  {
    RETURN_FALSE;
  }

//RETURN_FALSE;
  int spell_count = Hunspell_spell(ze_obj->dic, word);
  
  if (spell_count == 0)
  {
    RETURN_FALSE;
  }
  else
  {
    char **slist;
    int sl_count = Hunspell_stem(ze_obj->dic, &slist, (const char *)word);

    if (sl_count == 0)
    {
      RETURN_FALSE;
    }
    else
    {
      // if (array_init(return_value) == FAILURE) php_error_docref(NULL, E_ERROR, "Unable to initialize array");
      array_init(return_value);
      for (i = 0; i < sl_count; ++i)
      {
        add_next_index_string(return_value, slist[i]);
      }
    }
    Hunspell_free_list(ze_obj->dic, &slist, sl_count);
  }
}




ZEND_BEGIN_ARG_INFO(arginfo_hunspell___construct, 0)
    ZEND_ARG_INFO(0, dic_path) /* parameter name */
    ZEND_ARG_INFO(0, aff_path) /* parameter name */
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_hunspell_open, 0)
    ZEND_ARG_INFO(0, dic_path) /* parameter name */
    ZEND_ARG_INFO(0, aff_path) /* parameter name */
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_hunspell_spell, 0)
    ZEND_ARG_INFO(0, word) /* parameter name */
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_hunspell_suggest, 0)
    ZEND_ARG_INFO(0, word) /* parameter name */
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_hunspell_analyze, 0)
    ZEND_ARG_INFO(0, word) /* parameter name */
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO(arginfo_hunspell_stem, 0)
    ZEND_ARG_INFO(0, word) /* parameter name */
ZEND_END_ARG_INFO();

/* {{{ hunspell_functions[]
 */
static zend_function_entry hunspell_class_functions[] =
{
    PHP_ME(hunspell,	__construct,	arginfo_hunspell___construct,	ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	open,		arginfo_hunspell_open,		ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	get_encoding,	NULL,				ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	spell,		arginfo_hunspell_spell,		ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	suggest,	arginfo_hunspell_suggest,	ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	analyze,	arginfo_hunspell_analyze,	ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	stem,		arginfo_hunspell_stem,		ZEND_ACC_PUBLIC)
    PHP_ME(hunspell,	close,		NULL,				ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

/* {{{ hunspell_module_entry
 */
zend_module_entry hunspell_module_entry = {
	STANDARD_MODULE_HEADER,
	"hunspell",
	NULL,
	PHP_MINIT(hunspell),
	PHP_MSHUTDOWN(hunspell),
	NULL,
	NULL,
	PHP_MINFO(hunspell),
	"0.2", /* Replace with version number for your extension */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HUNSPELL
ZEND_GET_MODULE(hunspell)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini 
PHP_INI_BEGIN()
/*    STD_PHP_INI_ENTRY("hunspell.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_hunspell_globals, hunspell_globals)
    STD_PHP_INI_ENTRY("hunspell.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_hunspell_globals, hunspell_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ ZE2 OO definitions */
static zend_object_handlers hunspell_handlers;
static zend_class_entry *hunspell_class_entry;
/* }}} */

static void php_hunspell_object_free_storage(void *object ) /* {{{ */
{
  ze_hunspell_object *intern = php_ze_hunspell_object_fetch_object(object);
  if (!intern)
    return;
// Memory was given not the emalloc or malloc, so we don't need to free it!!!
/* 
  if (intern->aff_path != NULL)
    efree(intern->aff_path);
    return ;
  if (intern->dic_path != NULL)
    efree(intern->dic_path);
*/
  if (intern->dic != NULL)
    Hunspell_destroy(intern->dic);
    intern->dic = NULL;

  zend_object_std_dtor(&intern->zo);
}

static zend_object * php_ze_hunspell_object_new(zend_class_entry *ce ) /* {{{ */
{
    ze_hunspell_object *intern = ecalloc(1,
        sizeof(ze_hunspell_object) +
        zend_object_properties_size(ce));

    intern->dic = NULL;
    intern->aff_path = NULL;
    intern->dic_path = NULL;
    intern->aff_path_len = 0;
    intern->dic_path_len = 0;

    zend_object_std_init(&intern->zo, ce);
    object_properties_init(&intern->zo, ce);

    intern->zo.handlers = &hunspell_handlers;

    return &intern->zo;
}

static inline ze_hunspell_object * php_ze_hunspell_object_fetch_object(zend_object *obj) {
    return (ze_hunspell_object *)((char *)obj - XtOffsetOf(ze_hunspell_object, zo));
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(hunspell)
{
  zend_class_entry ce;
  zend_class_entry *hunspell_ce;
  
  INIT_CLASS_ENTRY(ce, "hunspell", hunspell_class_functions);
  hunspell_ce = zend_register_internal_class(&ce);
  hunspell_ce->create_object = php_ze_hunspell_object_new;
  
  memcpy(&hunspell_handlers, zend_get_std_object_handlers(),
         sizeof(zend_object_handlers));
  
  hunspell_handlers.offset = XtOffsetOf(ze_hunspell_object, zo);
  hunspell_handlers.free_obj = (zend_object_free_obj_t)php_hunspell_object_free_storage;
  hunspell_handlers.clone_obj = NULL;

  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(hunspell)
{
  return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(hunspell)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "hunspell support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini */
	//DISPLAY_INI_ENTRIES();
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
