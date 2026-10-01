#ifndef SAVES_H
#define SAVES_H

#include "settings.h"

void SaveProgress(int currentLevel);
void LoadProgress(int *currentLevel);

void SaveSettings(const SettingsState *state);
void LoadSettings(SettingsState *state);

#endif
