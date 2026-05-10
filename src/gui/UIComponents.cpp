#include "gui/UIComponents.h"

// ══════════════════════════════════════
//  TextBox
// ══════════════════════════════════════
void TextBox::init(sf::Font& f, float px, float py, float pw, float ph2,
                   const string& l, const string& p) {
    x=px; y=py; w=pw; h=ph2; phStr=p;
    box.setPosition(px,py); box.setSize({pw,ph2});
    box.setFillColor(INPUT_BG);
    box.setOutlineThickness(1.5f);
    box.setOutlineColor(BORDER);
    lbl = mkTxt(f, l, 10, TEXT_MUTED, px, py-16);
    val = mkTxt(f, "", 13, TEXT_MAIN,  px+8, py+(ph2-14)/2.f);
    ph  = mkTxt(f, p, 13, TEXT_MUTED, px+8, py+(ph2-14)/2.f);
}

bool TextBox::hit(float mx, float my) const {
    return mx>=x && mx<=x+w && my>=y && my<=y+h;
}

void TextBox::add(char c) {
    if (content.size() < 12) { content += c; val.setString(content); }
}

void TextBox::del() {
    if (!content.empty()) { content.pop_back(); val.setString(content); }
}

void TextBox::update(float dt) {
    ct += dt;
    if (ct > 0.5f) { cur = !cur; ct = 0; }
    box.setOutlineColor(focused ? ACCENT : BORDER);
    box.setFillColor(focused ? INPUT_ACT : INPUT_BG);
}

void TextBox::draw(sf::RenderWindow& win) {
    win.draw(box);
    win.draw(lbl);
    if (content.empty() && !focused)
        win.draw(ph);
    else {
        val.setString(content + (focused && cur ? "|" : ""));
        win.draw(val);
    }
}

// ══════════════════════════════════════
//  Button
// ══════════════════════════════════════
void Button::init(sf::Font& f, float px, float py, float pw, float ph,
                  const string& l, sf::Color c) {
    x=px; y=py; w=pw; h=ph;
    nor = c;
    hov = sf::Color((sf::Uint8)min(255,(int)c.r+35),
                    (sf::Uint8)min(255,(int)c.g+35),
                    (sf::Uint8)min(255,(int)c.b+35));
    act = sf::Color((sf::Uint8)max(0,(int)c.r-35),
                    (sf::Uint8)max(0,(int)c.g-35),
                    (sf::Uint8)max(0,(int)c.b-35));
    box.setPosition(px,py); box.setSize({pw,ph}); box.setFillColor(c);
    txt = mkTxt(f, l, 11, sf::Color::White, 0, 0);
    auto b = txt.getLocalBounds();
    txt.setOrigin(b.left+b.width/2.f, b.top+b.height/2.f);
    txt.setPosition(px+pw/2.f, py+ph/2.f);
}

bool Button::hit(float mx, float my) const {
    return mx>=x && mx<=x+w && my>=y && my<=y+h;
}

void Button::upd(float mx, float my) {
    if (selected)
        box.setFillColor(act);
    else
        box.setFillColor(hit(mx,my) ? hov : nor);
}

void Button::draw(sf::RenderWindow& win) {
    win.draw(box);
    win.draw(txt);
}

void Button::setLabel(sf::Font& f, const string& s) {
    txt.setFont(f);
    txt.setString(s);
    auto b = txt.getLocalBounds();
    txt.setOrigin(b.left+b.width/2.f, b.top+b.height/2.f);
    txt.setPosition(x+w/2.f, y+h/2.f);
}
