#include <iostream>
#include <fstream>

using namespace std;

#include "mix_bmp.h"

#define INVALID_MIN -128.0 * 16
#define INVALID_MAX 128.0 * 16

#define UNUSED(x) (x)=(x);

#ifndef PRINT_ERROR
#define PRINT_ERROR(...)    \
  do{                       \
    printf("\x1B[41;37m");  \
    printf("[ERROR] ");      \
    printf("\x1B[0m");      \
    printf(__VA_ARGS__);    \
    }                       \
  while(0)
#endif

#ifndef PRINT_INFO
#define PRINT_INFO(...)     \
  do{                       \
    printf("\1xB[42;37m");  \
    printf("[INFO] ");      \
    printf(__VA_ARGS__);    \
    }                       \
  while(0)
#endif

# pragma pack(push)
# pragma pack(1)
typedef struct {
    /* type : Magic identifier,BM(0x42,0x4d) */
    unsigned short int type;
    unsigned int size;/* File size in bytes */
    unsigned short int number_type; // number type of mix bmp.
    unsigned short int reserved2;
    unsigned int offset;/* Offset to image data, bytes */
} BmpFileHeader;

typedef struct {
    unsigned int size;/* Info Header size in bytes */
    int width, height;/* Width and height of image */
    unsigned short planes;/* Number of colour planes */
    unsigned short bits; /* Bits per pixel */
    unsigned int compression; /* Compression type */
    unsigned int imagesize; /* Image size in bytes */
    int xresolution, yresolution; /* Pixels per meter */
    unsigned int ncolours; /* Number of colours */
    unsigned int importantcolours; /* Important colours */
} BmpInfoHeader;
#pragma pack(pop)

// The memory is allocated inside, remember to release outside.
int cvt_uint8_to_rgb(void *data, color_map_t cmap, int W, int H,
    uint8_t** data_rgb)
{
    UNUSED(cmap);
    UNUSED(W);
    UNUSED(H);
    *data_rgb = (uint8_t*)data;
    return 0;
}

// The memory is allocated inside, remember to release outside.
int cvt_int16_to_rgb(void *data, color_map_t cmap, int W, int H,
                uint8_t** data_rgb)
{
    *data_rgb = new uint8_t[W*H*3];
    int16_t* data_i16 = (int16_t *)data;
    int16_t min = (int16_t)cmap.min;
    int16_t max = (int16_t)cmap.max;
    for(int r=0; r<H; r++)
    {
        for(int c=0; c<W; c++)
        {
            int16_t v = data_i16[r*W+c];
            if(v>INVALID_MIN && v<INVALID_MAX)
            {
                int loc = (int) ( (float)(v-min)/(float)(max-min) * (float)cmap.N);   // divided by 16
                loc = loc > cmap.N-1 ? (cmap.N-1) : (loc<0 ? 0 : loc);
                (*data_rgb)[r*W*3 + c*3 + 0] = cmap.addr[loc*3 + 0];    //Red
                (*data_rgb)[r*W*3 + c*3 + 1] = cmap.addr[loc*3 + 1];    //Green
                (*data_rgb)[r*W*3 + c*3 + 2] = cmap.addr[loc*3 + 2];    //Blue
            }
            else    // Invalid value.
            {
                (*data_rgb)[r*W*3 + c*3 + 0] = 0;   //Red
                (*data_rgb)[r*W*3 + c*3 + 1] = 0;   //Green
                (*data_rgb)[r*W*3 + c*3 + 2] = 0;   //Blue
            }
        }
    }
    return 0;
}

// The memory is allocated inside, remember to release outside.
int cvt_float_to_rgb(void *data, color_map_t cmap, int W, int H,
    uint8_t** data_rgb)
{
    *data_rgb = new uint8_t[W*H * 3];
    float* data_f = (float *)data;
    float min = (float)cmap.min;
    float max = (float)cmap.max;
    for (int r = 0; r<H; r++)
    {
        for (int c = 0; c<W; c++)
        {
            float v = data_f[r*W + c];
            if (v > INVALID_MIN && v <= INVALID_MAX)
            {
                int loc = (int)((v - min) / (max - min) * (float)cmap.N);   // divided by 16
                loc = loc > cmap.N - 1 ? (cmap.N - 1) : (loc<0 ? 0 : loc);
                (*data_rgb)[r*W * 3 + c * 3 + 0] = cmap.addr[loc * 3 + 0];    //Red
                (*data_rgb)[r*W * 3 + c * 3 + 1] = cmap.addr[loc * 3 + 1];    //Green
                (*data_rgb)[r*W * 3 + c * 3 + 2] = cmap.addr[loc * 3 + 2];    //Blue
            }
            else    // Invalid value.
            {
                (*data_rgb)[r*W * 3 + c * 3 + 0] = 0;   //Red
                (*data_rgb)[r*W * 3 + c * 3 + 1] = 0;   //Green
                (*data_rgb)[r*W * 3 + c * 3 + 2] = 0;   //Blue
            }
        }
    }
    return 0;
}

