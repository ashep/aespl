idf_component_register(
        SRCS "ds3231.c"
        INCLUDE_DIRS "include"
        REQUIRES "aespl_util" "aespl_i2c"
)
