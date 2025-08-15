#include <Particule/Core/Inputs/Devices/Keyboard.hpp>
#include <Particule/Core/System/gint.hpp>

using namespace Particule::Core::Inputs::Devices;

template<typename T>
Keyboard<Inputs::Button> Keyboard<T>::GetKey() {
    return Keyboard<Inputs::Button>(getkey_opt(GETKEY_REP_ALL, NULL).key);
}

static constexpr std::string keys_name[] = {"UNKNOWN", "F1", "F2", "F3", "F4", "F5", "F6", "SHIFT", "OPTN", "VARS", "MENU", "LEFT", "UP", "ALPHA", "SQUARE", "POWER", "EXIT", "DOWN", "RIGHT", "XOT", "LOG", "LN", "SIN", "COS", "TAN", "FRAC", "FD", "LEFTP", "RIGHTP", "COMMA", "ARROW", "7", "8", "9", "DEL", "4", "5", "6", "MUL", "DIV", "1", "2", "3", "ADD", "SUB", "0", "DOT", "EXP", "NEG", "EXE", "ACON", "HELP", "LIGHT", "X2", "CARET", "SWITCH", "LEFTPAR", "RIGHTPAR", "STORE", "TIMES", "PLUS", "MINUS"};
static constexpr int keys_value[] = {0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_SHIFT, KEY_OPTN, KEY_VARS, KEY_MENU, KEY_LEFT, KEY_UP, KEY_ALPHA, KEY_SQUARE, KEY_POWER, KEY_EXIT, KEY_DOWN, KEY_RIGHT, KEY_XOT, KEY_LOG, KEY_LN, KEY_SIN, KEY_COS, KEY_TAN, KEY_FRAC, KEY_FD, KEY_LEFTP, KEY_RIGHTP, KEY_COMMA,
                            KEY_ARROW, KEY_7, KEY_8, KEY_9, KEY_DEL, KEY_4, KEY_5, KEY_6, KEY_MUL, KEY_DIV, KEY_1, KEY_2, KEY_3, KEY_ADD, KEY_SUB, KEY_0, KEY_DOT, KEY_EXP, KEY_NEG, KEY_EXE, KEY_ACON, KEY_HELP, KEY_LIGHT, KEY_X2, KEY_CARET, KEY_SWITCH, KEY_LEFTPAR, KEY_RIGHTPAR, KEY_STORE, KEY_TIMES, KEY_PLUS, KEY_MINUS};

std::string Keyboard<Inputs::Button>::GetKeyName() const {
    auto it = std::find(std::begin(keys_value), std::end(keys_value), m_key);
    if (it != std::end(keys_value)) {
        int index = std::distance(std::begin(keys_value), it);
        return std::string(keys_name[index]);
    }
    return "UNKNOWN";
}