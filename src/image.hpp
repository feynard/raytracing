#ifndef IMAGE_HPP
#define IMAGE_HPP

// STBI Configuration
#define STBIR_DEFAULT_FILTER_UPSAMPLE STBIR_FILTER_TRIANGLE

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>


#include <vector>
#include <string>
#include <tuple>
#include <iostream>

#include "vec.hpp"


// Declaration

template<typename T, int C>
class Image {
public:

    Image(const std::string & filename);
    Image(const int & width, const int & height, const Color<T, C> & color = Color<T, C>());

    // i-th row, j-th column
    Color<T, C> & operator() (const int & i, const int & j);
    Color<T, C> operator() (const int & i, const int & j) const;

    std::tuple<int, int> size();

    void resize(const int & width, const int & height);
    void fill(const Color<T, C> & color);
    void save(const std::string & filename);

    template <typename S>
    explicit operator Image<S, C> () const;

    // Arithmetic

    void operator*= (const T & a);
    void operator/= (const T & a);
    void operator+= (const T & a);
    void operator-= (const T & a);

    Image<T, C> operator* (const T & a) const;
    Image<T, C> operator/ (const T & a) const;
    Image<T, C> operator+ (const T & a) const;
    Image<T, C> operator- (const T & a) const;

    void operator*= (const Image<T, C> & img);
    void operator/= (const Image<T, C> & img);
    void operator+= (const Image<T, C> & img);
    void operator-= (const Image<T, C> & img);

    Image<T, C> operator* (const Image<T, C> & img) const;
    Image<T, C> operator/ (const Image<T, C> & img) const;
    Image<T, C> operator+ (const Image<T, C> & img) const;
    Image<T, C> operator- (const Image<T, C> & img) const;

    void flip_vertically();
    void flip_horizontally();

private:
    int width;
    int height;
    int channels;

    std::vector<Color<T, C>> data;
};


// Definition

template<typename T, int C>
void Image<T, C>::flip_vertically()
{
    for (int i = 0; i < height / 2; ++ i)
        for (int j = 0; j < width; ++j)
            std::swap(data[i * width + j], data[(height - i - 1) * width + j]);
}

template<typename T, int C>
void Image<T, C>::flip_horizontally()
{
    for (int i = 0; i < height; ++ i)
        for (int j = 0; j < width / 2; ++j)
            std::swap(data[i * width + j], data[i * width + width - 1 - j]);
}

template<typename T, int C>
Image<T, C>::Image(const std::string & filename)
{
    uint8_t * raw_data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    if constexpr (std::is_floating_point<T>::value) {
        data.resize(width * height);
        for (size_t i = 0; i < data.size(); ++i) {
            T * current_pixel = reinterpret_cast<T *>(data.data() + i);
            for (int j = 0; j < C; ++j)
                current_pixel[j] = static_cast<T>(raw_data[i * C + j]) / 255;
        }
    } else {
        data.assign(
            reinterpret_cast<Color<T, C> *>(raw_data),
            reinterpret_cast<Color<T, C> *>(raw_data) + width * height
        );
    }

    stbi_image_free(raw_data);
}

template<typename T, int C>
Image<T, C>::Image(
    const int & width,
    const int & height,
    const Color<T, C> & color) :

    width(width), height(height), channels(C), data(width * height, color)
{}

template<typename T, int C>
Color<T, C> & Image<T, C>::operator() (const int & i, const int & j)
{
    return data[width * i + j];
}

template<typename T, int C>
Color<T, C> Image<T, C>::operator() (const int & i, const int & j) const
{
    return data[width * i + j];
}

template<typename T, int C>
std::tuple<int, int> Image<T, C>::size()
{
    return { width, height };
}

template<typename T, int C>
void Image<T, C>::resize(const int & width, const int & height)
{
    std::vector<Color<T, C>> new_data(width * height);

    if constexpr (std::is_floating_point<T>::value)
        stbir_resize_float(
            reinterpret_cast<float *>(data.data()), this -> width, this -> height, 0,
            reinterpret_cast<float *>(new_data.data()), width, height, 0,
            channels);
    else
        stbir_resize_uint8(
            reinterpret_cast<uint8_t *>(data.data()), this -> width, this -> height, 0,
            reinterpret_cast<uint8_t *>(new_data.data()), width, height, 0,
            channels);

    data = new_data;
    this -> width = width;
    this -> height = height;
}

template<typename T, int C>
void Image<T, C>::fill(const Color<T, C> & color)
{
    for (int i = 0; i < width * height; ++i)
        data[i] = color;
}

template<typename T, int C>
void Image<T, C>::save(const std::string & filename)
{
    if constexpr (std::is_floating_point<T>::value) {
        static_cast<Image<uint8_t, C>>(*this).save(filename);
    } else {
        if (filename.ends_with(".png"))
            stbi_write_png(filename.c_str(), width, height, channels, data.data(), width * channels);
        if (filename.ends_with(".jpg") || filename.ends_with(".jpeg"))
            stbi_write_jpg(filename.c_str(), width, height, channels, data.data(), width * channels);
    }
}

template <typename T, int C>
template <typename S>
Image<T, C>::operator Image<S, C> () const
{
    Image<S, C> img(width, height);

    if constexpr (std::is_same<S, T>::value)
        img = *this;
    else
        for (int i = 0; i < height; ++i)
            for (int j = 0; j < width; ++j)
                img(i, j) = static_cast<Color<S, C>>((*this)(i, j));

    return img;
}

template <typename T, int C>
void Image<T, C>::operator*= (const T & a)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] *= a;
}

template <typename T, int C>
void Image<T, C>::operator/= (const T & a)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] /= a;
}

template <typename T, int C>
void Image<T, C>::operator+= (const T & a)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] += a;
}

template <typename T, int C>
void Image<T, C>::operator-= (const T & a)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] -= a;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator* (const T & a) const
{
    Image<T, C> img = (*this);
    img *= a;
    return img;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator/ (const T & a) const
{
    Image<T, C> img = (*this);
    img /= a;
    return img;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator+ (const T & a) const
{
    Image<T, C> img = (*this);
    img += a;
    return img;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator- (const T & a) const
{
    Image<T, C> img = (*this);
    img -= a;
    return img;
}

template <typename T, int C>
void Image<T, C>::operator*= (const Image<T, C> & img)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] *= img.data[i];
}

template <typename T, int C>
void Image<T, C>::operator/= (const Image<T, C> & img)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] /= img.data[i];
}

template <typename T, int C>
void Image<T, C>::operator+= (const Image<T, C> & img)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] += img.data[i];
}

template <typename T, int C>
void Image<T, C>::operator-= (const Image<T, C> & img)
{
    for (size_t i = 0; i < data.size(); ++i)
        data[i] -= img.data[i];
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator* (const Image<T, C> & img) const
{
    Image<T, C> new_img = (*this);
    new_img *= img;
    return new_img;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator/ (const Image<T, C> & img) const
{
    Image<T, C> new_img = (*this);
    new_img /= img;
    return new_img;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator+ (const Image<T, C> & img) const
{
    Image<T, C> new_img = (*this);
    new_img += img;
    return new_img;
}

template <typename T, int C>
Image<T, C> Image<T, C>::operator- (const Image<T, C> & img) const
{
    Image<T, C> new_img = (*this);
    new_img -= img;
    return new_img;
}


#endif // IMAGE_HPP
