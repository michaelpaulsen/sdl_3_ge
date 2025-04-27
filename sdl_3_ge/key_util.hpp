#pragma once
#include <string>
#include "Console.hpp"
#include <SDL3/SDL.h>
std::string white_space_to_display(const char key) {

    switch (key) {
    case 0x00: return "<NUL>";
    case 0x01: return "<SOH>";
    case 0x02: return "<STX>";
    case 0x03: return "<STX>";
    case 0x04: return "<EOT>";
    case 0x05: return "<ENQ>";
    case 0x06: return "<ACK>";
    case 0x07: return "<BEL>";
    case 0x08: return "<BS>";
    case 0x09: return "<TAB>";
    case 0x0a: return "<LF>";
    case 0x0b: return "<VT>";
    case 0x0c: return "<FF>";
    case 0x0d: return "<CR>";
    case 0x0e: return "<SO>";
    case 0x0f: return "<SI>";
    case 0x10: return "<DLE>";
    case 0x11: return "<DC1>";
    case 0x12: return "<DC2>";
    case 0x13: return "<DC3>";
    case 0x14: return "<DC4>";
    case 0x15: return "<NAK>";
    case 0x16: return "<SYN>";
    case 0x17: return "<ETB>";
    case 0x18: return "<CAN>";
    case 0x19: return "<EM>";
    case 0x1a: return "<SUB>";
    case 0x1b: return "<ESC>";
    case 0x1c: return "<FS>";
    case 0x1d: return "<GS>";
    case 0x1e: return "<RS>";
    case 0x1f: return "<US>";
    case 0x20: return "<SPACE>";
    case 0x7f: return "<DEL>";
    default: return std::string(1, key);
    }
}

void debug_mod_keys(SKC::Console& console, SDL_Keymod mod) {
    if (mod & SDL_KMOD_SHIFT) { console.Inform("<SHIFT> "); }
    if (mod & SDL_KMOD_ALT) { console.Inform("<ALT> "); }
    if (mod & SDL_KMOD_GUI) { console.Inform("<MENU> "); }
    if (mod & SDL_KMOD_SCROLL) { console.Inform("<SCR LCK> "); }
    if (mod & SDL_KMOD_NUM) { console.Inform("<NUM LCK> "); }
}
void debug_key_codes(SKC::Console& console, int key, bool caps = false) {
    if (key < 256) {
        if (key < 21 || key == 127) {
            console.Inform("key \"", white_space_to_display((char)key), "\"\r");
            return;
        }
        if (caps) {
            console.Inform("key \"", (char)toupper((char)key), "\"\r");
            return;
        }
        console.Inform("key \"", (char)key, "\"\r");
        return;
    }
}

void debug_key_events(SKC::Console& console, SDL_Event evnt) {
    auto keye = evnt.key;
    auto key = keye.key;
    auto mod = keye.mod;
    console.ClearLine();
    debug_mod_keys(console, mod);
    debug_key_codes(console, key, mod & SDL_KMOD_CAPS);
}