int (*cvt_to_rgb[NUM_MIX_BMP_TYPE])(void *data, color_map_t cmap, int W,
                                    int H, uint8_t** data_rgb) =
{
    cvt_uint8_to_rgb,
    NULL,
    NULL,
    cvt_int16_to_rgb,
    NULL,
    NULL,
    NULL,
    cvt_float_to_rgb,
    NULL
};

const int g_mix_bmp_type_size[NUM_MIX_BMP_TYPE] =
{
    sizeof(uint8_t),
    sizeof(int8_t),
    sizeof(uint8_t),
    sizeof(int16_t),
    sizeof(uint16_t),
    sizeof(int32_t),
    sizeof(uint32_t),
    sizeof(float),
    sizeof(double)
};


// type: 1: int8, 2: uint8, 3: int16, 4: uint16, 5: int32, 6: uint32, 7 float, 8: double.
int init_mix_bmp_header(BmpFileHeader *fh, BmpInfoHeader *ih, int width,
                        int height, mix_bmp_type_t type)
{
    ih->size = sizeof(BmpInfoHeader);
    ih->width = width;
    ih->height = -height; // save data from up to down.
    ih->planes = 1;
    ih->bits = 24;
    ih->compression = 0;
    ih->imagesize = 3*width*height;
    ih->xresolution = 0;
    ih->yresolution = 0;
    ih->ncolours = 0;
    ih->importantcolours = 0;

    fh->type = 0x4d42;
    fh->size = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + width*height;
    fh->offset = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) ;
    fh->number_type = (short int)type;
    fh->reserved2 = 0;

    return 0;
}

// Memory is allocated in function, remember to free outside.
int create_color_map(int N, double min ,double max, color_map_t *cmap)
{
    uint8_t *cmap_addr = new uint8_t[N*3];
    float r = 0.4;
    int32_t s1=(int32_t)((float)N*r), s2 = N/2, s3=(int32_t)((float)N*(1.0-r));
    float k1,k2,k3,k4;
    k1 = 255.0 / r /(float)N;
    k2 = -255.0/(0.5-r)/(float)N;
    k3 = 255.0/(0.5-r)/(float)N;
    k4 = -255.0 / r /(float)N;
    for(int32_t i=0; i<N; i++)
    {
        if(i<s1)
        {
            cmap_addr[i*3] = 255;
            float pix = k1 *(float)i;
            pix = pix<0.0 ? 0.0 : (pix>255.0 ? 255 : pix);
            cmap_addr[i*3+1] = (uint8_t)(pix);
            cmap_addr[i*3+2] = 0;
        }
        else if(i<s2)
        {
            float pix = k2*(float)(i-s2);
            pix = pix<0.0 ? 0.0 : (pix>255.0 ? 255 : pix);
            cmap_addr[i*3] = (uint8_t)(pix);
            cmap_addr[i*3+1] = 255;
            cmap_addr[i*3+2] = 0;
        }
        else if(i<s3)
        {
            cmap_addr[i*3] = 0;
            cmap_addr[i*3+1] = 255;
            float pix = k3*(float)(i-s2);
            pix = pix<0.0 ? 0.0 : (pix>255.0 ? 255 : pix);
            cmap_addr[i*3+2] = (uint8_t)(pix);
        }
        else
        {
            cmap_addr[i*3] = 0;
            float pix = k4 *(float)(i-N);
            pix = pix<0.0 ? 0.0 : (pix>255.0 ? 255 : pix);
            cmap_addr[i*3+1] =  (uint8_t)(pix);
            cmap_addr[i*3+2] = 255;
        }
    }

    cmap->N = N;
    cmap->min = min;
    cmap->max = max;
    cmap->addr = cmap_addr;
    return 0;
}

