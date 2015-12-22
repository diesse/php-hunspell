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

#ifndef PHP_HUNSPELL_H
#define PHP_HUNSPELL_H

extern zend_module_entry hunspell_module_entry;
#define phpext_hunspell_ptr &hunspell_module_entry

#ifdef PHP_WIN32
#define PHP_HUNSPELL_API __declspec(dllexport)
#else
#define PHP_HUNSPELL_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(hunspell);
PHP_MSHUTDOWN_FUNCTION(hunspell);
PHP_RINIT_FUNCTION(hunspell);
PHP_RSHUTDOWN_FUNCTION(hunspell);
PHP_MINFO_FUNCTION(hunspell);

PHP_FUNCTION(confirm_hunspell_compiled);	/* For testing, remove later. */

typedef struct _ze_hunspell_object {
    Hunhandle *dic;
    char *aff_path;
    int aff_path_len;
    char *dic_path;
    int dic_path_len;
    zend_object zo;
} ze_hunspell_object;

static inline ze_hunspell_object * php_hunspell_object_fetch_object(zend_object *obj);
#define Z_HUNSPELL_OBJ_P(zv) php_hunspell_object_fetch_object(Z_OBJ_P(zv));

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(hunspell)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(hunspell)
*/

/* In every utility function you add that needs to use variables 
   in php_hunspell_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as HUNSPELL_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define HUNSPELL_G(v) TSRMG(hunspell_globals_id, zend_hunspell_globals *, v)
#else
#define HUNSPELL_G(v) (hunspell_globals.v)
#endif

#endif	/* PHP_HUNSPELL_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
