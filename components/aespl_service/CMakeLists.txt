idf_component_register(
        SRCS "service.c"
        INCLUDE_DIRS "include"
        REQUIRES "json" "aespl_http_util" "aespl_httpd"
)
