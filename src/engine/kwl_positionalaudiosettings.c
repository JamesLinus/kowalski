#include "kwl_positionalaudiosettings.h"

void kwlPositionalAudioSettings_setDefaults(kwlPositionalAudioSettings* settings)
{
    //set defaults. TODO: choose these properly
    settings->dopplerScale = 1.0f;
    settings->speedOfSound = 340.0f;
    settings->referenceDistance = 1.0f;
    settings->rolloffFactor = 0.25f;
    settings->distanceModel = KWL_INV_DISTANCE;
    settings->isListenerConeAttenuationEnabled = 1;
    settings->isEventConeAttenuationEnabled = 1;
    settings->clamp = 1;
    settings->maxDistance = 100.0f;
}
