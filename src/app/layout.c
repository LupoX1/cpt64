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

bool txt_addrEditMode = false;
char txt_addrText[128] = "";

bool vic_reg_edit[47];

uint16_t mem_base_addr = 0x0000;

void render_vic_registers(emu_t *emu)
{
    GuiGroupBox((Rectangle){anchor04.x + 0, anchor04.y + 0, 152, 592}, "Vic Registers");

    uint8_t reg = 23;
    GuiLabel((Rectangle){anchor04.x + 8, anchor04.y + 8 + (24 * reg), 32, 24}, TextFormat("%02X", reg));
    if (GuiTextBox((Rectangle){anchor04.x + 40, anchor04.y + 8 + (24 * reg), 32, 24}, TextFormat("%02X", emu_read_vic(emu, reg)), 128, vic_reg_edit[reg]))
        vic_reg_edit[reg] = !vic_reg_edit[reg];

    for (int16_t i = 0; i < 23; i++)
    {
        reg = i;
        GuiLabel((Rectangle){anchor04.x + 8, anchor04.y + 8 + (24 * i), 32, 24}, TextFormat("%02X", reg));
        if (GuiTextBox((Rectangle){anchor04.x + 40, anchor04.y + 8 + (24 * i), 32, 24}, TextFormat("%02X", emu_read_vic(emu, reg)), 128, vic_reg_edit[reg]), 128, vic_reg_edit[reg])
            vic_reg_edit[reg] = !vic_reg_edit[reg];

        reg = 24 + i;
        GuiLabel((Rectangle){anchor04.x + 80, anchor04.y + 8 + (24 * i), 32, 24}, TextFormat("%02X", reg));
        if (GuiTextBox((Rectangle){anchor04.x + 112, anchor04.y + 8 + (24 * i), 32, 24}, TextFormat("%02X", emu_read_vic(emu, reg)), 128, vic_reg_edit[reg]), 128, vic_reg_edit[reg])
            vic_reg_edit[reg] = !vic_reg_edit[reg];
    }
}

void render_cpu_registers(emu_t *emu)
{
    char flags[9];
    emu_get_flags(emu, flags);

    char irq[12];
    emu_get_interrupt(emu, irq);

    GuiGroupBox((Rectangle){anchor01.x + 0, anchor01.y + 0, 352, 112}, "Cpu Status");
    GuiLabel((Rectangle){anchor01.x + 8, anchor01.y + 8, 48, 24}, "PC");
    GuiLabel((Rectangle){anchor01.x + 56, anchor01.y + 8, 48, 24}, "SP");
    GuiLabel((Rectangle){anchor01.x + 104, anchor01.y + 8, 48, 24}, "SR");
    GuiLabel((Rectangle){anchor01.x + 152, anchor01.y + 8, 96, 24}, "Flags");
    GuiLabel((Rectangle){anchor01.x + 248, anchor01.y + 8, 96, 24}, "Cycle");
    if (GuiTextBox((Rectangle){anchor01.x + 8, anchor01.y + 32, 48, 24}, TextFormat("%04X", emu_get_pc(emu)), 128, txt_pcEditMode))
        txt_pcEditMode = !txt_pcEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 56, anchor01.y + 32, 48, 24}, TextFormat("%02X", emu_get_sp(emu)), 128, txt_spEditMode))
        txt_spEditMode = !txt_spEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 104, anchor01.y + 32, 48, 24}, TextFormat("%02X", emu_get_status(emu)), 128, txt_srEditMode))
        txt_srEditMode = !txt_srEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 152, anchor01.y + 32, 96, 24}, flags, 128, txt_flagsEditMode))
        txt_flagsEditMode = !txt_flagsEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 248, anchor01.y + 32, 96, 24}, TextFormat("%lu", emu_get_cycle(emu)), 128, txt_cycleEditMode))
        txt_cycleEditMode = !txt_cycleEditMode;
    GuiLabel((Rectangle){anchor01.x + 8, anchor01.y + 56, 48, 24}, "A");
    GuiLabel((Rectangle){anchor01.x + 56, anchor01.y + 56, 48, 24}, "X");
    GuiLabel((Rectangle){anchor01.x + 104, anchor01.y + 56, 48, 24}, "Y");
    if (GuiTextBox((Rectangle){anchor01.x + 8, anchor01.y + 80, 48, 24}, TextFormat("%02X", emu_get_a(emu)), 128, txt_aregEditMode))
        txt_aregEditMode = !txt_aregEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 56, anchor01.y + 80, 48, 24}, TextFormat("%02X", emu_get_x(emu)), 128, txt_xregEditMode))
        txt_xregEditMode = !txt_xregEditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 104, anchor01.y + 80, 48, 24}, TextFormat("%02X", emu_get_y(emu)), 128, txt_yregEditMode))
        txt_yregEditMode = !txt_yregEditMode;
    GuiLabel((Rectangle){anchor01.x + 152, anchor01.y + 56, 96, 24}, "Interrupt");
    if (GuiTextBox((Rectangle){anchor01.x + 152, anchor01.y + 80, 96, 24}, irq, 128, txt_interruptEditMode))
        txt_interruptEditMode = !txt_interruptEditMode;
    GuiLabel((Rectangle){anchor01.x + 248, anchor01.y + 56, 48, 24}, "0000");
    GuiLabel((Rectangle){anchor01.x + 296, anchor01.y + 56, 48, 24}, "0001");
    if (GuiTextBox((Rectangle){anchor01.x + 248, anchor01.y + 80, 48, 24}, TextFormat("%02X", emu_read_memory(emu, 0x0000)), 128, txt_port0EditMode))
        txt_port0EditMode = !txt_port0EditMode;
    if (GuiTextBox((Rectangle){anchor01.x + 296, anchor01.y + 80, 48, 24}, TextFormat("%02X", emu_read_memory(emu, 0x0001)), 128, txt_port1EditMode))
        txt_port1EditMode = !txt_port1EditMode;
}

