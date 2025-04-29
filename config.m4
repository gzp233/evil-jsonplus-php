PHP_ARG_ENABLE(jsonplus, whether to enable jsonplus extension, [  --enable-jsonplus   Enable jsonplus extension])

if test "$PHP_JSONPLUS" = "yes"; then
  PHP_NEW_EXTENSION(jsonplus, jsonplus.c, $ext_shared)
fi
