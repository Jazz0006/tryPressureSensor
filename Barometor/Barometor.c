#include "legato.h"
#include "interfaces.h"

static const char PressureFile[] = "/sys/devices/i2c-0/0-0076/iio:device1/in_pressure_input";

static le_timer_Ref_t SampleTimer;

le_result_t ReadDoubleFromFile
(
    const char *filePath,
    double *value
)
{
    le_result_t r = LE_OK;
    FILE *f = fopen(filePath, "r");
    if (f == NULL)
    {
        LE_WARN("Couldn't open '%s' - %m", filePath);
        r = LE_IO_ERROR;
        goto done;
    }

    int numScanned = fscanf(f, "%lf", value);
    if (numScanned != 1)
    {
        r = LE_FORMAT_ERROR;
    }

    fclose(f);
done:
    return r;
}

/**
 * Reports the pressure kPa.
 */
le_result_t mangOH_ReadPressureSensor
(
    double *reading
)
{
    return ReadDoubleFromFile(PressureFile, reading);
}

static void SampleTimerHandler(le_timer_Ref_t timer)
{
	double readPressure;
	double *p = &readPressure;

	mangOH_ReadPressureSensor(p);
    LE_INFO("The result of pressure reading is: %f", readPressure);
}

COMPONENT_INIT
{
	LE_INFO("Jazz: Starting process for pressure sensor");

	SampleTimer = le_timer_Create("Pressure Sensor Read");
	LE_ASSERT_OK(le_timer_SetMsInterval( SampleTimer, 5000)); // Every 5 second.
	LE_ASSERT_OK(le_timer_SetRepeat(SampleTimer, 0));
	LE_ASSERT_OK(le_timer_SetHandler(SampleTimer, SampleTimerHandler));
	le_timer_Start(SampleTimer);

}
