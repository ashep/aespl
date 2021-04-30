idf_component_register(
        SRCS "client.c"
        INCLUDE_DIRS "include"
        REQUIRES "json" "http_parser" "esp_http_client"
)
