#pragma once

extern int printf(const char*, ...);
extern int puts(const char*);

enum ColorAttribute
{
    caNORMAL    = 0,
    caBOLD      = 1,
    caUNDERLINE = 4,
    caBLINKING  = 5,
    caREVERSED  = 7,
    caCONCEALED = 8
};

enum ForegroundColor
{
    fgBLACK       = 30,
    fgRED         = 31,
    fgGREEN       = 32,
    fgORANGE      = 33,
    fgBLUE        = 34,
    fgPURPLE      = 35,
    fgCYAN        = 36,
    fgGREY        = 37,
    fgGRAY        = 37,
    fgDARKGREY    = 90,
    fgDARKGRAY    = 90,
    fgLIGHTRED    = 91,
    fgLIGHTGREEN  = 92,
    fgYELLOW      = 93,
    fgLIGHTBLUE   = 94,
    fgLIGHTPURPLE = 95,
    fgTURQUOISE   = 96
};

enum BackgroundColor
{
    bgBLACK       =  40,
    bgRED         =  41,
    bgGREEN       =  42,
    bgORANGE      =  43,
    bgBLUE        =  44,
    bgPURPLE      =  45,
    bgCYAN        =  46,
    bgGREY        =  47,
    bgGRAY        =  47,
    bgDARKGREY    = 100,
    bgDARKGRAY    = 100,
    bgLIGHTRED    = 101,
    bgLIGHTGREEN  = 102,
    bgYELLOW      = 103,
    bgLIGHTBLUE   = 104,
    bgLIGHTPURPLE = 105,
    bgTURQUOISE   = 106
};

static inline void SetColors(int foreground, int background, int attribute)
{
    printf("\033[%i;%i;%im", attribute, foreground, background);
}

static inline void SetColorsDefBackground(int foreground, int attribute)
{
    printf("\033[%i;%im", attribute, foreground);
}

static inline void ResetColors(void)
{
    printf("\033[0m");
}

static inline void Success()
{
    SetColorsDefBackground(fgGREEN, caBOLD);
}

static inline void Info()
{
    SetColorsDefBackground(fgLIGHTBLUE, caBOLD);
}

static inline void Error()
{
    SetColorsDefBackground(fgRED, caBOLD);
}

static inline void Warning()
{
    SetColorsDefBackground(fgORANGE, caBOLD);
}

static inline void PrintInfo(const char* str)
{
    Info();
    puts(str);
    ResetColors();
}

static inline void PrintSuccess(const char* str)
{
    Success();
    puts(str);
    ResetColors();
}
