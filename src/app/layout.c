#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "core/emu.h"
#include "log/log.h"

typedef struct
{
    int toolbar_height;
    int sidebar_width;
    int c64_screen_width;
    int c64_screen_height;
    int status_height;
    bool show_memory_viewer;
    bool show_register_viewer;
    bool show_disassembly;
    int memory_scroll;
    int memory_address;
    char rom_path[256];
    char program_path[256];
    char state_path[256];
    Texture2D *tex;
} ui_state_t;

void render_toolbar(ui_state_t *ui, emu_t *emu)
{
    bool is_running = emu_is_running(emu);
    bool is_paused = emu_is_paused(emu);

    // Bottoni controllo emulazione
    int btn_x = ui->sidebar_width + 5;
    int btn_y = 0;
    int btn_w = 80;
    int btn_h = 30;
    int btn_gap = 0;

    GuiSetState(!is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(ICON_PLAYER_PLAY, "Start")))
        emu_start(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(is_paused ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE, is_paused ? "Resume" : "Pause")))
        is_paused ? emu_resume(emu) : emu_pause(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(ICON_PLAYER_STOP, "Stop")))
        emu_stop(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(is_running & is_paused ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){btn_x, btn_y, btn_w, btn_h}, GuiIconText(ICON_PLAYER_NEXT, "Step")))
        emu_step(emu);
    btn_x += btn_w + btn_gap;

    GuiSetState(STATE_NORMAL);
}

void render_sidebar(ui_state_t *ui, emu_t *emu)
{
    // ========================================
    // SIDEBAR SINISTRA - Caricamento files
    // ========================================
    int sidebar_x = 0;
    int sidebar_y = 0;
    GuiPanel((Rectangle){sidebar_x, sidebar_y, ui->sidebar_width, GetScreenHeight()}, "Files");

    int sy = sidebar_y + 40;
    int sx = 10;
    int sw = ui->sidebar_width - 20;

    // ROM Files
    GuiGroupBox((Rectangle){sx, sy, sw, 105}, "ROM Files");
    sy += 10;

    GuiLabel((Rectangle){sx + 10, sy, 100, 20}, "BASIC ROM:");
    if (GuiButton((Rectangle){sx + 110, sy, sw - 120, 25}, "Load BASIC"))
    {
        // TODO: File dialog
        emu_load_rom(emu, "basic", "roms/basic.rom");
    }
    sy += 30;

    GuiLabel((Rectangle){sx + 10, sy, 100, 20}, "KERNAL ROM:");
    if (GuiButton((Rectangle){sx + 110, sy, sw - 120, 25}, "Load KERNAL"))
    {
        emu_load_rom(emu, "kernal", "roms/kernal.rom");
    }
    sy += 30;

    GuiLabel((Rectangle){sx + 10, sy, 100, 20}, "CHARSET ROM:");
    if (GuiButton((Rectangle){sx + 110, sy, sw - 120, 25}, "Load CHARSET"))
    {
        emu_load_rom(emu, "charset", "roms/charset.rom");
    }
    sy = 165;

    // Program
    GuiGroupBox((Rectangle){sx, sy, sw, 50}, "Program");
    sy += 10;

    if (GuiButton((Rectangle){sx + 10, sy, sw - 20, 30}, "#5#Load Program (.PRG)"))
    {
        // TODO: File dialog
        emu_load_program(emu, "program.prg", 0x0801);
    }
    sy = 235;

    // State
    GuiGroupBox((Rectangle){sx, sy, sw, 60}, "Save State");
    sy += 10;

    if (GuiButton((Rectangle){sx + 10, sy, (sw - 30) / 2, 30}, "#2#Save State"))
    {
        // TODO: File dialog
        emu_save_state(emu, "state.sav");
    }
    if (GuiButton((Rectangle){sx + 15 + (sw - 30) / 2, sy, (sw - 30) / 2, 30}, "#5#Load State"))
    {
        emu_load_state(emu, "state.sav");
    }
    sy += 30;

    GuiLabel((Rectangle){sx + 10, sy, sw - 20, 20}, "Quick save: F5 | Quick load: F9");
}

