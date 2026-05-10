#pragma once
#include <SFML/Graphics.hpp>
#include <string>
using namespace std;

// ══════════════════════════════════════════════════════
//  Colour Palette
// ══════════════════════════════════════════════════════
static const sf::Color BG         {12, 14, 22};
static const sf::Color PANEL      {20, 24, 40};
static const sf::Color CARD       {28, 33, 52};
static const sf::Color CARD2      {35, 41, 62};
static const sf::Color ACCENT     {82, 170, 255};
static const sf::Color ACCENT2    {167, 119, 255};
static const sf::Color SUCCESS    {56, 210, 140};
static const sf::Color WARNING    {255, 184, 76};
static const sf::Color DANGER     {255, 100, 100};
static const sf::Color TEXT_MAIN  {220, 228, 242};
static const sf::Color TEXT_MUTED {105, 118, 142};
static const sf::Color BORDER     {42, 52, 74};
static const sf::Color INPUT_BG   {22, 27, 44};
static const sf::Color INPUT_ACT  {30, 38, 60};

static const sf::Color PCOLS[10] = {
    {82,170,255},{167,119,255},{56,210,140},{255,184,76},{255,100,100},
    {100,230,210},{255,140,180},{255,210,60},{130,220,150},{200,150,255}
};

// ══════════════════════════════════════════════════════
//  Helpers
// ══════════════════════════════════════════════════════
inline sf::Text mkTxt(sf::Font& f, const string& s, int sz,
                      sf::Color c, float x, float y)
{
    sf::Text t;
    t.setFont(f); t.setString(s); t.setCharacterSize((unsigned)sz);
    t.setFillColor(c); t.setPosition(x, y);
    return t;
}

inline string f2(double v) {
    char buf[32]; snprintf(buf, sizeof(buf), "%.2f", v);
    return string(buf);
}

inline void drawRect(sf::RenderWindow& win, float x, float y,
                     float w, float h, sf::Color fill,
                     sf::Color outline = sf::Color::Transparent,
                     float thickness = 0.f)
{
    sf::RectangleShape r({w, h});
    r.setPosition(x, y);
    r.setFillColor(fill);
    if (thickness > 0.f) { r.setOutlineColor(outline); r.setOutlineThickness(thickness); }
    win.draw(r);
}

// ══════════════════════════════════════════════════════
//  TextBox
// ══════════════════════════════════════════════════════
struct TextBox {
    sf::RectangleShape box;
    sf::Text lbl, val, ph;
    string content, phStr;
    bool focused = false, cur = false;
    float ct = 0, x = 0, y = 0, w = 0, h = 0;

    void init(sf::Font& f, float px, float py, float pw, float ph2,
              const string& label, const string& placeholder);
    bool hit(float mx, float my) const;
    void add(char c);
    void del();
    void update(float dt);
    void draw(sf::RenderWindow& win);
    void clear() { content = ""; val.setString(""); }
};

// ══════════════════════════════════════════════════════
//  Button
// ══════════════════════════════════════════════════════
struct Button {
    sf::RectangleShape box;
    sf::Text txt;
    sf::Color nor, hov, act;
    float x = 0, y = 0, w = 0, h = 0;
    bool selected = false;

    void init(sf::Font& f, float px, float py, float pw, float ph,
              const string& label, sf::Color c);
    bool hit(float mx, float my) const;
    void upd(float mx, float my);
    void draw(sf::RenderWindow& win);
    void setLabel(sf::Font& f, const string& s);
};
