#ifndef INPUT_HPP
#define INPUT_HPP
#include <Particule/Core/Types/Vector2.hpp>
#include <Particule/Core/Types/Fixed.hpp>
#include <Particule/Core/System/gint.hpp>
#include <string>
#include <algorithm>

namespace Particule::Core
{
    constexpr std::string keys_name[] = {"UNKNOWN", "F1", "F2", "F3", "F4", "F5", "F6", "SHIFT", "OPTN", "VARS", "MENU", "LEFT", "UP", "ALPHA", "SQUARE", "POWER", "EXIT", "DOWN", "RIGHT", "XOT", "LOG", "LN", "SIN", "COS", "TAN", "FRAC", "FD", "LEFTP", "RIGHTP", "COMMA", "ARROW", "7", "8", "9", "DEL", "4", "5", "6", "MUL", "DIV", "1", "2", "3", "ADD", "SUB", "0", "DOT", "EXP", "NEG", "EXE", "ACON", "HELP", "LIGHT", "X2", "CARET", "SWITCH", "LEFTPAR", "RIGHTPAR", "STORE", "TIMES", "PLUS", "MINUS"};
    constexpr int keys_value[] = {0, gint::KEY_F1, gint::KEY_F2, gint::KEY_F3, gint::KEY_F4, gint::KEY_F5, gint::KEY_F6, gint::KEY_SHIFT, gint::KEY_OPTN, gint::KEY_VARS, gint::KEY_MENU, gint::KEY_LEFT, gint::KEY_UP, gint::KEY_ALPHA, gint::KEY_SQUARE, gint::KEY_POWER, gint::KEY_EXIT, gint::KEY_DOWN, gint::KEY_RIGHT, gint::KEY_XOT, gint::KEY_LOG, gint::KEY_LN, gint::KEY_SIN, gint::KEY_COS, gint::KEY_TAN, gint::KEY_FRAC, gint::KEY_FD, gint::KEY_LEFTP, gint::KEY_RIGHTP, gint::KEY_COMMA,
                                gint::KEY_ARROW, gint::KEY_7, gint::KEY_8, gint::KEY_9, gint::KEY_DEL, gint::KEY_4, gint::KEY_5, gint::KEY_6, gint::KEY_MUL, gint::KEY_DIV, gint::KEY_1, gint::KEY_2, gint::KEY_3, gint::KEY_ADD, gint::KEY_SUB, gint::KEY_0, gint::KEY_DOT, gint::KEY_EXP, gint::KEY_NEG, gint::KEY_EXE, gint::KEY_ACON, gint::KEY_HELP, gint::KEY_LIGHT, gint::KEY_X2, gint::KEY_CARET, gint::KEY_SWITCH, gint::KEY_LEFTPAR, gint::KEY_RIGHTPAR, gint::KEY_STORE, gint::KEY_TIMES, gint::KEY_PLUS, gint::KEY_MINUS};
    class Input
    {
    private:
        int m_key;
    public:
        Input() : m_key(0) {};
        Input(int key) : m_key(key) {};
        Input(const Input& other) : m_key(other.m_key) {};
        Input& operator=(const Input& other) { m_key = other.m_key; return *this; };
        ~Input() {};

        inline bool IsKeyPressed() { return gint::keydown(m_key); };
        inline bool IsKeyDown() { return gint::keypressed(m_key); };
        inline bool IsKeyUp() { return gint::keyreleased(m_key); };
        inline std::string GetKeyName() { 
            //find the key in the array keys_value and return the corresponding name in keys_name
            auto it = std::find(std::begin(keys_value), std::end(keys_value), m_key);
            if (it != std::end(keys_value)) {
                int index = std::distance(std::begin(keys_value), it);
                return std::string(keys_name[index]);
            }
            return "UNKNOWN";
         };
        inline fixed12_32 Get1DAxis() { return 0; };
        inline Vector2<fixed12_32> Get2DAxis() { return Vector2<fixed12_32>(0, 0); };
    };
}

#endif // INPUT_HPP