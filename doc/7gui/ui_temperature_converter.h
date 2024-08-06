#pragma once

/**
 * @brief Temperature Conversion application data.
 */
struct ui_temperature_converter_t;

/**
 * @brief Supported temperature scales.
 */
enum ui_temperature_scale_t
{
  CELCIUS    = 1, //<! freezing is 0 degrees, boiling is 100 degrees
  FAHRENHEIT = 2, //<! freezing is 32 degrees, boiling is 212 degrees
};

/**
 * @brief Ratio used to convert between Farenheit and Celcius.
 */
static const double FAHRENHEIT_CELCIUS_RATIO = 9.0 / 5.0;

/**
 * @brief Degrees Farenheight that represent the temperature at which water freezes.
 */
static const double FAHRENHEIT_FREEZING = 32.0;

/**
 * @brief Degrees Kelvin that represent one degree Celcius.
 */
static const double KELVIN_CELCIUS_OFFSET = -273.15;

/**
 * @brief Sets the temperature.
 * @param temperature_converter @p ui_temperature_converter_t
 * @param scale of the given @p degrees
 * @param degrees in the given @p scale
 */
void ui_temperature_converter_set (struct ui_temperature_converter_t *temperature_converter,
                                   enum ui_temperature_scale_t scale, double degrees);

/**
 * @brief Gets the temperature converted to the given scale.
 * @param temperature_converter @p ui_temperature_converter_t
 * @param scale of the given @p degrees
 * @return temperature in the given @p scale
 */
double ui_temperature_converter_get (struct ui_temperature_converter_t *temperature_converter,
                                     enum ui_temperature_scale_t        scale);

/**
 * @brief Runs the Temperature Converter application.
 * @param temperature_converter @p ui_temperature_converter_t
 * @return non-zero when exiting with an error
 */
int ui_temperature_converter_main (struct ui_temperature_converter_t *temperature_converter);
