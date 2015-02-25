#pragma once

#include <string>

#include <reflectionzeug/reflectionzeug_api.h>

namespace reflectionzeug
{

/**
 * \brief A simple RGBA color class for use as a property value.
 */
class REFLECTIONZEUG_API Color
{
public:
    static Color fromString(const std::string & string, bool * ok);

public:
    Color();
    explicit Color(unsigned int rgba);
    Color(int red, int green, int blue, int alpha = 255);

    int red() const;
    void setRed(int value);
    int green() const;
    void setGreen(int value);
    int blue() const;
    void setBlue(int value);
    int alpha() const;
    void setAlpha(int value);

    unsigned int rgba() const;
    void setRgba(unsigned int rgba);

    std::string asHex(bool alpha) const;

    std::string toString() const;
    std::string toString(bool alpha) const;

    bool operator==(const Color & rhs) const;
    bool operator!=(const Color & rhs) const;

protected:
    union {
        struct {
            unsigned char b;
            unsigned char g;
            unsigned char r;
            unsigned char a;
        } m_rgba;
        unsigned int m_v;
    };
};

} // namespace reflectionzeug
