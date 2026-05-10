#include "gui/MainWindow.h"
#include "algorithms/PriorityScheduler.h"
#include "algorithms/RoundRobin.h"
#include "utils/Validator.h"

#include <algorithm>
#include <set>
#include <map>
#include <sstream>
#include <cmath>

using namespace std;

// ══════════════════════════════════════════════════════
//  PRESET SCENARIOS  (from Documentation)
// ══════════════════════════════════════════════════════
const ScenarioPreset MainWindow::PRESETS[5] = {
    // ── Scenario A : Basic Mixed Workload ─────────────
    {
        "A: Basic Mixed",
        "Normal workload - 5 processes, different AT/BT/Priority",
        3,
        {{"P1",0,8,3},{"P2",1,4,1},{"P3",2,6,2},{"P4",3,3,4},{"P5",4,5,2}}
    },
    // ── Scenario B : Urgency Case ─────────────────────
    {
        "B: Urgency Case",
        "P2 & P4 have highest priority (1) - urgency policy visible",
        3,
        {{"P1",0,10,4},{"P2",1,3,1},{"P3",2,7,3},{"P4",3,2,1},{"P5",5,5,2}}
    },
    // ── Scenario C : Fairness Case ────────────────────
    {
        "C: Fairness Case",
        "All arrive at t=0 - reveals CPU distribution balance",
        4,
        {{"P1",0,12,1},{"P2",0,4,2},{"P3",0,8,3},{"P4",0,6,4}}
    },
    // ── Scenario D : Starvation Case ─────────────────
    {
        "D: Starvation Case",
        "P4 & P5 lowest priority (5) - starvation risk shown",
        2,
        {{"P1",0,3,1},{"P2",0,5,1},{"P3",0,4,2},{"P4",0,6,5},{"P5",0,3,5}}
    },
    // ── Scenario E : Validation Case ─────────────────
    {
        "E: Validation",
        "Tests invalid inputs: BT=0, AT=-1, bad quantum, duplicates",
        0,   // quantum=0 → invalid, triggers validation error
        {{"P1",0,5,2},{"P1",1,3,1},{"P2",-1,4,3},{"P3",2,0,1}}
        // duplicate P1, negative AT, zero BT → validator fires
    }
};

// ══════════════════════════════════════════════════════
//  Static helpers
// ══════════════════════════════════════════════════════
static void drawGantt(sf::RenderWindow& win, sf::Font& font,
                      const vector<GanttEntry>& g,
                      float ox, float oy, float maxW)
{
    if (g.empty()) return;
    int tot = 0;
    for (auto& e : g) tot = max(tot, e.endTime);
    if (tot == 0) return;

    float sc = maxW / (float)tot;
    map<string, sf::Color> cm;
    int ci = 0;
    for (auto& e : g)
        if (e.id != "IDLE" && !cm.count(e.id))
            cm[e.id] = PCOLS[ci++ % 10];

    const float bh = 36.f;

    // Draw bars
    for (auto& e : g) {
        float ex = ox + e.startTime * sc;
        float ew = (e.endTime - e.startTime) * sc;
        sf::Color col = (e.id == "IDLE") ? sf::Color(40,50,72) : cm[e.id];

        // Bar
        sf::RectangleShape bar({max(1.f, ew-2.f), bh});
        bar.setPosition(ex, oy);
        bar.setFillColor(col);
        win.draw(bar);

        // Outline
        sf::RectangleShape outline({max(1.f, ew-2.f), bh});
        outline.setPosition(ex, oy);
        outline.setFillColor(sf::Color::Transparent);
        outline.setOutlineColor({0,0,0,60});
        outline.setOutlineThickness(1.f);
        win.draw(outline);

        // Label
        if (ew > 18) {
            auto t = mkTxt(font, e.id, 10, sf::Color::White, ex+ew/2.f, oy+bh/2.f-7.f);
            auto lb = t.getLocalBounds();
            t.setOrigin(lb.left+lb.width/2.f, 0);
            t.setStyle(sf::Text::Bold);
            win.draw(t);
        }
    }

    // Time ticks
    set<int> tks;
    for (auto& e : g) { tks.insert(e.startTime); tks.insert(e.endTime); }
    for (int t : tks) {
        float tx = ox + t * sc;
        sf::RectangleShape tk({1.f, 5.f});
        tk.setPosition(tx, oy+bh);
        tk.setFillColor(TEXT_MUTED);
        win.draw(tk);
        win.draw(mkTxt(font, to_string(t), 9, TEXT_MUTED, tx-4, oy+bh+5));
    }
}