void render_status(ui_state_t *ui, emu_t *emu)
{
    int screen_x = ui->sidebar_width + 20;
    int screen_y = ui->toolbar_height + 20;

    // ========================================
    // PANNELLO STATUS - Sotto lo schermo
    // ========================================
    if (!emu_is_paused(emu))
        return;

    int status_y = screen_y + ui->c64_screen_height + 50;
    GuiPanel((Rectangle){screen_x - 5, status_y, ui->c64_screen_width + 10, ui->status_height}, "Debugger");

    int info_y = status_y + 30;
    int col1_x = screen_x + 10;
    int col2_x = screen_x + 250;
    int col3_x = screen_x + 450;

    // Registri CPU
    GuiLabel((Rectangle){col1_x, info_y, 200, 20}, TextFormat("PC: $%04X", emu_get_pc(emu)));
    GuiLabel((Rectangle){col2_x, info_y, 200, 20}, TextFormat("A: $%02X", emu_get_a(emu)));
    GuiLabel((Rectangle){col3_x, info_y, 200, 20}, TextFormat("X: $%02X", emu_get_x(emu)));
    info_y += 25;

    GuiLabel((Rectangle){col1_x, info_y, 200, 20}, TextFormat("Y: $%02X", emu_get_y(emu)));
    GuiLabel((Rectangle){col2_x, info_y, 200, 20}, TextFormat("SP: $%02X", emu_get_sp(emu)));
    GuiLabel((Rectangle){col3_x, info_y, 200, 20}, TextFormat("SR: $%02X", emu_get_status(emu)));
    info_y += 25;

    // Flags
    uint8_t status = emu_get_status(emu);
    char flags[9];
    flags[0] = (status & 0x80) ? 'N' : 'n';
    flags[1] = (status & 0x40) ? 'V' : 'v';
    flags[2] = '-';
    flags[3] = (status & 0x10) ? 'B' : 'b';
    flags[4] = (status & 0x08) ? 'D' : 'd';
    flags[5] = (status & 0x04) ? 'I' : 'i';
    flags[6] = (status & 0x02) ? 'Z' : 'z';
    flags[7] = (status & 0x01) ? 'C' : 'c';
    flags[8] = '\0';
    GuiLabel((Rectangle){col1_x, info_y, 300, 20}, TextFormat("Flags: %s", flags));
    info_y += 30;

    // Istruzione corrente
    GuiLabel((Rectangle){col1_x, info_y, 600, 20}, TextFormat("Current: %s", emu_get_current_instruction(emu)));
}

void render_side_panels(ui_state_t *ui, emu_t *emu)
{
    int screen_x = ui->sidebar_width + 20;
    int screen_y = ui->toolbar_height + 20;

    // ========================================
    // PANNELLI LATERALI OPZIONALI
    // ========================================
    int side_x = screen_x + ui->c64_screen_width + 30;
    int side_w = GetScreenWidth() - side_x - 10;

    if (side_w > 200)
    {
        int panel_y = ui->toolbar_height + 20;

        // Memory Viewer
        if (ui->show_memory_viewer)
        {
            GuiPanel((Rectangle){side_x, panel_y, side_w, 250}, "Memory Viewer");

            int mem_y = panel_y + 30;
            GuiLabel((Rectangle){side_x + 10, mem_y, 100, 20}, "Address:");
            GuiTextBox((Rectangle){side_x + 80, mem_y, 100, 25}, TextFormat("%04X", ui->memory_address), 5, false);

            mem_y += 35;

            // Mostra 16 righe di memoria (16 byte per riga)
            for (int row = 0; row < 8; row++)
            {
                uint16_t addr = ui->memory_address + (row * 16);
                char line[80];
                sprintf(line, "%04X: ", addr);

                for (int col = 0; col < 16; col++)
                {
                    sprintf(line + strlen(line), "%02X ", emu_read_memory(emu, addr + col));
                }

                GuiLabel((Rectangle){side_x + 10, mem_y, side_w - 20, 20}, line);
                mem_y += 22;
            }

            panel_y += 260;
        }

        // Register Viewer (dettagliato)
        if (ui->show_register_viewer && emu_is_paused(emu))
        {
            GuiPanel((Rectangle){side_x, panel_y, side_w, 200}, "CPU Registers");
            // Già mostrato nel pannello status principale
            panel_y += 210;
        }

        // Disassembly
        if (ui->show_disassembly && emu_is_paused(emu))
        {
            GuiPanel((Rectangle){side_x, panel_y, side_w, 300}, "Disassembly");

            int dis_y = panel_y + 30;
            uint16_t pc = emu_get_pc(emu);

            // Mostra 10 istruzioni intorno al PC
            for (int i = -3; i < 7; i++)
            {
                // TODO: Implementa disassembler per ottenere istruzioni
                // const char *instr = emu_disassemble(pc + i * 3);

                Color color = (i == 0) ? RED : LIGHTGRAY;
                DrawText(TextFormat("%04X: NOP", pc + i * 3), side_x + 10, dis_y, 16, color);
                dis_y += 22;
            }
        }
    }
}

