#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "UIComponents.h"
#include "../core/Process.h"
#include "../core/SimulationResult.h"
#include "../core/GanttEntry.h"

using namespace std;

enum class Screen { MAIN, RESULTS };

// ── Preset scenario data ──────────────────────────────
struct ScenarioPreset {
    string name;
    string description;
    int quantum;
    // {id, arrival, burst, priority}
    vector<tuple<string,int,int,int>> procs;
};

class MainWindow {
public:
    MainWindow();
    void run();

private:
    sf::RenderWindow window;
    sf::Font         font;
    Screen           screen = Screen::MAIN;

    vector<Process>          processes;
    vector<SimulationResult> results;
    int   resultTab = 0;
    float scrollY   = 0;
    float targSY    = 0;

    // ── Input widgets ──────────────────────────────────
    TextBox tbId, tbArr, tbBurst, tbPrio, tbQ;
    TextBox* focus = nullptr;

    // ── Algorithm / scenario buttons ──────────────────
    int    algoSel = 0;
    Button algoBtns[4];   // Pre(P), Non-Pre(P), RR, Run All
    Button btnAdd, btnClear, btnRun, btnReset, btnBack;
    Button tabs[3];

    // ── Preset scenario buttons (A-E) ─────────────────
    Button scenarioBtns[5];
    static const ScenarioPreset PRESETS[5];

    // ── Status bar ─────────────────────────────────────
    string    statusMsg;
    sf::Color statusCol  = {56,210,140};
    float     statusT    = 0;

    // ── Private helpers ────────────────────────────────
    void buildUI();
    void handleEvent(const sf::Event& ev);
    void handleMain   (const sf::Event& ev, float mx, float my);
    void handleResults(const sf::Event& ev, float mx, float my);
    void update  (float dt);
    void drawMain();
    void drawResults();

    void doAdd();
    void doRun();
    void loadPreset(int idx);
    void setStatus(const string& msg, sf::Color col);
    void clearAll();
};
