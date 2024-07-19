#ifndef BMPFORMAT_H
#define BMPFORMAT_H

#define BMP_MAGIC_NUMBER 0x4D42
#define BMP_HEADER_SIZE 40

enum bmp_compression_t {
    BMP_COMPRESSION_NONE = 0,
};

#pragma pack(push, 1)
struct bmp_header_t {
    uint16_t magic;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t data_offset;
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    bmp_compression_t compression;
    uint32_t image_size;
    uint32_t x_pixels_per_meter;
    uint32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
};
#pragma pack(pop)

#endif // BMPFORMAT_H