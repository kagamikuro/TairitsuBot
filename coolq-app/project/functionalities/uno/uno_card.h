#pragma once

#include <vector>
#include <string>

struct UnoCard final
{
public:
    enum Color
    {
        Red,
        Green,
        Blue,
        Yellow,
        WildCard
    };
    enum Number
    {
        Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
        Reverse, Skip, DrawTwo,
        Wild, DrawFour,
        None
    };
    Color color;
    Number number;
    UnoCard() : color(WildCard), number(None) {}
    UnoCard(const int color, const int number) : color(Color(color)), number(Number(number)) {}
    UnoCard(const int color, const Number number) : color(Color(color)), number(number) {}
    UnoCard(const Color color, const int number) : color(color), number(Number(number)) {}
    UnoCard(const Color color, const Number number) : color(color), number(number) {}
    std::string to_string() const
    {
        std::string result;
        switch (color)
        {
        case Red: result = u8"红色"; break;
        case Green: result = u8"绿色"; break;
        case Blue: result = u8"蓝色"; break;
        case Yellow: result = u8"黄色"; break;
        case WildCard: break;
        }
        if (number < 10)
            result += char(number + '0');
        else switch (number)
        {
        case Reverse: result += u8"转向"; break;
        case Skip: result += u8"跳过"; break;
        case DrawTwo: result += "+2"; break;
        case Wild: result += u8"变色"; break;
        case DrawFour: result += "+4"; break;
        default: break;
        }
        return result;
    }
    bool can_use_after(const UnoCard& previous) const
    {
        return previous.number == None || number == previous.number || color == previous.color || color == WildCard;
    }
    static std::vector<UnoCard> get_all_cards()
    {
        std::vector<UnoCard> result;
        for (int i = 0; i < 4; i++)
        {
            result.emplace_back(WildCard, Wild);
            result.emplace_back(WildCard, DrawFour);
            result.emplace_back(i, Zero);
            for (int j = 1; j <= 12; j++)
            {
                result.emplace_back(i, j);
                result.emplace_back(i, j);
            }
        }
        return result;
    }
    friend bool operator < (const UnoCard& left, const UnoCard& right)
    {
        if (left.color < right.color) return true;
        if (left.color > right.color) return false;
        return left.number < right.number;
    }
    friend bool operator == (const UnoCard& left, const UnoCard& right)
    {
        return left.color == right.color && left.number == right.number;
    }
};