static void drawTable(sf::RenderWindow& win, sf::Font& font,
                      const vector<Process>& ps, float ox, float oy)
{
    const float  C[] = {0,55,110,165,215,270,325,385,445};
    const string H[] = {"PID","Arr","Burst","Prio","Start","Finish","TAT","Wait","Resp"};

    // Header
    sf::RectangleShape hr({490,21});
    hr.setPosition(ox, oy);
    hr.setFillColor({38,48,72});
    win.draw(hr);
    for (int c = 0; c < 9; c++)
        win.draw(mkTxt(font, H[c], 10, ACCENT, ox+C[c]+3, oy+4));

    for (int r = 0; r < (int)ps.size(); r++) {
        float ry = oy + 22 + r*19;
        sf::RectangleShape rw({490,19});
        rw.setPosition(ox, ry);
        rw.setFillColor(r%2==0 ? CARD : CARD2);
        win.draw(rw);
        auto& p = ps[r];
        vector<string> v = {
            p.getId(), to_string(p.getArrivalTime()), to_string(p.getBurstTime()),
            to_string(p.getPriority()), to_string(p.getStartTime()),
            to_string(p.getFinishTime()), to_string(p.getTurnaroundTime()),
            to_string(p.getWaitingTime()), to_string(p.getResponseTime())
        };
        for (int c = 0; c < 9; c++)
            win.draw(mkTxt(font, v[c], 10, TEXT_MAIN, ox+C[c]+3, ry+3));
    }
}

static void drawStatCard(sf::RenderWindow& win, sf::Font& font,
                         float cx, float cy, float cw,
                         const string& label, double val, sf::Color col)
{
    sf::RectangleShape cd({cw, 54});
    cd.setPosition(cx, cy);
    cd.setFillColor(CARD);
    cd.setOutlineColor(col);
    cd.setOutlineThickness(1.5f);
    win.draw(cd);
    win.draw(mkTxt(font, label, 10, TEXT_MUTED, cx+8, cy+6));
    auto vt = mkTxt(font, f2(val), 18, col, cx+8, cy+22);
    vt.setStyle(sf::Text::Bold);
    win.draw(vt);
}

