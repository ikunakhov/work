#ifndef _YUV_H
#define _YUV_H
#include <QImage>
#include <QString>
#include <assert.h>
#include <Project_init.h>
extern "C" {
#include <libswscale/swscale.h>
#include <libavformat/avformat.h>
}
    enum color_mode {
        YUV,
        YCbCr,
        YPbPr,
        LUV,
        XYZ
    };
    enum pix_order_mod {
        PLANER_DATA
    };

    enum pix_format {
        I444,
        I422,
        I420,
        I411,
    };

    QImage yuv_to_image (FILE* in_yuv, color_mode mod_1, pix_order_mod mod_2);
    void   image_to_yuvfile(QImage in_image, s8c* filename, color_mode mod_1, pix_order_mod mod_2, pix_format mod_3);

    static __32f RGB_YUV_matrix[3 * 3] =   { 1.0,  0.0,      1.13983,
                                             1.0, -0.39465, -0.58060,
                                             1.0,  2.03211,  0.0      };
    static __32f RGB_yuv_column[3 * 1] =   {-145.89824, 124.832, -260.11008}; 

    static __32f RGB_YCbCr_matrix[3 * 3] = { 65.481,  128.553, 24.966,
                                            -37.797, -74.203,  112.0,
                                             112.0,  -93.786, -18.214 };
    static __32f RGB_YCbCr_column[3 * 1] = {0 , 0, 0};
     
    static s16i  YUV_matrix[3 * 3]       = { 299,  587,  114,
                                            -147, -289,  436,
                                             615, -515, -100  };

    static s16i YCbCr_matrix[3 * 3]      = { 257,  504,   98,
                                            -148, -291,  439,
                                             615, -515, -100 };
    static s16i YUV_column[3 * 1]        = { 0,
                                             0,
                                             0 };
    static s16i YCbCr_column[3 * 1]      = { 16000,
                                             128000,
                                             128000 };

#endif