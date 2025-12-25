/// 字体配置测试代码 - 在初始化时调用以验证配置
///
/// 使用方法：
/// 1. 在你的初始化代码中调用 test_font_roboto_21()
/// 2. 检查调试输出
/// 3. 如果输出异常，说明字库配置有问题

#include "lv_font_roboto_21.h"
#include "cmsis_os_api.h"

void test_font_roboto_21(void)
{
    PRINT_LOG("\n========== lv_font_roboto_21 配置测试 ==========\n");
    
    /// 1. 检查字体结构
    PRINT_LOG("字体地址: %p\n", (void*)&lv_font_roboto_21);
    PRINT_LOG("line_height: %d\n", lv_font_roboto_21.line_height);
    PRINT_LOG("base_line: %d\n", lv_font_roboto_21.base_line);
    
    /// 2. 检查font_dsc
    const lv_font_fmt_txt_dsc_t *dsc = (const lv_font_fmt_txt_dsc_t *)lv_font_roboto_21.dsc;
    if (dsc) {
        PRINT_LOG("font_dsc address: %p\n", (void*)dsc);
        PRINT_LOG("  bpp: %d\n", dsc->bpp);
        PRINT_LOG("  bitmap_format: %d\n", dsc->bitmap_format);
        PRINT_LOG("  cmap_num: %d\n", dsc->cmap_num);
        PRINT_LOG("  glyph_bitmap: %p\n", (void*)dsc->glyph_bitmap);
        PRINT_LOG("  glyph_dsc: %p\n", (void*)dsc->glyph_dsc);
    }
    
    /// 3. 测试字符查找
    PRINT_LOG("\n测试字符 'L' (Unicode 76):\n");
    lv_font_glyph_dsc_t g_dsc;
    bool found = lv_font_get_glyph_dsc_fmt_txt(&lv_font_roboto_21, &g_dsc, 'L', '\0');
    
    if (found) {
        PRINT_LOG("  找到字符!\n");
        PRINT_LOG("  adv_w: %d\n", g_dsc.adv_w);
        PRINT_LOG("  box_w: %d\n", g_dsc.box_w);
        PRINT_LOG("  box_h: %d\n", g_dsc.box_h);
        PRINT_LOG("  ofs_x: %d\n", g_dsc.ofs_x);
        PRINT_LOG("  ofs_y: %d\n", g_dsc.ofs_y);
    } else {
        PRINT_LOG("  ✗ 未找到字符 'L'!\n");
    }
    
    PRINT_LOG("\n测试字符 '9' (Unicode 57):\n");
    found = lv_font_get_glyph_dsc_fmt_txt(&lv_font_roboto_21, &g_dsc, '9', '\0');
    
    if (found) {
        PRINT_LOG("  找到字符!\n");
        PRINT_LOG("  adv_w: %d\n", g_dsc.adv_w);
        PRINT_LOG("  box_w: %d\n", g_dsc.box_w);
        PRINT_LOG("  box_h: %d\n", g_dsc.box_h);
    } else {
        PRINT_LOG("  ✗ 未找到字符 '9'!\n");
    }
    
    /// 4. 检查位图数据的前几个字节
    PRINT_LOG("\n位图数据前 16 字节:\n  ");
    for (int i = 0; i < 16; i++) {
        PRINT_LOG("0x%02X ", dsc->glyph_bitmap[i]);
    }
    PRINT_LOG("\n");
    
    PRINT_LOG("\n========== 测试完成 ==========\n");
    PRINT_LOG("如果字符查找成功且参数合理，字体配置正确！\n\n");
}

