#ifndef DISP_BMP_T_H
#define DISP_BMP_T_H

#include <stdint.h>
#include <string>

using namespace std;

enum mix_bmp_type_t{
    NOT_MIX_BMP = 0,
    MIX_BMP_INT8,
    MIX_BMP_UINT8,
    MIX_BMP_INT16,
    MIX_BMP_UINT16,
    MIX_BMP_INT32,
    MIX_BMP_UINT32,
    MIX_BMP_FLOAT,
    MIX_BMP_DOUBLE,
    NUM_MIX_BMP_TYPE
};

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint8_t *addr;
    int N;
    double min;
    double max;
}color_map_t;
#pragma pack(pop)

/**
 * @brief Save map as mix bmp file.
 * @param[in] path
 * @param[in] width
 * @param[in] height
 * @param[in] type
 * @param[in] buf
 * @param[in] cmapN The bins of color map, normally 32
 * @param[in] cmap_min The minimum value of color map.
 * @param[in] cmap_max The maximum value of color map.
 * @return
 */
int save_mix_bmp_file(string path, int width, int height, mix_bmp_type_t type,
                     uint8_t *buf,  int cmapN, double cmap_min, double cmap_max);

/**
 * @brief Read mix bmp file, for raw data and bmp data.
 * @param[in] path
 * @param[out] W
 * @param[out] H
 * @param[out] type
 * @param[out] data
 * @param[out] data_rgb
 * @param[out] cmap_out
 * @return
 */
int read_mix_bmp_file(string path, int *W, int *H, mix_bmp_type_t *type,
                      void** data, uint8_t** data_rgb, color_map_t *cmap_out);


/**
 * @brief Read mix bmp file, only for raw data.
 * @param[in] path
 * @param[out] W
 * @param[out] H
 * @param[out] type
 * @param[out] data
 * @param[out] data_rgb
 * @param[out] cmap_out
 * @return
 */
int read_mix_bmp_file(string path, int *W, int *H, mix_bmp_type_t *type,
                      void** data);

#endif // DISP_BMP_T_H