void render_controls(emu_t *emu)
{
    bool is_running = emu_is_running(emu);
    bool is_paused = emu_is_paused(emu);

    GuiGroupBox((Rectangle){anchor02.x + 0, anchor02.y + 8, 232, 40}, "Controls");
    GuiSetState(!is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){anchor02.x + 8, anchor02.y + 16, 48, 24}, GuiIconText(ICON_PLAYER_PLAY, NULL)))
        emu_start(emu);
    GuiSetState(is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){anchor02.x + 64, anchor02.y + 16, 48, 24}, GuiIconText(ICON_PLAYER_STOP, NULL)))
        emu_stop(emu);
    GuiSetState(is_running ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){anchor02.x + 120, anchor02.y + 16, 48, 24}, GuiIconText(is_paused ? ICON_PLAYER_PLAY : ICON_PLAYER_PAUSE, NULL)))
        is_paused ? emu_resume(emu) : emu_pause(emu);
    GuiSetState(is_running & is_paused ? STATE_NORMAL : STATE_DISABLED);
    if (GuiButton((Rectangle){anchor02.x + 176, anchor02.y + 16, 48, 24}, GuiIconText(ICON_PLAYER_NEXT, NULL)))
        emu_step(emu);
    GuiSetState(STATE_NORMAL);
}

void render_status_bar(emu_t *emu)
{
    const char *status_text = emu_is_running(emu) ? (emu_is_paused(emu) ? "PAUSED" : "RUNNING") : "STOPPED";
    GuiStatusBar((Rectangle){8, 800, 1280, 24}, TextFormat("Status: %s | FPS: %d", status_text, GetFPS()));
}

void render_files()
{
    GuiGroupBox((Rectangle){anchor03.x + 0, anchor03.y + 8, 152, 168}, "Files");
    GuiButton((Rectangle){anchor03.x + 8, anchor03.y + 144, 64, 24}, "Save State");
    GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 144, 64, 24}, "Load State");
    GuiLabel((Rectangle){anchor03.x + 8, anchor03.y + 16, 64, 24}, "Basic ROM");
    GuiLabel((Rectangle){anchor03.x + 8, anchor03.y + 48, 64, 24}, "Kernal ROM");
    GuiLabel((Rectangle){anchor03.x + 8, anchor03.y + 80, 64, 24}, "Chars ROM");
    GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 16, 64, 24}, "Load");
    GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 48, 64, 24}, "Load");
    GuiButton((Rectangle){anchor03.x + 80, anchor03.y + 80, 64, 24}, "Load");
    GuiButton((Rectangle){anchor03.x + 8, anchor03.y + 112, 136, 24}, "Load Program .prg");
}

