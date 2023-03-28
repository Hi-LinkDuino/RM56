#include "cmsis_os2.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "lvgl.h"
LV_IMG_DECLARE(img_logo);//图片资源
void lv_demo_logo(void){
    lv_disp_t * disp = lv_disp_get_default();

    lv_style_t style1;
    lv_style_init(&style1);
    lv_style_set_bg_color(&style1, lv_color_hex(0xff0000));
    lv_style_set_border_width(&style1, 0);
    lv_style_set_radius(&style1, 0);

    lv_obj_t *scr = lv_scr_act();
    // lv_obj_t *bkg = lv_obj_create(scr);
    // lv_obj_set_pos(bkg, 0, 0);
    // lv_obj_set_size(bkg, 480, 480);
    // lv_obj_add_style(bkg, &style1, 0);

    lv_obj_t *img = lv_img_create(scr);
    lv_img_set_src(img, &img_logo);
    lv_obj_set_pos(img, 100, 100);

    // lv_obj_invalidate(scr);
    // lv_task_handler();

    lv_refr_now(disp);

    lv_obj_del(img);
}
