SET(SOURCES screens/ui_Screen_main.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_indicator_ver_png.c)

add_library(ui ${SOURCES})