void render_screen(emu_t *emu, Texture2D *tex)
{
    GuiGroupBox((Rectangle){anchor06.x + 0, anchor06.y + 0, 832, 592}, "Screen");
    GuiPanel((Rectangle){anchor06.x + 96, anchor06.y + 96, 640, 400}, NULL);

    uint32_t *framebuffer = emu_get_framebuffer(emu);
    UpdateTexture(*tex, framebuffer);
    DrawTexturePro(*tex,
                   (Rectangle){0, 0, 320, 200},
                   (Rectangle){anchor06.x + 96, anchor06.y + 96, 640, 400},
                   (Vector2){0, 0}, 0.0f, WHITE);
}

void render_memory_status_old()
{
    GuiGroupBox((Rectangle){anchor05.x + 0, anchor05.y + 0, 280, 776}, "Memory");
    GuiLabel((Rectangle){anchor05.x + 8, anchor05.y + 8, 48, 24}, "Address");
    if (GuiTextBox((Rectangle){anchor05.x + 56, anchor05.y + 8, 48, 24}, txt_addrText, 128, txt_addrEditMode))
        txt_addrEditMode = !txt_addrEditMode;
    GuiButton((Rectangle){anchor05.x + 112, anchor05.y + 8, 48, 24}, "Prev");
    GuiButton((Rectangle){anchor05.x + 168, anchor05.y + 8, 48, 24}, "Next");
    GuiPanel((Rectangle){anchor05.x + 8, anchor05.y + 40, 264, 728}, NULL);
}

void render_memory_status(emu_t *emu)
{
    const int bytes_per_row = 16;
    const int rows_per_page = 32;

    GuiGroupBox((Rectangle){anchor05.x + 0, anchor05.y + 0, 280, 776}, "Memory");
    GuiLabel((Rectangle){anchor05.x + 8, anchor05.y + 8, 48, 24}, "Address");

    if (GuiTextBox((Rectangle){anchor05.x + 56, anchor05.y + 8, 48, 24}, txt_addrText, 128, txt_addrEditMode))
        txt_addrEditMode = !txt_addrEditMode;

    if (GuiButton((Rectangle){anchor05.x + 112, anchor05.y + 8, 48, 24}, "Prev"))
        mem_base_addr -= bytes_per_row * rows_per_page;

    if (GuiButton((Rectangle){anchor05.x + 168, anchor05.y + 8, 48, 24}, "Next"))
        mem_base_addr += bytes_per_row * rows_per_page;

    Rectangle panel = { anchor05.x + 8, anchor05.y + 40, 264, 728 };
    GuiPanel(panel, NULL);

    // === Dump inside the panel ===
    int start_x = panel.x + 8;
    int start_y = panel.y + 8;
    int line_height = 20;

    char linebuf[128];
    for (int row = 0; row < rows_per_page; row++) {
        uint16_t addr = mem_base_addr + row * bytes_per_row;
        snprintf(linebuf, sizeof(linebuf), "%04X: ", addr);

        // Hex dump
        int len = strlen(linebuf);
        for (int i = 0; i < bytes_per_row; i++) {
            uint8_t val = emu_read_memory(emu, addr + i);
            len += snprintf(linebuf + len, sizeof(linebuf) - len, "%02X ", val);
        }

        DrawText(linebuf, start_x, start_y + row * line_height, 10, DARKGRAY);
    }
}

void render(emu_t *emu, Texture2D *tex)
{
    BeginDrawing();

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    render_files();

    render_controls(emu);
    render_screen(emu, tex);
    render_cpu_registers(emu);
    render_vic_registers(emu);
    render_memory_status(emu);
    render_status_bar(emu);

    EndDrawing();
}