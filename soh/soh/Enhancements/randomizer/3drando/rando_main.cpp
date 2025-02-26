#include "menu.hpp"
#include "hint_list.hpp"
#include "item_list.hpp"
#include "item_location.hpp"
#include "location_access.hpp"
#include "rando_main.hpp"
// #include <soh/Enhancements/randomizer.h>
#include <Cvar.h>
#include <Window.h>
#include <PR/ultra64/types.h>

#define TICKS_PER_SEC 268123480.0

void RandoMain::GenerateRando(std::unordered_map<RandomizerSettingKey, u8> cvarSettings) {
    HintTable_Init();
    ItemTable_Init();
    LocationTable_Init();

    // std::string settingsFileName = "./randomizer/latest_settings.json";
    // CVar_SetString("gLoadedPreset", settingsFileName.c_str());

    std::string fileName = Ship::Window::GetPathRelativeToAppDirectory(GenerateRandomizer(cvarSettings).c_str());
    CVar_SetString("gSpoilerLog", fileName.c_str());

    CVar_Save();
    CVar_Load();
    CVar_SetS32("gNewSeedGenerated", 1);
}