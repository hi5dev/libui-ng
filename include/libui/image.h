#pragma once

#include "api.h"

/**
 * @brief A container for an image to be displayed on screen.
 *
 * The container can hold multiple representations of the same image with the _same_ aspect ratio but in different
 * resolutions to support high-density displays.
 *
 * Common image dimension scale factors are `1x` and `2x`. Providing higher density representations is entirely
 * optional.
 *
 * The system will automatically determine the correct image to render depending on the screen's pixel density.
 *
 * @remark @p uiImage only supports premultiplied 32-bit RGBA images.
 * @remark No image file loading or image format conversion utilities are provided.
 */
typedef struct uiImage uiImage;

/**
 * @brief @p uiImage constructor
 * @param width in points
 * @param height in points
 * @return @p uiImage
 */
API uiImage *uiNewImage (double width, double height);

/**
 * @brief @p uiImage destructor
 * @param i @p uiImage
 */
API void uiFreeImage (uiImage *i);

/**
 * @brief Appends a new image representation.
 * @param i @p uiImage
 * @param pixels Byte array of premultiplied pixels in [R G B A] order.
 * @param pixelWidth Width in pixels.
 * @param pixelHeight Height in pixels.
 * @param byteStride Number of bytes per row of the pixel array.
 * @remark @code pixels@endcode must be at least @code byteStride * pixelHeight@endcode bytes long
 * @remark @code pixels@endcode is copied internally; ownership is not transferred
 * @remark @code ((uint8_t *) pixels)[0]@endcode is the first pixel's red value
 * @remark @code ((uint8_t *) pixels)[1]@endcode is the first pixel's green value
 * @remark @code ((uint8_t *) pixels)[2]@endcode is the first pixel's blue value
 * @remark @code ((uint8_t *) pixels)[3]@endcode is the first pixel's alpha value
 */
API void uiImageAppend (uiImage *i, void *pixels, int pixelWidth, int pixelHeight, int byteStride);
