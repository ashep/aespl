idf_component_register(
        SRCS "server.c"
        INCLUDE_DIRS "include"
        REQUIRES "json" "esp_http_server" "aespl_common"
)