void render_footer(ui_state_t *ui, emu_t *emu)
{
    // ========================================
    // FOOTER - Info generali
    // ========================================
    const char *status_text = emu_is_running(emu) ? (emu_is_paused(emu) ? "PAUSED" : "RUNNING") : "STOPPED";
    DrawText(TextFormat("Status: %s | FPS: %d", status_text, GetFPS()), 10, GetScreenHeight() - 20, 16, LIGHTGRAY);
}

ui_state_t ui_state =
    {
        .show_memory_viewer = false,
        .show_register_viewer = false,
        .show_disassembly = false,
        .memory_scroll = 0,
        .memory_address = 0x744B,
        .rom_path = "",
        .program_path = "",
        .state_path = "",
        .toolbar_height = 50,
        .sidebar_width = 240,
        .c64_screen_width = 640,
        .c64_screen_height = 400,
        .status_height = 40};

void render_old(ui_state_t ui_state, emu_t *emu, Texture2D *tex)
{
    BeginDrawing();
    render_sidebar(&ui_state, emu);
    render_toolbar(&ui_state, emu);

    // ========================================
    // SCHERMO C64 - Centro
    // ========================================
    int screen_x = ui_state.sidebar_width + 20;
    int screen_y = ui_state.toolbar_height + 20;

    GuiPanel((Rectangle){screen_x - 5, screen_y - 5, ui_state.c64_screen_width + 10, ui_state.c64_screen_height + 35}, "C64 Display");

    // Bordo schermo
    DrawRectangle(screen_x - 2, screen_y + 25 - 2, ui_state.c64_screen_width + 4, ui_state.c64_screen_height + 4, BLUE);
    DrawRectangle(screen_x, screen_y + 25, ui_state.c64_screen_width, ui_state.c64_screen_height, BLACK);

    // Texture dello schermo C64 emu_get_framebuffer
    uint32_t *framebuffer = emu_get_framebuffer(emu);
    UpdateTexture(*tex, framebuffer);
    DrawTexturePro(*tex,
                   (Rectangle){0, 0, 320, 200},
                   (Rectangle){screen_x, screen_y + 25, ui_state.c64_screen_width, ui_state.c64_screen_height},
                   (Vector2){0, 0}, 0.0f, WHITE);
    render_status(&ui_state, emu);
    // render_side_panels(ui, emu);
    render_footer(&ui_state, emu);
    EndDrawing();
}

bool vic_reg_edit = false;
// layout_cpt64: controls initialization
//----------------------------------------------------------------------------------
Vector2 anchor01 = {168, 680};
Vector2 anchor02 = {168, 8};
Vector2 anchor03 = {8, 8};
Vector2 anchor04 = {8, 200};
Vector2 anchor05 = {1008, 16};
Vector2 anchor06 = {168, 72};

bool txt_pcEditMode = false;
char txt_pcText[128] = "";
bool txt_spEditMode = false;
char txt_spText[128] = "";
bool txt_srEditMode = false;
char txt_srText[128] = "";
bool txt_flagsEditMode = false;
char txt_flagsText[128] = "";
bool txt_cycleEditMode = false;
char txt_cycleText[128] = "";
bool txt_aregEditMode = false;
char txt_aregText[128] = "";
bool txt_xregEditMode = false;
char txt_xregText[128] = "";
bool txt_yregEditMode = false;
char txt_yregText[128] = "";
bool txt_interruptEditMode = false;
char txt_interruptText[128] = "";
bool txt_port0EditMode = false;
char txt_port0Text[128] = "";
bool txt_port1EditMode = false;
char txt_port1Text[128] = "";
bool btn_startPressed = false;
bool btn_stopPressed = false;
bool btn_pausePressed = false;
bool btn_stepPressed = false;
bool Button031Pressed = false;
bool Button032Pressed = false;
bool Button036Pressed = false;
bool Button037Pressed = false;
bool Button038Pressed = false;
bool Button039Pressed = false;
bool txt_vic_00EditMode = false;
char txt_vic_00Text[128] = "";

