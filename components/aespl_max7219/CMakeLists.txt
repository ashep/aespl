idf_component_register(
        SRCS "max7219.c" "matrix.c"
        INCLUDE_DIRS "include"
        REQUIRES "aespl_common" "aespl_gfx"
)
