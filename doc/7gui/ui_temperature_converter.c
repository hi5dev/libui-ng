#include "main.h"

#include "ui_temperature_converter.h"

#include <assert.h>

struct ui_temperature_converter_t
{
  double kelvin; //!< @brief Temperature is stored in degrees on the Kelvin scale.
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
ui_temperature_converter_main (struct ui_temperature_converter_t *)
{
  return ui_main ();
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

  return ui_temperature_converter_main (&temperature_converter);
}