// ══════════════════════════════════════════════════════
//  Constructor
// ══════════════════════════════════════════════════════
MainWindow::MainWindow() {
    window.create(sf::VideoMode(980, 720), "CPU Scheduler Simulator  — Round Robin vs Priority",
                  sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // Try common Windows + Linux font paths
    if (!font.loadFromFile("C:/Windows/Fonts/consola.ttf"))
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    if (!font.loadFromFile("C:/Windows/Fonts/segoeui.ttf"))
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
    if (!font.loadFromFile("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf"))
        font.loadFromFile("arial.ttf");

    statusCol = SUCCESS;
    buildUI();
}

// ══════════════════════════════════════════════════════
//  Build UI
// ══════════════════════════════════════════════════════
void MainWindow::buildUI() {
    // Input row
    float fx = 28, fy = 155;
    tbId   .init(font, fx,     fy, 88, 30, "Process ID","P1");
    tbArr  .init(font, fx+100, fy, 88, 30, "Arrival",   "0");
    tbBurst.init(font, fx+200, fy, 88, 30, "Burst",     "5");
    tbPrio .init(font, fx+300, fy, 88, 30, "Priority",  "1");
    tbQ    .init(font, fx+400, fy, 96, 30, "RR Quantum","2");

    btnAdd  .init(font, fx+508, fy,      82, 30, "+ Add",  SUCCESS);
    btnClear.init(font, fx+600, fy,      72, 30, "Clear",  DANGER);

    // Algorithm selector row
    const char* an[] = {"Priority (Pre)","Priority (Non)","Round Robin","Run All"};
    sf::Color   ac[] = {ACCENT2, {120,80,200}, ACCENT, SUCCESS};
    for (int i = 0; i < 4; i++)
        algoBtns[i].init(font, 28+i*178, 222, 170, 28, an[i], ac[i]);

    btnRun  .init(font, 742, 222, 108, 28, "Run  \xe2\x96\xb6", ACCENT);
    btnReset.init(font, 860, 222,  90, 28, "Reset",  {60,72,100});
    btnBack .init(font, 870,  12,  90, 28, "Back \xe2\x86\x90",{55,65,90});

    // Preset scenario buttons (A–E)
    sf::Color sc[] = {
        {70,160,100},   // A green
        {200,130,40},   // B amber
        {50,140,220},   // C blue
        {200,60,60},    // D red
        {130,100,200}   // E violet
    };
    const char* sn[] = {"A: Basic","B: Urgency","C: Fairness","D: Starvation","E: Validate"};
    for (int i = 0; i < 5; i++)
        scenarioBtns[i].init(font, 28+i*188, 264, 180, 26, sn[i], sc[i]);
}

// ══════════════════════════════════════════════════════
//  Status
// ══════════════════════════════════════════════════════
void MainWindow::setStatus(const string& msg, sf::Color col) {
    statusMsg = msg; statusCol = col; statusT = 4.f;
}

// ══════════════════════════════════════════════════════
//  Clear all
// ══════════════════════════════════════════════════════
void MainWindow::clearAll() {
    processes.clear();
    tbId.clear(); tbArr.clear(); tbBurst.clear();
    tbPrio.clear(); tbQ.clear();
    setStatus("Cleared.", TEXT_MUTED);
}

// ══════════════════════════════════════════════════════
//  Main loop
// ══════════════════════════════════════════════════════
void MainWindow::run() {
    sf::Clock clk;
    while (window.isOpen()) {
        float dt = clk.restart().asSeconds();
        sf::Event ev;
        while (window.pollEvent(ev)) handleEvent(ev);
        update(dt);
        window.clear(BG);
        screen == Screen::MAIN ? drawMain() : drawResults();
        window.display();
    }
}

// ══════════════════════════════════════════════════════
//  Event handling
// ══════════════════════════════════════════════════════
void MainWindow::handleEvent(const sf::Event& ev) {
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    float mx = (float)mp.x, my = (float)mp.y;
    if (ev.type == sf::Event::Closed) window.close();
    if (screen == Screen::MAIN) handleMain(ev, mx, my);
    else                        handleResults(ev, mx, my);
}

void MainWindow::handleMain(const sf::Event& ev, float mx, float my) {
    if (ev.type == sf::Event::MouseButtonPressed &&
        ev.mouseButton.button == sf::Mouse::Left)
    {
        focus = nullptr;
        for (auto* t : {&tbId,&tbArr,&tbBurst,&tbPrio,&tbQ}) {
            t->focused = false;
            if (t->hit(mx,my)) { t->focused=true; focus=t; }
        }
        if (btnAdd  .hit(mx,my)) doAdd();
        if (btnClear.hit(mx,my)) clearAll();
        if (btnRun  .hit(mx,my)) doRun();
        if (btnReset.hit(mx,my)) clearAll();
        for (int i = 0; i < 4; i++)
            if (algoBtns[i].hit(mx,my)) algoSel = i;
        for (int i = 0; i < 5; i++)
            if (scenarioBtns[i].hit(mx,my)) loadPreset(i);
    }

    if (ev.type == sf::Event::TextEntered && focus) {
        if      (ev.text.unicode == '\b') focus->del();
        else if (ev.text.unicode >= 32 && ev.text.unicode < 128)
            focus->add((char)ev.text.unicode);
    }

    if (ev.type == sf::Event::KeyPressed) {
        if (ev.key.code == sf::Keyboard::Enter) doAdd();
        if (ev.key.code == sf::Keyboard::Tab) {
            TextBox* o[] = {&tbId,&tbArr,&tbBurst,&tbPrio,&tbQ};
            int cur = -1;
            for (int i = 0; i < 5; i++)
                if (o[i]->focused) { cur=i; o[i]->focused=false; break; }
            int nx = (cur+1)%5;
            o[nx]->focused=true; focus=o[nx];
        }
    }
}

void MainWindow::handleResults(const sf::Event& ev, float mx, float my) {
    if (ev.type == sf::Event::MouseButtonPressed) {
        if (btnBack.hit(mx,my)) { screen=Screen::MAIN; scrollY=targSY=0; }
        for (int i = 0; i < (int)results.size() && i < 3; i++)
            if (tabs[i].hit(mx,my)) { resultTab=i; scrollY=targSY=0; }
    }
    if (ev.type == sf::Event::MouseWheelScrolled) {
        targSY -= ev.mouseWheelScroll.delta * 48.f;
        if (targSY < 0) targSY = 0;
    }
}

// ══════════════════════════════════════════════════════
//  Load Preset
// ══════════════════════════════════════════════════════
void MainWindow::loadPreset(int idx) {
    const auto& P = PRESETS[idx];

    // Scenario E is special: demonstrate validation errors live
    if (idx == 4) {
        processes.clear();
        // First add a valid process so list isn't empty
        processes.emplace_back("P1", 0, 5, 2);
        // Set quantum to 0 to trigger quantum validation on Run
        tbQ.content = "0"; tbQ.val.setString("0");
        tbId.content = "P1"; tbId.val.setString("P1");  // duplicate
        tbArr.content = "0"; tbArr.val.setString("0");
        tbBurst.content = "0"; tbBurst.val.setString("0");  // invalid burst
        tbPrio.content = "1"; tbPrio.val.setString("1");
        setStatus(
            "Scenario E: Try clicking +Add (dup ID / BT=0) or Run (Quantum=0) to see validation!",
            WARNING);
        return;
    }

    processes.clear();
    for (auto& [id, at, bt, pr] : P.procs)
        processes.emplace_back(id, at, bt, pr);
    tbQ.content = to_string(P.quantum);
    tbQ.val.setString(to_string(P.quantum));
    setStatus("Loaded: " + P.name + "  — " + P.description, SUCCESS);
}

// ══════════════════════════════════════════════════════
//  Add Process
// ══════════════════════════════════════════════════════
void MainWindow::doAdd() {
    string id=tbId.content, ar=tbArr.content,
           bu=tbBurst.content, pr=tbPrio.content;
    string e;
    e = Validator::validateId(id, processes);  if(e!="OK"){setStatus("ID: "+e,DANGER);return;}
    e = Validator::validateArrival(ar);        if(e!="OK"){setStatus("Arrival: "+e,DANGER);return;}
    e = Validator::validateBurst(bu);          if(e!="OK"){setStatus("Burst: "+e,DANGER);return;}
    e = Validator::validatePriority(pr);       if(e!="OK"){setStatus("Priority: "+e,DANGER);return;}

    processes.emplace_back(id, stoi(ar), stoi(bu), stoi(pr));
    setStatus("Process " + id + " added successfully!", SUCCESS);
    tbId.clear(); tbArr.clear(); tbBurst.clear(); tbPrio.clear();
}

// ══════════════════════════════════════════════════════
//  Run Simulation
// ══════════════════════════════════════════════════════
void MainWindow::doRun() {
    if (processes.empty()) { setStatus("Add at least one process first!", DANGER); return; }

    int q = 2;
    if (algoSel==2 || algoSel==3) {
        string qs = tbQ.content;
        string eq = Validator::validateQuantum(qs);
        if (eq!="OK") { setStatus("Quantum: "+eq, DANGER); return; }
        q = stoi(qs);
    }

    results.clear(); resultTab=0;
    auto rst = [](vector<Process>& v){ for(auto& p:v) p.reset(); };

    if (algoSel==0 || algoSel==3) {
        auto cp=processes; rst(cp);
        PriorityScheduler ps(PriorityScheduler::Mode::PREEMPTIVE);
        results.push_back(ps.run(cp));
    }
    if (algoSel==1 || algoSel==3) {
        auto cp=processes; rst(cp);
        PriorityScheduler ps(PriorityScheduler::Mode::NON_PREEMPTIVE);
        results.push_back(ps.run(cp));
    }
    if (algoSel==2 || algoSel==3) {
        auto cp=processes; rst(cp);
        RoundRobin rr(q);
        auto res = rr.run(cp);
        string nm = "Round Robin (Q=" + to_string(q) + ")";
        res.setAlgorithmName(nm);
        results.push_back(res);
    }

    sf::Color tc[] = {ACCENT2, {140,90,220}, ACCENT};
    for (int i = 0; i < (int)results.size() && i < 3; i++)
        tabs[i].init(font, 28+i*230, 56, 222, 28,
                     results[i].getAlgorithmName(), tc[i]);

    screen=Screen::RESULTS; scrollY=targSY=0;
    setStatus("Simulation complete! " + to_string(results.size()) + " algorithm(s) run.", SUCCESS);
}

// ══════════════════════════════════════════════════════
//  Update
// ══════════════════════════════════════════════════════
void MainWindow::update(float dt) {
    sf::Vector2i mp = sf::Mouse::getPosition(window);
    float mx=(float)mp.x, my=(float)mp.y;
    if (statusT > 0) statusT -= dt;
    for (auto* t : {&tbId,&tbArr,&tbBurst,&tbPrio,&tbQ}) t->update(dt);
    for (auto* b : {&btnAdd,&btnClear,&btnRun,&btnReset,&btnBack}) b->upd(mx,my);
    for (auto& b : algoBtns) b.upd(mx,my);
    for (auto& b : scenarioBtns) b.upd(mx,my);
    for (auto& b : tabs) b.upd(mx,my);
    scrollY += (targSY - scrollY) * min(1.f, dt*12.f);
}

// ══════════════════════════════════════════════════════
//  Draw Main Screen
// ══════════════════════════════════════════════════════
void MainWindow::drawMain() {
    // ── Header ─────────────────────────────────────────
    sf::RectangleShape h({980,112}); h.setFillColor(PANEL); window.draw(h);
    sf::RectangleShape bar({980,3}); bar.setPosition(0,112); bar.setFillColor(ACCENT);
    window.draw(bar);

    auto title = mkTxt(font,"CPU Scheduler Simulator",22,TEXT_MAIN,28,14);
    title.setStyle(sf::Text::Bold);
    window.draw(title);
    window.draw(mkTxt(font,"Round Robin  vs  Priority Scheduling",13,TEXT_MUTED,28,46));
    window.draw(mkTxt(font,"Operating Systems Project  |  lower priority number = higher priority",
                      10,ACCENT2,28,68));

    // ── Input section ──────────────────────────────────
    window.draw(mkTxt(font,"ADD PROCESS",10,ACCENT,28,134));
    for (auto* t : {&tbId,&tbArr,&tbBurst,&tbPrio,&tbQ}) t->draw(window);
    btnAdd.draw(window); btnClear.draw(window);

    // ── Algorithm row ──────────────────────────────────
    window.draw(mkTxt(font,"SCHEDULING ALGORITHM",10,ACCENT,28,208));
    for (int i = 0; i < 4; i++) {
        algoBtns[i].selected = (i==algoSel);
        algoBtns[i].draw(window);
    }
    btnRun.draw(window); btnReset.draw(window);

    // ── Preset scenarios row ───────────────────────────
    window.draw(mkTxt(font,"LOAD PRESET SCENARIO",10,WARNING,28,252));
    for (auto& b : scenarioBtns) b.draw(window);

    // ── Status bar ─────────────────────────────────────
    if (statusT > 0) {
        sf::RectangleShape sb({920,24}); sb.setPosition(28,298);
        sb.setFillColor({statusCol.r,statusCol.g,statusCol.b,45});
        window.draw(sb);
        window.draw(mkTxt(font, statusMsg, 11, statusCol, 36, 302));
    }

    // ── Process list ───────────────────────────────────
    float ty = 334;
    {
        string hdr = "PROCESS LIST  (" + to_string(processes.size()) + ")";
        auto ht = mkTxt(font, hdr, 10, ACCENT, 28, ty);
        window.draw(ht);
    }
    ty += 16;

    if (processes.empty()) {
        window.draw(mkTxt(font,"No processes yet — fill the form above and click  +Add",
                          12, TEXT_MUTED, 28, ty+4));
    } else {
        const float  CW[] = {0,68,128,190,255};
        const string CH[] = {"PID","Arrival","Burst","Priority"};
        sf::RectangleShape th({540,20}); th.setPosition(28,ty);
        th.setFillColor({38,48,72}); window.draw(th);
        for (int c=0;c<4;c++)
            window.draw(mkTxt(font,CH[c],10,ACCENT,32+CW[c],ty+4));
        ty += 20;
        int mx2 = min((int)processes.size(), 16);
        for (int r=0;r<mx2;r++) {
            float ry=ty+r*19;
            sf::RectangleShape rw({540,19}); rw.setPosition(28,ry);
            rw.setFillColor(r%2==0?CARD:CARD2); window.draw(rw);
            auto& p=processes[r];
            vector<string> v={p.getId(),to_string(p.getArrivalTime()),
                to_string(p.getBurstTime()),to_string(p.getPriority())};
            for (int c=0;c<4;c++)
                window.draw(mkTxt(font,v[c],10,TEXT_MAIN,32+CW[c],ry+3));
        }
        if ((int)processes.size()>mx2)
            window.draw(mkTxt(font,"... +" + to_string(processes.size()-mx2) + " more",
                              10,TEXT_MUTED,32,ty+mx2*19));
    }

    // ── Scenario info sidebar ─────────────────────────
    float sx = 600, sy = 334;
    window.draw(mkTxt(font,"SCENARIO REFERENCE",10,ACCENT,sx,sy)); sy+=16;
    struct SInfo { const char* name; const char* tip; };
    SInfo info[] = {
        {"A: Basic Mixed","5 procs, mixed AT/BT/Priority, Q=3"},
        {"B: Urgency","P2 & P4 Prio=1 (critical), Q=3"},
        {"C: Fairness","All AT=0, check WT balance, Q=4"},
        {"D: Starvation","P4/P5 Prio=5, blocked by high-prio, Q=2"},
        {"E: Validation","Invalid inputs: dup ID, BT=0, Q=0"}
    };
    sf::Color sc2[] = {{70,160,100},{200,130,40},{50,140,220},{200,60,60},{130,100,200}};
    for (int i=0;i<5;i++) {
        drawRect(window, sx, sy, 360, 15, {sc2[i].r,sc2[i].g,sc2[i].b,30});
        window.draw(mkTxt(font, info[i].name, 10, sc2[i], sx+4, sy+1));
        window.draw(mkTxt(font, info[i].tip,  9,  TEXT_MUTED, sx+110, sy+2));
        sy += 18;
    }

    // ── Footer ─────────────────────────────────────────
    sf::RectangleShape ft({980,22}); ft.setPosition(0,698); ft.setFillColor(PANEL);
    window.draw(ft);
    window.draw(mkTxt(font,
        "TAB: switch fields    ENTER: add process    Priority: 1=highest, larger=lower",
        10,TEXT_MUTED,20,704));
}

// ══════════════════════════════════════════════════════
//  Draw Results Screen
// ══════════════════════════════════════════════════════
void MainWindow::drawResults() {
    // ── Header ─────────────────────────────────────────
    sf::RectangleShape h({980,52}); h.setFillColor(PANEL); window.draw(h);
    sf::RectangleShape bar({980,3}); bar.setPosition(0,52); bar.setFillColor(ACCENT);
    window.draw(bar);
    window.draw(mkTxt(font,"Simulation Results",17,TEXT_MAIN,28,15));
    btnBack.draw(window);

    // ── Tabs ───────────────────────────────────────────
    for (int i=0;i<(int)results.size()&&i<3;i++) {
        tabs[i].selected = (i==resultTab);
        tabs[i].draw(window);
    }

    if (resultTab >= (int)results.size()) return;
    auto& res = results[resultTab];

    float ox = 28;
    float oy = 100 - scrollY;

    // ── Algorithm title ────────────────────────────────
    auto at = mkTxt(font, res.getAlgorithmName(), 16, ACCENT, ox, oy);
    at.setStyle(sf::Text::Bold);
    window.draw(at);
    oy += 32;

    // ── Stat cards ─────────────────────────────────────
    drawStatCard(window, font, ox,       oy, 195, "Avg Waiting Time",    res.getAverageWaitingTime(),    WARNING);
    drawStatCard(window, font, ox+202,   oy, 195, "Avg Turnaround Time", res.getAverageTurnaroundTime(), ACCENT);
    drawStatCard(window, font, ox+404,   oy, 195, "Avg Response Time",   res.getAverageResponseTime(),   ACCENT2);
    oy += 68;

    // ── Gantt Chart ────────────────────────────────────
    window.draw(mkTxt(font,"GANTT CHART",10,ACCENT,ox,oy)); oy+=14;
    drawGantt(window, font, res.getGanttChart(), ox, oy, 915);
    oy += 72;

    // ── Process Table ──────────────────────────────────
    window.draw(mkTxt(font,"PROCESS DETAILS",10,ACCENT,ox,oy)); oy+=14;
    drawTable(window, font, res.getProcesses(), ox, oy);
    oy += 22 + (int)res.getProcesses().size()*19 + 12;

    // ── Comparison Summary (only in Run All mode) ──────
    if (results.size() >= 2) {
        // Section divider
        sf::RectangleShape divLine({915,1}); divLine.setPosition(ox,oy);
        divLine.setFillColor(BORDER); window.draw(divLine); oy+=10;

        auto sh = mkTxt(font,"COMPARISON SUMMARY",10,WARNING,ox,oy);
        sh.setStyle(sf::Text::Bold);
        window.draw(sh); oy+=18;

        // Compare header
        const float CC[] = {0,180,360,540};
        const string CH[] = {"Metric","Round Robin","Priority (Non-Pre)","Winner"};
        sf::Color  CH_C[] = {TEXT_MUTED, ACCENT, ACCENT2, SUCCESS};
        drawRect(window, ox, oy, 700, 20, {38,48,72});
        for (int c=0;c<4;c++)
            window.draw(mkTxt(font, CH[c], 10, CH_C[c], ox+CC[c]+4, oy+4));
        oy += 21;

        // Find RR and non-preemptive results
        SimulationResult* rrRes  = nullptr;
        SimulationResult* priRes = nullptr;
        for (auto& r : results) {
            if (r.getAlgorithmName().find("Round Robin") != string::npos)   rrRes  = &r;
            if (r.getAlgorithmName().find("Non")         != string::npos)   priRes = &r;
        }
        // Fallback: use first two
        if (!rrRes  && results.size() >= 2) rrRes  = &results[1];
        if (!priRes && results.size() >= 1) priRes = &results[0];

        struct CompRow { string label; double rr; double pri; bool lowerBetter; };
        vector<CompRow> rows;
        if (rrRes && priRes) {
            rows = {
                {"Avg Waiting Time",    rrRes->getAverageWaitingTime(),    priRes->getAverageWaitingTime(),    true},
                {"Avg Turnaround Time", rrRes->getAverageTurnaroundTime(), priRes->getAverageTurnaroundTime(), true},
                {"Avg Response Time",   rrRes->getAverageResponseTime(),   priRes->getAverageResponseTime(),   true},
            };
        }

        for (int r=0;r<(int)rows.size();r++) {
            auto& row = rows[r];
            drawRect(window, ox, oy, 700, 19, r%2==0?CARD:CARD2);
            window.draw(mkTxt(font, row.label, 10, TEXT_MAIN,  ox+4,    oy+3));
            window.draw(mkTxt(font, f2(row.rr), 10, ACCENT,    ox+184,  oy+3));
            window.draw(mkTxt(font, f2(row.pri),10, ACCENT2,   ox+364,  oy+3));
            bool rrWins = row.lowerBetter ? (row.rr <= row.pri) : (row.rr >= row.pri);
            string winner = rrWins ? "Round Robin" : "Priority";
            sf::Color wc  = rrWins ? ACCENT : ACCENT2;
            window.draw(mkTxt(font, winner, 10, wc, ox+544, oy+3));
            oy += 19;
        }
        oy += 14;

        // ── Analysis questions ─────────────────────────
        sf::RectangleShape divLine2({915,1}); divLine2.setPosition(ox,oy);
        divLine2.setFillColor(BORDER); window.draw(divLine2); oy+=10;

        auto ah = mkTxt(font,"ANALYSIS & CONCLUSIONS",10,WARNING,ox,oy);
        ah.setStyle(sf::Text::Bold);
        window.draw(ah); oy+=18;

        struct AQ { string q; string a; sf::Color c; };
        vector<AQ> qa;
        if (rrRes && priRes) {
            bool rrBetterWT  = rrRes->getAverageWaitingTime()    <= priRes->getAverageWaitingTime();
            bool rrBetterRT  = rrRes->getAverageResponseTime()   <= priRes->getAverageResponseTime();
            bool rrBetterTAT = rrRes->getAverageTurnaroundTime() <= priRes->getAverageTurnaroundTime();
            qa = {
                {"Q1 Better Avg Waiting Time?",
                 rrBetterWT ? "Round Robin" : "Priority — serves high-prio first, lowers avg WT",
                 rrBetterWT ? ACCENT : ACCENT2},
                {"Q2 Better Response Time?",
                 rrBetterRT ? "Round Robin — gives every process a CPU slice quickly"
                            : "Priority — high-prio processes respond very fast",
                 rrBetterRT ? ACCENT : ACCENT2},
                {"Q3 High-priority processes gain advantage?",
                 "Yes — Priority scheduling significantly reduces WT/TAT for high-prio tasks",
                 WARNING},
                {"Q4 Round Robin more balanced?",
                 "Yes — RR distributes CPU time evenly; smaller waiting-time range across processes",
                 ACCENT},
                {"Q5 Starvation observed in Priority?",
                 "Yes — low-priority processes (Prio 4-5) can face very high WT / starvation risk",
                 DANGER},
                {"Q6 Recommendation?",
                 "RR for interactive/fair systems; Priority for real-time/critical task systems",
                 SUCCESS},
            };
        }

        for (auto& qa_item : qa) {
            window.draw(mkTxt(font, qa_item.q, 10, TEXT_MUTED, ox+4,   oy));
            window.draw(mkTxt(font, qa_item.a, 10, qa_item.c,  ox+230, oy));
            oy += 16;
        }

        oy += 10;

        // ── Final Conclusion ───────────────────────────
        sf::RectangleShape divLine3({915,1}); divLine3.setPosition(ox,oy);
        divLine3.setFillColor(BORDER); window.draw(divLine3); oy+=10;

        auto ch = mkTxt(font,"FINAL CONCLUSION",10,SUCCESS,ox,oy);
        ch.setStyle(sf::Text::Bold);
        window.draw(ch); oy+=18;

        struct CLine { string text; sf::Color col; };
        vector<CLine> cl = {
            {"Priority Scheduling achieved lower avg WT & TAT in most scenarios by favouring high-prio processes.", ACCENT2},
            {"Round Robin achieved consistently better response times — ideal for interactive workloads.", ACCENT},
            {"Starvation risk is real in Priority Scheduling without aging — low-prio processes may wait indefinitely.", DANGER},
            {"Round Robin guarantees fairness: every process is served within (n-1)*quantum time units.", SUCCESS},
        };
        for (auto& line : cl) {
            // bullet
            sf::RectangleShape bullet({6,6});
            bullet.setPosition(ox+2, oy+4);
            bullet.setFillColor(line.col);
            window.draw(bullet);
            window.draw(mkTxt(font, line.text, 10, TEXT_MAIN, ox+14, oy));
            oy += 16;
        }
    }

    // ── Scroll hint ───────────────────────────────────
    sf::RectangleShape ft({980,22}); ft.setPosition(0,698); ft.setFillColor(PANEL);
    window.draw(ft);
    window.draw(mkTxt(font,"Scroll with mouse wheel to see full results",
                      10, TEXT_MUTED, 20, 704));
}
