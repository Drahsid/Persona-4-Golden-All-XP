// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "globals.h"
#include "character.h"
#include "ini.h"

DWORD threadID;

HANDLE hProcess;
HANDLE asyncHandle;

protag_t* protag;
character_t* characters[NUM_CHARS];
int last_char_xp[NUM_CHARS];
int last_protag_xp  = 0;
bool break_step     = false;

float inactive_xp_scale = 1.0f;
DWORD update_rate = 8;

void create_config(std::string config_dir) {
    std::fstream config_stream;
    config_stream.open(config_dir, std::fstream::trunc | std::fstream::out); // Make sure file is empty
    config_stream.close();
    config_stream.open(config_dir, std::fstream::in | std::fstream::out);
    config_stream << "; Config file for inactive member xp injection by Drahsid\n[header]\nversion = 1\n\n[settings]\n\n; This option represents the scale of xp that inactive or reserve party members recieve. For example: 1.0 is 100%, 2.0 is 200% (double), 0.5 is 50% (half)\ninactive_xp_scale = 1.0\n; This option represents how often the injection will update in ms\nupdate_rate = 8\n\n\n\n";
    config_stream.close();
}

void update_last_xp() {
    int index;
    last_protag_xp  = protag->xp;

    for (index = 0; index < NUM_CHARS; index++) {
        last_char_xp[index] = characters[index]->xp;
    }
}

DWORD WINAPI asyncThread(HMODULE hModule) {
    DWORD pId;
    size_t size;
    int index;
    char* var;

    pId = GetCurrentProcessId();
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pId);

    protag = (*(protag_t**)PP_TO_PROTAG);
    for (index = 0; index < NUM_CHARS; index++) {
        // end of protag struct + (sizeof character struct * index); should make aob scan way easier in future version
        characters[index] = (character_t*)((((uint8_t*)protag) + sizeof(protag_t)) + (sizeof(character_t) * index));
    }
    

    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    printf("alloc'd console\n\n");

    
    getenv_s(&size, NULL, 0, "USERPROFILE");
    var = (char*)malloc(size * sizeof(char));
    getenv_s(&size, var, size, "USERPROFILE");
    std::string config_dir = std::string(var) + "\\Documents\\My Games\\inactivexp.ini";
    free(var);

    INIReader config(config_dir);

    if (config.ParseError() != 0) {
        printf("Failed to load config. If this is your first time running, a config was generated at %s\nOtherwise, there is something wrong with your config!\n", config_dir.c_str());
        create_config(config_dir);
    }
    else {
        inactive_xp_scale = config.GetFloat("settings", "inactive_xp_scale", 1.0f);
        update_rate = config.GetInteger("settings", "update_rate", 8);
    }

    printf("inactive xp scale is %f\nprotag ptr is %x\n\nprotag XP: %d\nyosuke XP: %d\nchie XP: %d\nyukiko XP: %d\nrise XP: %d\nkanji XP: %d\nnaoto XP: %d\nteddie XP: %d\n\n\n", inactive_xp_scale, protag,
        protag->xp, characters[eCharacterIndex::Yosuke]->xp, characters[eCharacterIndex::Chie]->xp, characters[eCharacterIndex::Yukiko]->xp, characters[eCharacterIndex::Rise]->xp, characters[eCharacterIndex::Kanji]->xp, characters[eCharacterIndex::Naoto]->xp, characters[eCharacterIndex::Teddie]->xp);

    update_last_xp();

    for (;;) {
        if (break_step) break;
        if (protag->xp != last_protag_xp) {
            int delta = (protag->xp - last_protag_xp);

            printf("protag gained %d xp\n", delta);

            delta = ceil(delta * inactive_xp_scale);
            
            printf("inactive xp gain is %d\n", delta);

            for (index = 0; index < NUM_CHARS; index++) {
                if (characters[index]->xp == last_char_xp[index]) character_t_add_xp(characters[index], delta, index);
            }

            update_last_xp();
            printf("\n");
        }

        Sleep(update_rate);
    }

    return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        asyncHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)asyncThread, hModule, NULL, &threadID);
    case DLL_PROCESS_DETACH:
        FreeConsole();
        CloseHandle(asyncHandle);
        break;
    }
    return TRUE;
}

