idf_component_register(
        SRCS "ds3231.c"
        INCLUDE_DIRS "include"
        REQUIRES "aespl_common" "aespl_i2c"
)
