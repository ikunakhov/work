#include "YUV.h"

QImage yuv_to_image(FILE* in_yuv, color_mode mode_1, pix_order_mod mod_2)
{
    QImage ret;
    s16i *matrix;
    if (mode_1 == YUV) matrix = YUV_matrix;
    if (mode_1 == YCbCr || mode_1 == YPbPr) matrix = YCbCr_matrix;
    if (mode_1 == LUV   || mode_1 == XYZ)    assert("Disabled");
    return ret;

}



void image_to_yuvfile(QImage in_image, s8c* filename, color_mode mode_1, pix_order_mod mod_2, pix_format mod_3)
{
    s16i *matrix;
    s16i *add_p;
    if (mode_1 == YUV)
    {
        matrix = YUV_matrix;
        add_p  = YUV_column;
    }
    if (mode_1 == YCbCr || mode_1 == YPbPr)
    {
        matrix = YCbCr_matrix;
        add_p = YCbCr_column;
    }
    if (mode_1 == LUV   || mode_1 == XYZ)    assert("Disabled");

    QImage img;
    img = in_image.convertToFormat(QImage::Format_RGB888);
    FILE *yuv_out;
    yuv_out = fopen((const char*)filename, "wb");
    s32i width = img.width(); s32i height = img.height();
    s32i end_of_img = 3 * width * height;
    s32i tmp_1; unsigned char tmp_2;

    if (mod_2 == PLANER_DATA)
    {
        for (s32i i = 0; i < end_of_img; i += 3)
        {
            tmp_1 = matrix[0] * img.bits()[i] + matrix[1] * img.bits()[i + 1] + matrix[2] * img.bits()[i + 2] + add_p[0];
            if (tmp_1 < 0) tmp_1 = 0;
            tmp_1 /= 1000; tmp_2 = tmp_1;
            putc(tmp_2, yuv_out);
        }
        for (s32i i = 0; i < end_of_img; i += 3)
        {
            tmp_1 = matrix[3] * img.bits()[i] + matrix[4] * img.bits()[i + 1] + matrix[5] * img.bits()[i + 2] + add_p[1];
            if (tmp_1 < 0) tmp_1 = 0;
            tmp_1 /= 1000; tmp_2 = tmp_1;
            putc(tmp_2, yuv_out);
        }
        for (s32i i = 0; i < end_of_img; i += 3)
        {
            tmp_1 = matrix[6] * img.bits()[i] + matrix[7] * img.bits()[i + 1] + matrix[8] * img.bits()[i + 2] + add_p[2];
            if (tmp_1 < 0) tmp_1 = 0;
            tmp_1 /= 1000; tmp_2 = tmp_1;
            putc(tmp_2, yuv_out);
        }
        fclose(yuv_out);
    }
    else {}

}