char txt_vic_01Text[128] = "";
bool txt_vic_18EditMode = false;
char txt_vic_18Text[128] = "";
bool txt_addrEditMode = false;
char txt_addrText[128] = "";
bool btn_prevPressed = false;
bool btn_nextPressed = false;
bool txt_vic_16EditMode = false;
char txt_vic_16Text[128] = "";
bool txt_vic_19EditMode = false;
char txt_vic_19Text[128] = "";
bool TextBox141EditMode = false;
char TextBox141Text[128] = "";
bool txt_vic_2eEditMode = false;
char txt_vic_2eText[128] = "";

void render(emu_t *emu, Texture2D *tex)
{
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // raygui: controls drawing
    //----------------------------------------------------------------------------------
    GuiGroupBox((Rectangle){anchor01.x + 0, anchor01.y + 0, 352, 112}, "Cpu Status");
    GuiLabel((Rectangle){anchor01.x + 8, anchor01.y + 8, 48, 24}, "PC");
    GuiLabel((Rectangle){anchor01.x + 56, anchor01.y + 8, 48, 24}, "SP");
    GuiLabel((Rectangle){anchor01.x + 104, anchor01.y + 8, 48, 24}, "SR");
    GuiLabel((Rectangle){anchor01.x + 152, anchor01.y + 8, 96, 24}, "Flags");
    GuiLabel((Rectangle){anchor01.x + 248, anchor01.y + 8, 96, 24}, "Cycle");
    if (GuiTextBox((Rectangle){anchor01.x + 8, anchor01.y + 32, 48, 24}, txt_pcText, 128, txt_pcEditMode))
        txt_pcEditMode = !txt_pcEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 56, anchor01.y + 32, 48, 24}, txt_spText, 128, txt_spEditMode))
        txt_spEditMode = !txt_spEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 104, anchor01.y + 32, 48, 24}, txt_srText, 128, txt_srEditMode))
        txt_srEditMode = !txt_srEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 152, anchor01.y + 32, 96, 24}, txt_flagsText, 128, txt_flagsEditMode))
        txt_flagsEditMode = !txt_flagsEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 248, anchor01.y + 32, 96, 24}, txt_cycleText, 128, txt_cycleEditMode))
        txt_cycleEditMode = !txt_cycleEditMode;
    GuiLabel((Rectangle){anchor01.x + 8, anchor01.y + 56, 48, 24}, "A");
    GuiLabel((Rectangle){anchor01.x + 56, anchor01.y + 56, 48, 24}, "X");
    GuiLabel((Rectangle){anchor01.x + 104, anchor01.y + 56, 48, 24}, "Y");
    if (GuiTextBox((Rectangle){anchor01.x + 8, anchor01.y + 80, 48, 24}, txt_aregText, 128, txt_aregEditMode))
        txt_aregEditMode = !txt_aregEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 56, anchor01.y + 80, 48, 24}, txt_xregText, 128, txt_xregEditMode))
        txt_xregEditMode = !txt_xregEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 104, anchor01.y + 80, 48, 24}, txt_yregText, 128, txt_yregEditMode))
        txt_yregEditMode = !txt_yregEditMode;
    GuiLabel((Rectangle){anchor01.x + 152, anchor01.y + 56, 96, 24}, "Interrupt");
    if (GuiTextBox((Rectangle){anchor01.x + 152, anchor01.y + 80, 96, 24}, txt_interruptText, 128, txt_interruptEditMode))
        txt_interruptEditMode = !txt_interruptEditMode;
    GuiLabel((Rectangle){anchor01.x + 248, anchor01.y + 56, 48, 24}, "0000");
    GuiLabel((Rectangle){anchor01.x + 296, anchor01.y + 56, 48, 24}, "0001");
    if (GuiTextBox((Rectangle){anchor01.x + 248, anchor01.y + 80, 48, 24}, txt_port0Text, 128, txt_port0EditMode))
        txt_port0EditMode = !txt_port0EditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 296, anchor01.y + 80, 48, 24}, txt_port1Text, 128, txt_port1EditMode))
        txt_port1EditMode = !txt_port1EditMode;
    GuiGroupBox((Rectangle){anchor02.x + 0, anchor02.y + 8, 232, 40}, "Controls");
    btn_startPressed = GuiButton((Rectangle){anchor02.x + 8, anchor02.y + 16, 48, 24}, "Start");
    btn_stopPressed = GuiButton((Rectangle){anchor02.x + 64, anchor02.y + 16, 48, 24}, "Stop");
    btn_pausePressed = GuiButton((Rectangle){anchor02.x + 120, anchor02.y + 16, 48, 24}, "Pause");
    btn_stepPressed = GuiButton((Rectangle){anchor02.x + 176, anchor02.y + 16, 48, 24}, "Step");
    GuiStatusBar((Rectangle){8, 800, 1280, 24}, "Status");
    GuiGroupBox((Rectangle){anchor03.x + 0, anchor03.y + 8, 152, 168}, "Files");
    Button031Pressed = GuiButton((Rectangle){anchor03.x + 8, anchor03.y + 144, 64, 24}, "Save State");
    Button032Pressed = GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 144, 64, 24}, "Load State");
    GuiLabel((Rectangle){anchor03.x + 8, anchor03.y + 16, 64, 24}, "Basic ROM");
    GuiLabel((Rectangle){anchor03.x + 8, anchor03.y + 48, 64, 24}, "Kernal ROM");
    GuiLabel((Rectangle){anchor03.x + 8, anchor03.y + 80, 64, 24}, "Chars ROM");
    Button036Pressed = GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 16, 64, 24}, "Load");
    Button037Pressed = GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 48, 64, 24}, "Load");
    Button038Pressed = GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 80, 64, 24}, "Load");
    Button039Pressed = GuiButton((Rectangle){anchor03.x + 8, anchor03.y + 112, 136, 24}, "Load Program .prg");
    GuiGroupBox((Rectangle){anchor04.x + 0, anchor04.y + 0, 152, 592}, "Vic Registers");
    char buf[16];
    for (int16_t i = 0; i < 24; i++)
    {
        sprintf(buf, "%02X", i);
        GuiLabel((Rectangle){anchor04.x + 8,  anchor04.y + 8 + (24 * i), 32, 24}, buf);
        sprintf(buf, "%02X", emu_read_vic(emu, i));
        if (GuiTextBox((Rectangle){anchor04.x + 40, anchor04.y + 8 + (24 * i), 32, 24}, buf, 128, vic_reg_edit))
            vic_reg_edit = !vic_reg_edit;
    }
    for (int16_t i = 0; i < 23; i++)
    {
        sprintf(buf, "%02X", 24 + i);
        GuiLabel((Rectangle){anchor04.x + 80, anchor04.y + 8 + (24 * i), 32, 24}, buf);
        sprintf(buf, "%02X", emu_read_vic(emu, i));
        if (GuiTextBox((Rectangle){anchor04.x + 112, anchor04.y + 8 + (24 * i), 32, 24}, buf, 128, vic_reg_edit))
            vic_reg_edit = !vic_reg_edit;
    }
    GuiGroupBox((Rectangle){anchor05.x + 0, anchor05.y + 0, 280, 776}, "Memory");
    GuiLabel((Rectangle){anchor05.x + 8, anchor05.y + 8, 48, 24}, "Address");
    if (GuiTextBox((Rectangle){anchor05.x + 56, anchor05.y + 8, 48, 24}, txt_addrText, 128, txt_addrEditMode))
        txt_addrEditMode = !txt_addrEditMode;
    btn_prevPressed = GuiButton((Rectangle){anchor05.x + 112, anchor05.y + 8, 48, 24}, "Prev");
    btn_nextPressed = GuiButton((Rectangle){anchor05.x + 168, anchor05.y + 8, 48, 24}, "Next");
    GuiPanel((Rectangle){anchor05.x + 8, anchor05.y + 40, 264, 728}, NULL);
    GuiGroupBox((Rectangle){anchor06.x + 0, anchor06.y + 0, 832, 592}, "Screen");
    GuiPanel((Rectangle){anchor06.x + 96, anchor06.y + 96, 640, 400}, NULL);
    //----------------------------------------------------------------------------------

    EndDrawing();
    //----------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
