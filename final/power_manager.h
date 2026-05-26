#pragma once

#include "esp_display_panel.hpp"

void power_manager_init(esp_panel::board::Board *board);
void power_manager_tick();
