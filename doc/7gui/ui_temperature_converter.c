#include "ui_temperature_converter.h"
#include "main.h"

#include <assert.h>
#include <ui_window.h>

struct ui_temperature_converter_t
{
  /// @brief The application's main window.
  struct ui_window_t *window;

  /// @brief Temperature is stored in degrees on the Kelvin scale.
  double kelvin;
};

void
ui_temperature_converter_set (struct ui_temperature_converter_t *temperature_converter,
                              const enum ui_temperature_scale_t scale, const double degrees)
{
  switch (scale)
    {
    case CELCIUS:
      temperature_converter->kelvin = degrees - KELVIN_CELCIUS_OFFSET;
      break;

    case FAHRENHEIT:
      temperature_converter->kelvin
          = (degrees - FAHRENHEIT_FREEZING) / FAHRENHEIT_CELCIUS_RATIO - KELVIN_CELCIUS_OFFSET;
      break;

    default:
      assert (0);
    }
}

double
ui_temperature_converter_get (struct ui_temperature_converter_t *temperature_converter,
                              const enum ui_temperature_scale_t  scale)
{
  switch (scale)
    {
    case CELCIUS:
      return temperature_converter->kelvin + KELVIN_CELCIUS_OFFSET;

    case FAHRENHEIT:
      return (temperature_converter->kelvin + KELVIN_CELCIUS_OFFSET) * FAHRENHEIT_CELCIUS_RATIO + FAHRENHEIT_FREEZING;

    default:
      assert (0);
    }
}

int
ui_temperature_converter_main (struct ui_temperature_converter_t *temperature_converter)
{
  ui_window_show (temperature_converter->window);

  const int exit_code = ui_main ();

  ui_window_destroy (temperature_converter->window);

  return exit_code;
}

int
main (void)
{
  struct ui_temperature_converter_t temperature_converter = { 0 };

  ui_temperature_converter_set (&temperature_converter, CELCIUS, 100.0);
  assert (ui_temperature_converter_get (&temperature_converter, CELCIUS) == 100.0);
  assert (ui_temperature_converter_get (&temperature_converter, FAHRENHEIT) == 212.0);

  ui_temperature_converter_set (&temperature_converter, FAHRENHEIT, 32.0);
  assert (ui_temperature_converter_get (&temperature_converter, FAHRENHEIT) == 32.0);
  assert (ui_temperature_converter_get (&temperature_converter, CELCIUS) == 0.0);

  temperature_converter.window = ui_window_create ("Temperature Converter", 320, 240, 0);
  assert (temperature_converter.window != NULL);

  return ui_temperature_converter_main (&temperature_converter);
}
