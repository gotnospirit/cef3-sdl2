#ifndef _KEYBOARD_UTILS_H_
#define _KEYBOARD_UTILS_H_

#include <SDL.h>

int getKeyboardModifiers(uint16_t const mod)
{
    int result = EVENTFLAG_NONE;

    if (mod & KMOD_NUM)
    {
        result |= EVENTFLAG_NUM_LOCK_ON;
    }

    if (mod & KMOD_CAPS)
    {
        result |= EVENTFLAG_CAPS_LOCK_ON;
    }

    if (mod & KMOD_CTRL)
    {
        result |= EVENTFLAG_CONTROL_DOWN;
    }

    if (mod & KMOD_SHIFT)
    {
        result |= EVENTFLAG_SHIFT_DOWN;
    }

    if (mod & KMOD_ALT)
    {
        result |= EVENTFLAG_ALT_DOWN;
    }
    return result;
}

int getWindowsKeyCode(SDL_Keysym const &key)
{
    int result = 0;

    bool shift = !!(key.mod & KMOD_SHIFT);
    bool caps = !!(key.mod & KMOD_CAPS);
    bool alt_gr = !!(key.mod & KMOD_RALT);
    bool uppercase = (caps && !shift) || (shift && !caps);

    // mapped from azerty windows 8 asus laptop
    switch (key.sym)
    {
        case SDLK_RETURN:
            result = 13;
            break;
        case SDLK_ESCAPE:
            result = 27;
            break;
        case SDLK_BACKSPACE:
            result = 8;
            break;
        case SDLK_TAB:
            result = 9;
            break;
        case SDLK_SPACE:
            result = 32;
            break;
        case SDLK_EXCLAIM:
            result = uppercase ? 167 : 33; // § : !
            break;

        case SDLK_QUOTEDBL:
            result = 34;
            break;
        case SDLK_HASH:
            result = 35;
            break;
        case SDLK_DOLLAR:
            result = 36;
            break;
        case SDLK_PERCENT:
            result = 37;
            break;
        case SDLK_AMPERSAND:
            result = 38;
            break;
        case SDLK_QUOTE:
            result = 39;
            break;
        case SDLK_LEFTPAREN:
            result = 40;
            break;
        case SDLK_RIGHTPAREN:
            result = alt_gr ? 93 : uppercase ? 176 : 41; // ] ? ° : )
            break;
        case SDLK_ASTERISK:
            result = uppercase ? 181 : 42; // µ : *
            break;
        case SDLK_PLUS:
            result = 43;
            break;
        case SDLK_COMMA:
            result = uppercase ? 63 : 44; // '?' : ,
            break;
        case SDLK_MINUS:
            result = 45;
            break;
        case SDLK_PERIOD:
            result = 46;
            break;
        case SDLK_SLASH:
            result = 47;
            break;

        case SDLK_0:
            result = alt_gr ? 64 : uppercase ? 48 : 224; // @ ? 0 : à
            break;
        case SDLK_1:
            result = uppercase ? 49 : 38; // 1 : & (KO)
            break;
        case SDLK_2:
            result = alt_gr ? 126 : uppercase ? 50 : 233; // ~ ? 2 : é
            break;
        case SDLK_3:
            result = alt_gr ? 35 : uppercase ? 51 : 34; // # ? 3 : "
            break;
        case SDLK_4:
            result = alt_gr ? 123 : uppercase ? 52 : 39; // { ? 4 : '
            break;
        case SDLK_5:
            result = alt_gr ? 91 : uppercase ? 53 : 40; // [ ? 5 : ( (KO)
            break;
        case SDLK_6:
            result = alt_gr ? 124 : uppercase ? 54 : 45; // | ? 6 : -
            break;
        case SDLK_7:
            result = alt_gr ? 96 : uppercase ? 55 : 232; // ` ? 7 : è
            break;
        case SDLK_8:
            result = alt_gr ? 92 : uppercase ? 56 : 95; // \ ? 8 : _
            break;
        case SDLK_9:
            result = alt_gr ? 94 : uppercase ? 57 : 231; // ^ ? 9 : ç
            break;

        case SDLK_COLON:
            result = uppercase ? 47 : 58; // / : :
            break;
        case SDLK_SEMICOLON:
            result = uppercase ? 46 : 59; // . (KO) : ;
            break;
        case SDLK_LESS:
            result = uppercase ? 62 : 60; // > : <
            break;
        case SDLK_EQUALS:
            result = alt_gr ? 125 : uppercase ? 43 : 61; // } ? + : =
            break;
        case SDLK_GREATER:
            result = 62;
            break;
        case SDLK_QUESTION:
            result = 63;
            break;
        case SDLK_AT:
            result = 64;
            break;
        case SDLK_LEFTBRACKET:
            result = 91;
            break;
        case SDLK_BACKSLASH:
            result = 92;
            break;
        case SDLK_RIGHTBRACKET:
            result = 93;
            break;
        case SDLK_CARET:
            result = uppercase ? 168 : 94; // ^ : ¨
            break;
        case SDLK_UNDERSCORE:
            result = 95;
            break;
        case SDLK_BACKQUOTE:
            result = 96;
            break;

        case SDLK_a:
            result = uppercase ? 65 : 97;
            break;
        case SDLK_b:
            result = uppercase ? 66 : 98;
            break;
        case SDLK_c:
            result = uppercase ? 67 : 99;
            break;
        case SDLK_d:
            result = uppercase ? 68 : 100;
            break;
        case SDLK_e:
            result = uppercase ? 69 : 101;
            break;
        case SDLK_f:
            result = uppercase ? 70 : 102;
            break;
        case SDLK_g:
            result = uppercase ? 71 : 103;
            break;
        case SDLK_h:
            result = uppercase ? 72 : 104;
            break;
        case SDLK_i:
            result = uppercase ? 73 : 105;
            break;
        case SDLK_j:
            result = uppercase ? 74 : 106;
            break;
        case SDLK_k:
            result = uppercase ? 75 : 107;
            break;
        case SDLK_l:
            result = uppercase ? 76 : 108;
            break;
        case SDLK_m:
            result = uppercase ? 77 : 109;
            break;
        case SDLK_n:
            result = uppercase ? 78 : 110;
            break;
        case SDLK_o:
            result = uppercase ? 79 : 111;
            break;
        case SDLK_p:
            result = uppercase ? 80 : 112;
            break;
        case SDLK_q:
            result = uppercase ? 81 : 113;
            break;
        case SDLK_r:
            result = uppercase ? 82 : 114;
            break;
        case SDLK_s:
            result = uppercase ? 83 : 115;
            break;
        case SDLK_t:
            result = uppercase ? 84 : 116;
            break;
        case SDLK_u:
            result = uppercase ? 85 : 117;
            break;
        case SDLK_v:
            result = uppercase ? 86 : 118;
            break;
        case SDLK_w:
            result = uppercase ? 87 : 119;
            break;
        case SDLK_x:
            result = uppercase ? 88 : 120;
            break;
        case SDLK_y:
            result = uppercase ? 89 : 121;
            break;
        case SDLK_z:
            result = uppercase ? 90 : 122;
            break;
    }
    return result;
}

#endif // _KEYBOARD_UTILS_H_