int save_mix_bmp_file(string path, int width, int height, mix_bmp_type_t type,
                     uint8_t *buf,  int cmapN, double cmap_min, double cmap_max)
{
    if(NULL == cvt_to_rgb[type])
    {
        PRINT_ERROR("Type not support: %d", type);
        return -1;
    }

    ofstream file(path, ios::binary);
    if (!file.is_open())
    {
        PRINT_ERROR("Cannot open file to write:%s", path.c_str());
        return 1;
    }

    BmpFileHeader fh;
    BmpInfoHeader ih;
    init_mix_bmp_header(&fh, &ih, width, height, type);

    // Bmp file preview data
    file.write((char*)(&fh), sizeof(BmpFileHeader));
    file.write((char*)(&ih), sizeof(BmpInfoHeader));

    color_map_t cmap;
    create_color_map(cmapN, cmap_min, cmap_max, &cmap);

    uint8_t *data_rgb;
    cvt_to_rgb[type]((void*)buf, cmap, width, height, &data_rgb);
    file.write((char*)data_rgb, ih.imagesize);

    // mix binary data.
    if(type != NOT_MIX_BMP)
    {
        // Color map data
        file.write((char*)(&cmap.N), sizeof(int));
        file.write((char*)(&cmap.min), sizeof(double));
        file.write((char*)(&cmap.max), sizeof(double));
        file.write((char*)buf, width*height*g_mix_bmp_type_size[type]);
    }
    file.close();

    delete cmap.addr;
    delete data_rgb;

    return 0;
}


int read_mix_bmp_file(string path, int *W, int *H, mix_bmp_type_t *type,
                      void** data, uint8_t **data_rgb, color_map_t *cmap_out)
{
    cout<<path<<endl;
    setlocale(LC_ALL,"Chinese-simplified");
    ifstream ifs(path, ios::binary);
//    ifs.imbue(locale("chs"));

    if(!ifs.is_open())
    {
        PRINT_ERROR("Cannot open file: %s\n", path.c_str());
        return 1;
    }

    BmpFileHeader fh;
    BmpInfoHeader ih;
    ifs.read((char*)(&fh), sizeof(BmpFileHeader));
    ifs.read((char*)(&ih), sizeof(BmpInfoHeader));
    int w = ih.width;
    int h = abs(ih.height);
    if(ih.imagesize != 3*w*h)
    {
        PRINT_ERROR("image size: %u != w*h*3 !\n", ih.imagesize);
        return 1;
    }

    // Load binary data.
    // Use reserved1 to record mix bmp type!
    *type = (mix_bmp_type_t)(fh.number_type);
    *data = (void*) (new char[h*w*g_mix_bmp_type_size[*type]]);
    color_map_t cmap;
    if(NOT_MIX_BMP == *type)
    {
        ifs.seekg(fh.offset);
    }
    else
    {
        // Load bmp file
        ifs.seekg(fh.offset+ih.imagesize, ios::beg);
        // Load colormap
        ifs.read((char*)(&(cmap.N)), sizeof(int));
        ifs.read((char*)(&(cmap.min)), sizeof(double));
        ifs.read((char*)(&(cmap.max)), sizeof(double));
        create_color_map(cmap.N, cmap.min, cmap.max, &cmap);
    }
    ifs.read((char*)(*data), h*w*g_mix_bmp_type_size[*type]);


    cvt_to_rgb[*type](*data, cmap, w, h, data_rgb);

    *W = w;
    *H = h;
    *cmap_out = cmap;

    return 0;
}


int read_mix_bmp_file(string path, int *W, int *H, mix_bmp_type_t *type,
                      void** data)
{
    cout<<path<<endl;
    setlocale(LC_ALL,"Chinese-simplified");
    ifstream ifs(path, ios::binary);
//    ifs.imbue(locale("chs"));

    if(!ifs.is_open())
    {
        PRINT_ERROR("Cannot open file: %s\n", path.c_str());
        return 1;
    }

    BmpFileHeader fh;
    BmpInfoHeader ih;
    ifs.read((char*)(&fh), sizeof(BmpFileHeader));
    ifs.read((char*)(&ih), sizeof(BmpInfoHeader));
    int w = ih.width;
    int h = abs(ih.height);
    if(ih.imagesize != 3*w*h)
    {
        PRINT_ERROR("image size: %u != w*h*3 !\n", ih.imagesize);
        ifs.close();
        return 1;
    }

    // Load binary data.
    // Use reserved1 to record mix bmp type!
    *type = (mix_bmp_type_t)(fh.number_type);
    *data = (void*) (new char[h*w*g_mix_bmp_type_size[*type]]);
    color_map_t cmap;
    if(NOT_MIX_BMP == *type)
    {
        ifs.seekg(fh.offset);
    }
    else
    {
        // Load bmp file
        ifs.seekg(fh.offset+ih.imagesize, ios::beg);
        // Load colormap
        ifs.read((char*)(&(cmap.N)), sizeof(int));
        ifs.read((char*)(&(cmap.min)), sizeof(double));
        ifs.read((char*)(&(cmap.max)), sizeof(double));
        create_color_map(cmap.N, cmap.min, cmap.max, &cmap);
    }
    ifs.read((char*)(*data), h*w*g_mix_bmp_type_size[*type]);
    ifs.close();

//    cvt_to_rgb[*type](*data, cmap, w, h, data_rgb);

//    *W = w;
//    *H = h;
//    *cmap_out = cmap;

    return 0;
}
