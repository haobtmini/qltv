#ifndef UI_H
#define UI_H

#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <cstring>
#include "logic.h"

#define SCREEN_W 122
#define SCREEN_H 33          // <-- TANG tu 30 len 33 de chua 20 dong du lieu

#define KEY_TAB 9
#define KEY_ENTER 13
#define KEY_ESC 27
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_PGUP 73
#define KEY_PGDN 81
#define KEY_F1 59
#define KEY_F2 60
#define KEY_F3 61
#define KEY_F4 62
#define KEY_F5 63

// =========================================================================
// GHOST-STYLE MINIMALIST PALETTE
// =========================================================================
#define C_APP_BG        0x07
#define C_BRAND         0x0B
#define C_MUTED         0x08
#define C_TEXT          0x07

#define C_BOX_BORDER    0x08
#define C_TBL_HEADER    0x0B
#define C_ROW_NORM      0x07
#define C_ROW_SEL       0x30   // Nen Cyan tram, chu den
#define C_COL_SEP       0x08
#define C_COL_SEP_SEL   0x30   // <-- MOI: ke cot tren dong selected (chu den tren nen cyan)

#define C_FIELD_SEL_BG    0x20   // Chon: nen xanh la dam (2), chu den (0)
#define C_FIELD_NORM_BG   0x80   // Thuong: nen xam dam (8), chu den (0)

#define C_ROW_ALT       0x80   // Dong le: nen xam dam (8), chu den (0)

#define C_BADGE_OK      0x02
#define C_BADGE_WARN    0x06
#define C_BADGE_ERR     0x0C
#define C_BADGE_INFO    0x0B

#define C_KEYCAP        0x0B
#define C_KEYDESC       0x08

#define C_DLG_BG        0x07
#define C_DLG_BORDER    0x0B
#define C_DLG_TITLE     0x30
#define C_SHADOW        0x08
#define C_INPUT_BG      0x08
#define C_INPUT_FOCUS   0x30

#define C_CATE_HEADER     0x0B
#define C_CATE_HEADER_SEL 0x30

// <-- MOI: Mau cho cac field chip + tab box
#define C_TABBOX_BORDER   0x0B   // Vien box bao tab
#define C_TABBOX_BG       0x07
#define C_FIELD_BG        0x70   // Nen xam sang, chu den (chip thuong)
#define C_FIELD_SEL       0x30   // Nen cyan, chu den (chip dang chon)
#define C_FIELD_SORTED    0x60   // Nen nau vang, chu den (chip co sort arrow)
#define C_LABEL_FILTER    0x0E   // Vang sang cho "Truong loc"
#define C_LABEL_SEARCH    0x0E   // Vang sang cho "Tim kiem"
#define C_METRIC_DG       0x0A   // Xanh la sang cho Doc gia
#define C_METRIC_DS       0x0B   // Cyan sang cho Dau sach
#define C_METRIC_MT       0x0E   // Vang sang cho Dang muon
#define C_METRIC_OH       0x0C   // Do sang cho Qua han
#define C_METRIC_DATE     0x08   // Xam cho ngay

// =========================================================================
// UNICODE GLYPHS
// =========================================================================
#define U_TL      L'\u256D'
#define U_TR      L'\u256E'
#define U_BL      L'\u2570'
#define U_BR      L'\u256F'
#define U_H       L'\u2500'
#define U_V       L'\u2502'
#define U_VBOLD   L'\u2503'
#define U_L       L'\u251C'
#define U_R       L'\u2524'
#define U_BL_TREE L'\u2514'
#define U_TRI_R   L'\u2022'
#define U_ARR_UP  L'\u2191'
#define U_ARR_DN  L'\u2193'

class ConsoleEngine {
private:
    HANDLE hOut;
    CHAR_INFO buffer[SCREEN_W * SCREEN_H];

    wchar_t toWideChar(char c) {
        unsigned char uc = (unsigned char)c;
        switch (uc) {
            case 218: return U_TL;
            case 191: return U_TR;
            case 192: return U_BL;
            case 217: return U_BR;
            case 196: return U_H;
            case 179: return U_V;
            case 195: return U_L;
            default:  return (wchar_t)uc;
        }
    }

public:
    void init(const char* windowTitle) {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        SetConsoleTitleA(windowTitle);
        hOut = GetStdHandle(STD_OUTPUT_HANDLE);

        COORD size = { SCREEN_W, SCREEN_H };
        SMALL_RECT rect = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
        SetConsoleScreenBufferSize(hOut, size);
        SetConsoleWindowInfo(hOut, TRUE, &rect);

        CONSOLE_CURSOR_INFO cursorInfo;
        GetConsoleCursorInfo(hOut, &cursorInfo);
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hOut, &cursorInfo);
        clear();
    }

    void clear(WORD attr = C_APP_BG) {
        for (int i = 0; i < SCREEN_W * SCREEN_H; i++) {
            buffer[i].Char.UnicodeChar = L' ';
            buffer[i].Attributes = attr;
        }
    }

    void putWChar(int x, int y, wchar_t wc, WORD attr) {
        if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
            buffer[y * SCREEN_W + x].Char.UnicodeChar = wc;
            buffer[y * SCREEN_W + x].Attributes = attr;
        }
    }

    void putChar(int x, int y, char c, WORD attr) {
        putWChar(x, y, toWideChar(c), attr);
    }

    void writeStr(int x, int y, const char* str, WORD attr) {
        if (y < 0 || y >= SCREEN_H || !str) return;
        for (int i = 0; str[i] != '\0' && (x + i) < SCREEN_W; i++) {
            if (x + i >= 0) putChar(x + i, y, str[i], attr);
        }
    }

    void writeCell(int x, int y, int width, const char* str, WORD attr) {
        if (y < 0 || y >= SCREEN_H || !str) return;
        int len = (int)strlen(str);
        for (int i = 0; i < width && (x + i) < SCREEN_W; i++) {
            if (i == width - 1 && len > width) {
                putWChar(x + i, y, L'\u2026', attr);
            } else {
                char ch = (i < len) ? str[i] : ' ';
                putChar(x + i, y, ch, attr);
            }
        }
    }

    void fillRect(int x, int y, int w, int h, WORD attr) {
        for (int r = y; r < y + h && r < SCREEN_H; r++) {
            for (int c = x; c < x + w && c < SCREEN_W; c++) {
                if (r >= 0 && c >= 0) putWChar(c, r, L' ', attr);
            }
        }
    }

    void drawBox(int x, int y, int w, int h, WORD fillAttr, WORD borderAttr, const char* title = "", bool shadow = true, WORD titleAttr = C_DLG_TITLE) {
        if (shadow) {
            for (int i = 1; i <= h + 1; i++) {
                if (y + i < SCREEN_H && x + w + 1 < SCREEN_W) {
                    putWChar(x + w + 1, y + i, L'\u2591', C_SHADOW);
                    if (x + w + 2 < SCREEN_W) putWChar(x + w + 2, y + i, L'\u2591', C_SHADOW);
                }
            }
            for (int j = 2; j <= w + 2; j++) {
                if (y + h + 1 < SCREEN_H && x + j < SCREEN_W) {
                    putWChar(x + j, y + h + 1, L'\u2591', C_SHADOW);
                }
            }
        }
        for (int i = 0; i <= h; i++) {
            for (int j = 0; j <= w; j++) {
                wchar_t wc = L' ';
                if (i == 0 && j == 0) wc = U_TL;
                else if (i == 0 && j == w) wc = U_TR;
                else if (i == h && j == 0) wc = U_BL;
                else if (i == h && j == w) wc = U_BR;
                else if (i == 0 || i == h) wc = U_H;
                else if (j == 0 || j == w) wc = U_V;
                putWChar(x + j, y + i, wc, borderAttr);
                if (i > 0 && i < h && j > 0 && j < w) putWChar(x + j, y + i, L' ', fillAttr);
            }
        }
        if (title && strlen(title) > 0) {
            char buf[100];
            sprintf(buf, " %s ", title);
            writeStr(x + 2, y, buf, titleAttr);
        }
    }

    void drawTableGrid(int x, int y, int w, int h, const int colX[], int numCols, const char* headers[], int totalItems, int selectedIdx, int scrollOffset = 0, const char* sectionTitle = "") {
        for (int i = 0; i <= h; i++) {
            for (int j = 0; j <= w; j++) {
                wchar_t wc = L' ';
                if (i == 0 && j == 0) wc = U_TL;
                else if (i == 0 && j == w) wc = U_TR;
                else if (i == h && j == 0) wc = U_BL;
                else if (i == h && j == w) wc = U_BR;
                else if (i == 0 || i == h) wc = U_H;
                else if (j == 0 || j == w) wc = U_V;
                putWChar(x + j, y + i, wc, C_BOX_BORDER);
                if (i > 0 && i < h && j > 0 && j < w) putWChar(x + j, y + i, L' ', C_APP_BG);
            }
        }

        if (sectionTitle && strlen(sectionTitle) > 0) {
            char tBuf[60];
            sprintf(tBuf, " %s ", sectionTitle);
            writeStr(x + 2, y, tBuf, C_BRAND);
        }

        for (int c = 0; c < numCols; c++) {
            writeStr(colX[c] + 1, y + 1, headers[c], C_TBL_HEADER);
            if (c > 0) putWChar(colX[c] - 1, y + 1, U_V, C_COL_SEP);
        }

        putWChar(x, y + 2, U_L, C_BOX_BORDER);
        for (int j = 1; j < w; j++) putWChar(x + j, y + 2, U_H, C_BOX_BORDER);
        putWChar(x + w, y + 2, U_R, C_BOX_BORDER);

        int visibleRows = h - 3;
        if (totalItems > visibleRows && visibleRows > 0) {
            int trackH = visibleRows;
            int thumbH = (visibleRows * visibleRows) / totalItems;
            if (thumbH < 1) thumbH = 1;
            int maxScroll = totalItems - visibleRows;
            int thumbPos = (maxScroll > 0) ? (scrollOffset * (trackH - thumbH)) / maxScroll : 0;

            for (int r = 0; r < trackH; r++) {
                if (r >= thumbPos && r < thumbPos + thumbH) putWChar(x + w, y + 3 + r, U_VBOLD, C_BRAND);
                else putWChar(x + w, y + 3 + r, U_V, C_COL_SEP);
            }
        }

        char pageInfo[40];
        sprintf(pageInfo, " %d / %d ", totalItems > 0 ? selectedIdx + 1 : 0, totalItems);
        writeStr(x + w - (int)strlen(pageInfo) - 2, y + h, pageInfo, C_BRAND);
    }

    // FIX: khi selected, van ve ke cot voi mau C_COL_SEP_SEL (chu den tren nen cyan)
    WORD prepareTableRow(int x, int y, int w, int rowIdx, const int colX[], int numCols, bool isSelected) {
        WORD rowAttr = isSelected ? C_ROW_SEL : C_ROW_NORM;
        fillRect(x + 1, y, w - 1, 1, rowAttr);
        if (isSelected) {
            putWChar(x + 1, y, U_TRI_R, C_ROW_SEL);
        }
        // Luon ve ke cot (ca khi selected) -> sua loi mat ke cot
        for (int c = 1; c < numCols; c++) {
            putWChar(colX[c] - 1, y, U_V, isSelected ? C_COL_SEP_SEL : C_COL_SEP);
        }
        return rowAttr;
    }

    void render() {
        COORD bufSize = { SCREEN_W, SCREEN_H };
        COORD bufCoord = { 0, 0 };
        SMALL_RECT writeArea = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
        WriteConsoleOutputW(hOut, buffer, bufSize, bufCoord, &writeArea);
    }

    void showMessage(const char* msg, bool isError = false) {
        drawBox(30, 11, 60, 6, C_DLG_BG, isError ? C_BADGE_ERR : C_DLG_BORDER, isError ? "LOI HE THONG" : "THONG BAO", true, isError ? 0xC0 : C_DLG_TITLE);
        writeStr(33, 13, msg, isError ? C_BADGE_ERR : C_ROW_NORM);
        writeStr(42, 15, "[ ENTER / ESC de dong ]", C_MUTED);
        render();
        _getch();
    }

    bool showConfirm(const char* msg) {
        drawBox(30, 11, 60, 6, C_DLG_BG, C_DLG_BORDER, "XAC NHAN");
        writeStr(33, 13, msg, C_ROW_NORM);
        writeStr(35, 15, "[ ENTER/Y: Dong y   ESC/N: Huy ]", C_MUTED);
        render();
        while (true) {
            int ch = _getch();
            if (ch == KEY_ENTER || ch == 'y' || ch == 'Y') return true;
            if (ch == KEY_ESC || ch == 'n' || ch == 'N') return false;
        }
    }

    int showExitConfirm() {
        int boxW = 74;
        int boxH = 6;
        int startX = (SCREEN_W - boxW) / 2;
        int startY = (SCREEN_H - boxH) / 2;
    
        drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BORDER, "THOAT CHUONG TRINH");
    
        // Dong 1: "Ban co muon " (trang) + "LUU DU LIEU" (do) + " truoc khi thoat chuong trinh?" (trang)
        const char* part1 = "Ban co muon ";
        const char* part2 = "LUU DU LIEU";
        const char* part3 = " truoc khi thoat chuong trinh?";
        int y1 = startY + 2;
        int x1 = startX + 10;
    
        writeStr(x1, y1, part1, C_ROW_NORM);
        x1 += (int)strlen(part1);
        writeStr(x1, y1, part2, C_BADGE_ERR);   // do
        x1 += (int)strlen(part2);
        writeStr(x1, y1, part3, C_ROW_NORM);
    
        // Dong 2: huong dan - vang
        writeStr(startX + 13, startY + 4, "[ ENTER/Y: Luu & Thoat | N: Thoat | ESC: Huy ]", C_BADGE_WARN);
        render();
    
        while (true) {
            int ch = _getch();
            if (ch == KEY_ENTER || ch == 'y' || ch == 'Y') return 1;
            if (ch == 'n' || ch == 'N') return 0;
            if (ch == KEY_ESC) return -1;
        }
    }

    bool showFormDialog(const char* title, const char* labels[], char values[][50], const int maxLens[], int numFields, const char* readOnlyInfo = nullptr, const char* toggleOpts[] = nullptr) {
        int boxW = 68;
        int boxH = numFields * 2 + (readOnlyInfo ? 7 : 5);
        int startX = (SCREEN_W - boxW) / 2;
        int startY = (SCREEN_H - boxH) / 2;
        int curField = 0;

        while (true) {
            drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BORDER, title);
            int curY = startY + 2;

            if (readOnlyInfo) {
                writeStr(startX + 3, curY, readOnlyInfo, C_BRAND);
                curY += 2;
            }

            for (int i = 0; i < numFields; i++) {
                bool isFocus = (i == curField);
                bool isToggle = (toggleOpts && toggleOpts[i] && strlen(toggleOpts[i]) > 0);
                if (isFocus) putWChar(startX + 1, curY, U_TRI_R, C_BRAND);
                writeStr(startX + 3, curY, labels[i], isFocus ? C_BRAND : C_ROW_NORM);

                int inpX = startX + 28;
                int inpW = boxW - 31;
                WORD inpCol = isFocus ? C_INPUT_FOCUS : C_INPUT_BG;

                if (!isFocus) {
                    for (int k = 0; k < inpW; k++) putWChar(inpX + k, curY, L'.', C_MUTED);
                } else {
                    fillRect(inpX, curY, inpW, 1, inpCol);
                }

                if (isToggle) {
                    char displayVal[60];
                    if (isFocus) sprintf(displayVal, "< %s >", values[i]);
                    else sprintf(displayVal, "  %s  ", values[i]);
                    writeStr(inpX + 1, curY, displayVal, isFocus ? inpCol : C_ROW_NORM);
                } else {
                    writeStr(inpX + 1, curY, values[i], isFocus ? inpCol : C_ROW_NORM);
                    if (isFocus) {
                        int vLen = (int)strlen(values[i]);
                        if (vLen < inpW - 1) putWChar(inpX + 1 + vLen, curY, L'_', inpCol);
                    }
                }
                curY += 2;
            }

            bool hasToggle = false;
            if (toggleOpts) {
                for (int i = 0; i < numFields; i++) if (toggleOpts[i] && strlen(toggleOpts[i]) > 0) hasToggle = true;
            }
            if (hasToggle) writeStr(startX + 3, startY + boxH - 2, "Up/Down: Chon  |  <-/->: Doi gia tri  |  Enter: Luu", C_MUTED);
            else writeStr(startX + 3, startY + boxH - 2, "Up/Down: Chon  |  Enter: Luu  |  ESC: Huy", C_MUTED);

            render();

            int ch = _getch();
            if (ch == KEY_ESC) return false;
            if (ch == KEY_TAB) {
                curField = (curField + 1) % numFields;
            } else if (ch == KEY_ENTER) {
                bool valid = true;
                for (int i = 0; i < numFields; i++) {
                    if (strlen(values[i]) == 0) { valid = false; break; }
                }
                if (valid) return true;
                else showMessage("Vui long dien day du thong tin!", true);
            } else if (ch == 0 || ch == 224) {
                int ext = _getch();
                if (ext == KEY_UP) curField = (curField - 1 + numFields) % numFields;
                else if (ext == KEY_DOWN) curField = (curField + 1) % numFields;
                else if (ext == KEY_LEFT || ext == KEY_RIGHT) {
                    if (toggleOpts && toggleOpts[curField] && strlen(toggleOpts[curField]) > 0) {
                        char opts[10][50];
                        int optCount = 0;
                        char temp[256];
                        strcpy(temp, toggleOpts[curField]);
                        char* token = strtok(temp, ",");
                        while (token) {
                            strcpy(opts[optCount++], token);
                            token = strtok(nullptr, ",");
                        }
                        if (optCount > 0) {
                            int currentIdx = 0;
                            for (int i = 0; i < optCount; i++) {
                                if (strcmp(values[curField], opts[i]) == 0) { currentIdx = i; break; }
                            }
                            if (ext == KEY_LEFT) currentIdx = (currentIdx - 1 + optCount) % optCount;
                            else if (ext == KEY_RIGHT) currentIdx = (currentIdx + 1) % optCount;
                            strcpy(values[curField], opts[currentIdx]);
                        }
                    }
                }
            } else if (ch == 8) {
                bool isToggle = (toggleOpts && toggleOpts[curField] && strlen(toggleOpts[curField]) > 0);
                if (!isToggle) {
                    int len = (int)strlen(values[curField]);
                    if (len > 0) values[curField][len - 1] = '\0';
                }
            } else if (ch >= 32 && ch <= 126) {
                bool isToggle = (toggleOpts && toggleOpts[curField] && strlen(toggleOpts[curField]) > 0);
                if (!isToggle) {
                    int len = (int)strlen(values[curField]);
                    if (len < maxLens[curField]) {
                        bool isSpace = (ch == ' ');
                        bool lastIsSpace = (len > 0 && values[curField][len - 1] == ' ');
                        if (isSpace && (len == 0 || lastIsSpace)) {
                            // Bo qua: khong cho space dau tien hoac 2 space lien tiep
                        } else {
                            values[curField][len] = (char)ch;
                            values[curField][len + 1] = '\0';
                        }
                    }
                }
            }
        }
    }
};

struct ActiveBorrow {
    int maThe;
    char hoTen[60];
    char maSach[25];
    char tenSach[50];
    Date ngayMuon;
    int days;
};

struct TopRecord {
    int rank;
    DauSach* ds;
};

struct DisplayRowDS {
    bool isCategory;
    char categoryName[60];
    int countInCate;
    DauSach* ds;
    bool isLastInCategory;
};

class LibraryTUI {
private:
    LibraryService& service;
    ConsoleEngine screen;

    int currentTab = 0;
    int selectedRow = 0;
    int scrollOffset = 0;

    int selectedCol[5] = {0, 1, 0, 5, 0};
    int sortCol[5]     = {0, 1, 0, 5, 0};
    int sortState[5]   = {1, 1, 1, 2, 1};
    char searchKeyword[5][50] = {"", "", "", "", ""};

    int numFiltersForTab[5] = {7, 6, 6, 6, 5};

    bool viewCategoryTree = false;

    static const int MAX_MT_BUF = 500000;

    NodeDocGia** bufDgArrRaw;
    NodeDocGia** bufDgArr;

    ActiveBorrow* bufMtAll;
    ActiveBorrow* bufMtArr;

    OverdueRecord* bufQhRaw;
    OverdueRecord* bufQhArr;

    TopRecord* bufTrRaw;
    TopRecord* bufTrArr;

    DauSach* bufDsViewRaw[MAX_DAUSACH];
    DauSach* bufDsView[MAX_DAUSACH];
    DisplayRowDS bufTreeRows[MAX_DAUSACH * 2];
    DauSach* bufTopRaw[MAX_DAUSACH];

    void allocBuffers() {
        bufDgArrRaw = new NodeDocGia*[MAX_DOC_GIA];
        bufDgArr    = new NodeDocGia*[MAX_DOC_GIA];
        bufMtAll    = new ActiveBorrow[MAX_MT_BUF];
        bufMtArr    = new ActiveBorrow[MAX_MT_BUF];
        bufQhRaw    = new OverdueRecord[MAX_MT_BUF];
        bufQhArr    = new OverdueRecord[MAX_MT_BUF];
        bufTrRaw    = new TopRecord[MAX_DAUSACH];
        bufTrArr    = new TopRecord[MAX_DAUSACH];
    }

    void freeBuffers() {
        delete[] bufDgArrRaw;
        delete[] bufDgArr;
        delete[] bufMtAll;
        delete[] bufMtArr;
        delete[] bufQhRaw;
        delete[] bufQhArr;
        delete[] bufTrRaw;
        delete[] bufTrArr;
    }

    void adjustViewport(int totalItems, int visibleRows) {
        if (totalItems <= 0) { selectedRow = 0; scrollOffset = 0; return; }
        if (selectedRow < 0) selectedRow = 0;
        if (selectedRow >= totalItems) selectedRow = totalItems - 1;
        if (selectedRow < scrollOffset) scrollOffset = selectedRow;
        if (selectedRow >= scrollOffset + visibleRows) scrollOffset = selectedRow - visibleRows + 1;
    }

    bool matchKeyword(const char* text, const char* keyword) {
        if (!keyword || strlen(keyword) == 0) return true;
        char t[300], k[200];
        LibraryService::toUpperStr(t, text);
        LibraryService::toUpperStr(k, keyword);
        size_t kLen = strlen(k);
    
        // Kiem tra space cuoi
        bool hasTrailingSpace = (kLen > 0 && k[kLen - 1] == ' ');
        // Bo space cuoi
        while (kLen > 0 && k[kLen - 1] == ' ') k[--kLen] = '\0';
        if (kLen == 0) return true;
    
        // Buoc 1: strict prefix
        if (strncmp(t, k, kLen) != 0) return false;
    
        // Buoc 2: neu co space cuoi -> phai khop chinh xac toi cuoi tu
        if (hasTrailingSpace) {
            // Sau kLen ky tu phai la: het chuoi HOAC space (ranh gioi tu)
            if (t[kLen] != '\0' && t[kLen] != ' ') return false;
        }
    
        return true;
    }

    void buildHeaders(const char* raw[], int numCols, int tab, char outBuf[][50], const char* outPtr[]) {
        for (int i = 0; i < numCols; i++) {
            strcpy(outBuf[i], raw[i]);
            outPtr[i] = outBuf[i];
        }
    }

    int cmpDocGia(NodeDocGia* a, NodeDocGia* b, int col, int state) {
        int cmp = 0;
        if (col == 0) cmp = a->data.maThe - b->data.maThe;
        else if (col == 1) {
            cmp = _stricmp(a->data.ho, b->data.ho);
            if (cmp == 0) cmp = _stricmp(a->data.ten, b->data.ten);
        } else if (col == 2 || col == 3) {
            cmp = _stricmp(a->data.ten, b->data.ten);
            if (cmp == 0) cmp = _stricmp(a->data.ho, b->data.ho);
        } else if (col == 4) cmp = _stricmp(a->data.phai, b->data.phai);
        else if (col == 5) cmp = a->data.trangThai - b->data.trangThai;
        else if (col == 6) {
            int b1 = 0, b2 = 0; bool ov1, ov2;
            LibraryService::checkBorrowStatus(a->data, b1, ov1);
            LibraryService::checkBorrowStatus(b->data, b2, ov2);
            cmp = b1 - b2;
        }
        return state == 1 ? cmp : -cmp;
    }

    void qsDocGia(NodeDocGia* arr[], int left, int right, int col, int state) {
        int i = left, j = right;
        NodeDocGia* pivot = arr[(left + right) / 2];
        while (i <= j) {
            while (cmpDocGia(arr[i], pivot, col, state) < 0) i++;
            while (cmpDocGia(arr[j], pivot, col, state) > 0) j--;
            if (i <= j) {
                NodeDocGia* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
                i++; j--;
            }
        }
        if (left < j) qsDocGia(arr, left, j, col, state);
        if (i < right) qsDocGia(arr, i, right, col, state);
    }

    int cmpDauSach(DauSach* a, DauSach* b, int col, int state) {
        int cmp = 0;
        if (col == 0) cmp = _stricmp(a->ISBN, b->ISBN);
        else if (col == 1) cmp = _stricmp(a->tenSach, b->tenSach);
        else if (col == 2) cmp = _stricmp(a->tacGia, b->tacGia);
        else if (col == 3) cmp = _stricmp(a->theLoai, b->theLoai);
        else if (col == 4) cmp = a->namXB - b->namXB;
        else if (col == 5) cmp = a->soLuongSach - b->soLuongSach;
        return state == 1 ? cmp : -cmp;
    }

    void qsDauSach(DauSach* arr[], int left, int right, int col, int state) {
        int i = left, j = right;
        DauSach* pivot = arr[(left + right) / 2];
        while (i <= j) {
            while (cmpDauSach(arr[i], pivot, col, state) < 0) i++;
            while (cmpDauSach(arr[j], pivot, col, state) > 0) j--;
            if (i <= j) {
                DauSach* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
                i++; j--;
            }
        }
        if (left < j) qsDauSach(arr, left, j, col, state);
        if (i < right) qsDauSach(arr, i, right, col, state);
    }

    int cmpActiveBorrow(ActiveBorrow& a, ActiveBorrow& b, int col, int state) {
        int cmp = 0;
        if (col == 0) cmp = a.maThe - b.maThe;
        else if (col == 1) cmp = _stricmp(a.hoTen, b.hoTen);
        else if (col == 2) cmp = _stricmp(a.maSach, b.maSach);
        else if (col == 3) cmp = _stricmp(a.tenSach, b.tenSach);
        else if (col == 4) {
            long d1 = dateToDays(a.ngayMuon); long d2 = dateToDays(b.ngayMuon);
            cmp = (d1 > d2) - (d1 < d2);
        }
        else if (col == 5) cmp = a.days - b.days;
        return state == 1 ? cmp : -cmp;
    }

    void qsActiveBorrow(ActiveBorrow arr[], int left, int right, int col, int state) {
        int i = left, j = right;
        ActiveBorrow pivot = arr[(left + right) / 2];
        while (i <= j) {
            while (cmpActiveBorrow(arr[i], pivot, col, state) < 0) i++;
            while (cmpActiveBorrow(arr[j], pivot, col, state) > 0) j--;
            if (i <= j) {
                ActiveBorrow tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
                i++; j--;
            }
        }
        if (left < j) qsActiveBorrow(arr, left, j, col, state);
        if (i < right) qsActiveBorrow(arr, i, right, col, state);
    }

    int cmpOverdueRecord(OverdueRecord& a, OverdueRecord& b, int col, int state) {
        int cmp = 0;
        if (col == 0) cmp = a.maThe - b.maThe;
        else if (col == 1) cmp = _stricmp(a.hoTen, b.hoTen);
        else if (col == 2) cmp = _stricmp(a.maSach, b.maSach);
        else if (col == 3) cmp = _stricmp(a.tenSach, b.tenSach);
        else if (col == 4) {
            long d1 = dateToDays(a.ngayMuon); long d2 = dateToDays(b.ngayMuon);
            cmp = (d1 > d2) - (d1 < d2);
        }
        else if (col == 5) cmp = a.soNgayQuaHan - b.soNgayQuaHan;
        return state == 1 ? cmp : -cmp;
    }

    void qsOverdueRecord(OverdueRecord arr[], int left, int right, int col, int state) {
        int i = left, j = right;
        OverdueRecord pivot = arr[(left + right) / 2];
        while (i <= j) {
            while (cmpOverdueRecord(arr[i], pivot, col, state) < 0) i++;
            while (cmpOverdueRecord(arr[j], pivot, col, state) > 0) j--;
            if (i <= j) {
                OverdueRecord tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
                i++; j--;
            }
        }
        if (left < j) qsOverdueRecord(arr, left, j, col, state);
        if (i < right) qsOverdueRecord(arr, i, right, col, state);
    }

    void drawKeyHint(int& x, int y, const char* key, const char* desc) {
        if (x + (int)strlen(key) + (int)strlen(desc) + 4 >= SCREEN_W) return;
    
        // Key: nen xam dam (8), chu den (0) — khong ngoac
        char kBuf[20];
        sprintf(kBuf, " %s ", key);           // padding 1 o moi ben
        int kW = (int)strlen(kBuf);
    
        screen.fillRect(x, y, kW, 1, 0x80);   // to nen xam dam
        screen.writeStr(x, y, kBuf, 0x80);    // chu den
        x += kW + 1;
    
        // Desc: giu mau muted nhu cu
        screen.writeStr(x, y, desc, C_KEYDESC);
        x += (int)strlen(desc) + 2;
    }

    // =====================================================================
    // TOP DASHBOARD: them "Qua han" + moi metric mot mau noi bat
    // =====================================================================
    void drawTopDashboard(int totalDG, int totalDS) {
        screen.clear();

        screen.putWChar(2, 0, L'\u25C6', C_BRAND);
        screen.writeStr(4, 0, " Quan ly thu vien", C_MUTED);

        // Tinh so qua han
        int nQHCount = 0;
        {
            OverdueRecord tmp[2000];
            service.getOverdueList(tmp, nQHCount);
            if (nQHCount > 2000) nQHCount = 2000;
        }

        Date d = getCurrentDate();

        // Ve tung metric voi mau rieng, can le phai
        int rightEdge = SCREEN_W - 2;
        char dateBuf[30];
        sprintf(dateBuf, "%02d/%02d/%04d", d.d, d.m, d.y);
        int dateX = rightEdge - (int)strlen(dateBuf);
        screen.writeStr(dateX, 0, dateBuf, C_METRIC_DATE);

        char ohBuf[40]; sprintf(ohBuf, "Qua han: %d", nQHCount);
        int ohX = dateX - (int)strlen(ohBuf) - 4;
        screen.writeStr(ohX, 0, ohBuf, C_METRIC_OH);

        char mtBuf[40]; sprintf(mtBuf, "Dang muon: %d", service.getTotalBorrowing());
        int mtX = ohX - (int)strlen(mtBuf) - 4;
        screen.writeStr(mtX, 0, mtBuf, C_METRIC_MT);

        char dsBuf[40]; sprintf(dsBuf, "Dau sach: %d", totalDS);
        int dsX = mtX - (int)strlen(dsBuf) - 4;
        screen.writeStr(dsX, 0, dsBuf, C_METRIC_DS);

        char dgBuf[40]; sprintf(dgBuf, "Doc gia: %d", totalDG);
        int dgX = dsX - (int)strlen(dgBuf) - 4;
        screen.writeStr(dgX, 0, dgBuf, C_METRIC_DG);

        // Footer
        int kx = 2;
        if (currentTab == 0) {
            drawKeyHint(kx, SCREEN_H - 1, "ENTER", "Chi tiet");
            drawKeyHint(kx, SCREEN_H - 1, "F1", "Them");
            drawKeyHint(kx, SCREEN_H - 1, "F2", "Sua");
            drawKeyHint(kx, SCREEN_H - 1, "F3", "Xoa");
            drawKeyHint(kx, SCREEN_H - 1, "F4", "tang theo Ten+Ho/Ma");
            drawKeyHint(kx, SCREEN_H - 1, "F", "Tim");
            drawKeyHint(kx, SCREEN_H - 1, "S", "Sort");
            drawKeyHint(kx, SCREEN_H - 1, "TAB", "Chuyen Tab");
        } else if (currentTab == 1) {
            if (viewCategoryTree) {
                drawKeyHint(kx, SCREEN_H - 1, "ENTER", "Chi tiet");
                drawKeyHint(kx, SCREEN_H - 1, "F1", "Them");
                drawKeyHint(kx, SCREEN_H - 1, "F2", "Sua");
                drawKeyHint(kx, SCREEN_H - 1, "F3", "Xoa");
                drawKeyHint(kx, SCREEN_H - 1, "F4", "Bang thuong");
                drawKeyHint(kx, SCREEN_H - 1, "TAB", "Chuyen Tab");
            } else {
                drawKeyHint(kx, SCREEN_H - 1, "ENTER", "Chi tiet");
                drawKeyHint(kx, SCREEN_H - 1, "F1", "Them");
                drawKeyHint(kx, SCREEN_H - 1, "F2", "Sua");
                drawKeyHint(kx, SCREEN_H - 1, "F3", "Xoa");
                drawKeyHint(kx, SCREEN_H - 1, "F4", "The loai");
                drawKeyHint(kx, SCREEN_H - 1, "F5", "Tim Sach");
                drawKeyHint(kx, SCREEN_H - 1, "F", "Tim");
                drawKeyHint(kx, SCREEN_H - 1, "S", "Sort");
                drawKeyHint(kx, SCREEN_H - 1, "TAB", "Chuyen Tab");

            }
        } else if (currentTab == 2) {
            drawKeyHint(kx, SCREEN_H - 1, "F1", "Muon");
            drawKeyHint(kx, SCREEN_H - 1, "F2", "Tra");
            drawKeyHint(kx, SCREEN_H - 1, "F3", "Mat");
            drawKeyHint(kx, SCREEN_H - 1, "F", "Tim");
            drawKeyHint(kx, SCREEN_H - 1, "S", "Sort");
            drawKeyHint(kx, SCREEN_H - 1, "TAB", "Chuyen Tab");
        } else if (currentTab == 3) {
            drawKeyHint(kx, SCREEN_H - 1, "F", "Tim");
            drawKeyHint(kx, SCREEN_H - 1, "S", "Sort");
            drawKeyHint(kx, SCREEN_H - 1, "TAB", "Chuyen Tab");
        } else {
            drawKeyHint(kx, SCREEN_H - 1, "F", "Tim");
            drawKeyHint(kx, SCREEN_H - 1, "TAB", "Chuyen Tab");
        }
    }

    // =====================================================================
    // TABS: boc trong khoi bo goc
    // =====================================================================
    // Mỗi tab là một khối bo góc riêng biệt
    // Mỗi tab là một khối bo góc riêng, KHÔNG nền — chỉ viền + chữ
void drawTabs() {
    const char* tabs[5] = { "1. DOC GIA", "2. DAU SACH", "3. MUON/TRA", "4. QUA HAN", "5. TOP 10" };
    int xPos = 2;
    const int tabH = 2;
    const int tabY = 1;

    for (int i = 0; i < 5; i++) {
        int labelLen = (int)strlen(tabs[i]);
        int boxW = labelLen + 4;
        bool isActive = (i == currentTab);

        // Không nền: fillAttr = C_APP_BG (nền app), chỉ đổi màu viền + chữ
        WORD borderAttr = isActive ? C_BRAND : C_BOX_BORDER;
        WORD fillAttr   = C_APP_BG;
        WORD textAttr   = isActive ? C_BRAND : C_MUTED;

        screen.drawBox(xPos, tabY, boxW, tabH, fillAttr, borderAttr, "", false);
        screen.writeStr(xPos + 2, tabY + 1, tabs[i], textAttr);

        xPos += boxW + 2;
    }
}

    // =====================================================================
    // FILTER & SEARCH: label co mau, chip co background
    // =====================================================================
    void drawFieldAndSearchBars(bool isTyping) {
        screen.drawBox(1, 4, 119, 3, C_APP_BG, C_BOX_BORDER, "BO LOC & TIM KIEM", false, C_BRAND);
    
        const char* filters0[] = {"MA THE", "HO VA TEN", "TEN + HO", "TEN", "PHAI", "TRANG THAI", "DANG MUON"};
        const char* filters1[] = {"ISBN", "TEN SACH", "TAC GIA", "THE LOAI", "NAM XB", "SO SACH"};
        const char* filters2[] = {"MA THE", "HO TEN", "MA SACH", "TEN SACH", "NGAY MUON", "TINH TRANG"};
        const char* filters3[] = {"MA THE", "HO TEN", "MA SACH", "TEN SACH", "NGAY MUON", "QUA HAN"};
        const char* filters4[] = {"HANG", "ISBN", "TEN DAU SACH", "TAC GIA", "LUOT MUON"};
    
        const char** currentFilters = nullptr;
        if (currentTab == 0) currentFilters = filters0;
        else if (currentTab == 1) currentFilters = filters1;
        else if (currentTab == 2) currentFilters = filters2;
        else if (currentTab == 3) currentFilters = filters3;
        else if (currentTab == 4) currentFilters = filters4;
    
        int numF = numFiltersForTab[currentTab];
        int fIdx = selectedCol[currentTab];
        int sIdx = sortCol[currentTab];
        wchar_t arrow = (sortState[currentTab] == 1) ? U_ARR_UP : U_ARR_DN;
    
        screen.writeStr(3, 5, "Truong loc <-/->      :", C_LABEL_FILTER);
        int curX = 27;
    
        if (currentTab == 1 && viewCategoryTree) {
            screen.writeStr(curX, 5, "CHE DO CAY THE LOAI (Nhan F4 de quay lai)", C_BRAND);
        } else {
            for (int i = 0; i < numF; i++) {
                bool isSelected = (i == fIdx);
                bool isSorted   = (currentTab != 4 && i == sIdx);
    
                WORD chipAttr = isSelected ? C_FIELD_SEL_BG : C_FIELD_NORM_BG;

                // Sorted: chu + 2 o trong + arrow + 1 o padding phai
                // Thuong: chu + 1 o padding moi ben
                char chip[60];
                if (isSorted) sprintf(chip, " %s   ", currentFilters[i]);   // 3 dau cach sau chu
                else          sprintf(chip, " %s ",  currentFilters[i]);

                int chipW = (int)strlen(chip);

                screen.fillRect(curX, 5, chipW, 1, chipAttr);   // to nen
                screen.writeStr(curX, 5, chip, chipAttr);        // ghi chu

                if (isSorted) {
                    int arrowPos = curX + chipW - 2;             // arrow cach mep phai 1 o
                    screen.putWChar(arrowPos, 5, arrow, chipAttr);
                }

                curX += chipW + 1;
            }
        }
    
        // Search — giữ nguyên
        // Search
        if (currentTab == 1 && viewCategoryTree) {
            screen.writeStr(3, 6, "Tim kiem ", C_LABEL_SEARCH);
            screen.writeStr(12, 6, "Vo hieu hoa o che do cay the loai", C_MUTED);
        } else {
            screen.writeStr(3, 6, "Tim kiem ", C_LABEL_SEARCH);
            int curSearchX = 12;

            // Truong dang tim: [TEN TRUONG] - chu xanh, nen mac dinh (nen app)
            char fieldTag[40];
            sprintf(fieldTag, "[%s]", currentFilters[selectedCol[currentTab]]);
            screen.writeStr(curSearchX, 6, fieldTag, C_BADGE_OK);
            curSearchX += (int)strlen(fieldTag) + 1;

            const int BULLET_X = 25;
            screen.putWChar(BULLET_X, 6, ':', isTyping ? C_BRAND : C_MUTED);

            // O nhap bat dau co dinh tai cot 40
            const int INPUT_X = BULLET_X+2;

            int kwLen = (int)strlen(searchKeyword[currentTab]);
            if (kwLen == 0) {
                screen.writeStr(INPUT_X, 6, isTyping ? "" : "Nhan [F] de nhap tu khoa...", C_MUTED);
            } else {
                screen.writeStr(INPUT_X, 6, searchKeyword[currentTab], C_BRAND);
            }

            if (isTyping) {
                screen.putWChar(INPUT_X + kwLen, 6, L'_', C_BRAND);
                screen.writeStr(INPUT_X + 38, 6, "[ENTER] Xong   [ESC] Huy", C_BADGE_WARN);
            }
        }
    }

    void filterAndSortDocGia(NodeDocGia* rawArr[], int nRaw, NodeDocGia* arr[], int& nOut, bool alreadySortedByName) {
        nOut = 0;
        int searchC = selectedCol[0];
        int sortC   = sortCol[0];
        bool hasKeyword = (strlen(searchKeyword[0]) > 0);

        for (int i = 0; i < nRaw; i++) {
            if (!hasKeyword) {
                arr[nOut++] = rawArr[i];
                continue;
            }
            char val[300] = "";
            int b = 0; bool ov = false;
            if (searchC == 6) LibraryService::checkBorrowStatus(rawArr[i]->data, b, ov);

            if (searchC == 0) sprintf(val, "%d", rawArr[i]->data.maThe);
            else if (searchC == 1) sprintf(val, "%s %s", rawArr[i]->data.ho, rawArr[i]->data.ten);
            else if (searchC == 2) sprintf(val, "%s %s", rawArr[i]->data.ten, rawArr[i]->data.ho);
            else if (searchC == 3) strcpy(val, rawArr[i]->data.ten);
            else if (searchC == 4) strcpy(val, rawArr[i]->data.phai);
            else if (searchC == 5) strcpy(val, rawArr[i]->data.trangThai == HOAT_DONG ? "Hoat dong" : "Bi khoa");
            else if (searchC == 6) sprintf(val, "%d", b);

            if (matchKeyword(val, searchKeyword[0])) arr[nOut++] = rawArr[i];
        }

        if (sortC == 0 && sortState[0] == 1) return;
        if (alreadySortedByName) return;

        if (nOut > 1) {
            qsDocGia(arr, 0, nOut - 1, sortC, sortState[0]);
        }
    }

    // visibleRows = 20, grid h = 23 (title + header + divider + 20 rows + bottom)
    void renderTabDocGia(NodeDocGia* arr[], int nDG) {
        int visibleRows = 20;
        adjustViewport(nDG, visibleRows);
        const int colX[6] = { 3, 16, 49, 69, 81, 101 };
        const int colW[6] = { 11, 31, 18, 10, 18, 18 };
        char hBuf[6][50]; const char* headers[6];
        const char* rawHeaders[6] = { "MA THE", "HO VA TEN DEM", "TEN", "PHAI", "TRANG THAI", "DANG MUON" };
        buildHeaders(rawHeaders, 6, 0, hBuf, headers);
        screen.drawTableGrid(1, 8, 119, 23, colX, 6, headers, nDG, selectedRow, scrollOffset, "DANH SACH DOC GIA");
        for (int i = 0; i < visibleRows && (scrollOffset + i) < nDG; i++) {
            int idx = scrollOffset + i;
            int rowY = 11 + i;
            bool isSel = (idx == selectedRow);
            WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 6, isSel);
            char sMa[15]; sprintf(sMa, "%d", arr[idx]->data.maThe);
            screen.writeCell(colX[0] + 1, rowY, colW[0], sMa, rowAttr);
            screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx]->data.ho, rowAttr);
            screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx]->data.ten, rowAttr);
            screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx]->data.phai, rowAttr);
            bool active = (arr[idx]->data.trangThai == HOAT_DONG);
            int borrowing = 0; bool ov = false;
            LibraryService::checkBorrowStatus(arr[idx]->data, borrowing, ov);
            char sMuon[15]; sprintf(sMuon, "%d/3 cuon", borrowing);
            screen.writeCell(colX[4] + 1, rowY, colW[4], active ? "Hoat dong" : "Bi khoa", isSel ? C_ROW_SEL : (active ? C_BADGE_OK : C_BADGE_ERR));
            screen.writeCell(colX[5] + 1, rowY, colW[5], sMuon, isSel ? C_ROW_SEL : (ov ? C_BADGE_ERR : rowAttr));
        }
    }

    void filterAndSortDauSach(DauSach* rawArr[], int nRaw, DauSach* arr[], int& nOut) {
        nOut = 0;
        int searchC = selectedCol[1];
        int sortC   = sortCol[1];
        for (int i = 0; i < nRaw; i++) {
            char val[100] = "";
            if (searchC == 0) strcpy(val, rawArr[i]->ISBN);
            else if (searchC == 1) strcpy(val, rawArr[i]->tenSach);
            else if (searchC == 2) strcpy(val, rawArr[i]->tacGia);
            else if (searchC == 3) strcpy(val, rawArr[i]->theLoai);
            else if (searchC == 4) sprintf(val, "%d", rawArr[i]->namXB);
            else if (searchC == 5) sprintf(val, "%d", rawArr[i]->soLuongSach);
            if (matchKeyword(val, searchKeyword[1])) arr[nOut++] = rawArr[i];
        }
        if (sortC == 1 && sortState[1] == 1) return;
        if (nOut > 1) {
            qsDauSach(arr, 0, nOut - 1, sortC, sortState[1]);
        }
    }

    void buildCategoryTreeRows(DisplayRowDS rows[], int& nRows) {
        DauSach* sortedArr[MAX_DAUSACH];
        int count = 0;
        service.getBooksSortedByCategory(sortedArr, count);
        nRows = 0;
        int i = 0;
        while (i < count) {
            int j = i;
            while (j < count && _stricmp(sortedArr[j]->theLoai, sortedArr[i]->theLoai) == 0) j++;
            DisplayRowDS catRow;
            catRow.isCategory = true;
            strcpy(catRow.categoryName, sortedArr[i]->theLoai);
            catRow.countInCate = j - i;
            catRow.ds = nullptr;
            catRow.isLastInCategory = false;
            rows[nRows++] = catRow;
            for (int k = i; k < j; k++) {
                DisplayRowDS bookRow;
                bookRow.isCategory = false;
                strcpy(bookRow.categoryName, sortedArr[i]->theLoai);
                bookRow.countInCate = j - i;
                bookRow.ds = sortedArr[k];
                bookRow.isLastInCategory = (k == j - 1);
                rows[nRows++] = bookRow;
            }
            i = j;
        }
    }

    void renderTabDauSach(DauSach* arr[], int nDS, DisplayRowDS treeRows[], int nTreeRows) {
        int visibleRows = 20;
        if (viewCategoryTree) {
            adjustViewport(nTreeRows, visibleRows);
            const int colX[5] = { 3, 17, 69, 99, 110 };
            const int colW[5] = { 12, 50, 28, 9, 9 };
            const char* headers[5] = { "ISBN", "THE LOAI / TEN DAU SACH", "TAC GIA", "NAM XB", "SO SACH" };
            screen.drawTableGrid(1, 8, 119, 23, colX, 5, headers, nTreeRows, selectedRow, scrollOffset, "CAY DANH MUC");
            for (int i = 0; i < visibleRows && (scrollOffset + i) < nTreeRows; i++) {
                int idx = scrollOffset + i;
                int rowY = 11 + i;
                bool isSel = (idx == selectedRow);
                WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 5, isSel);
                DisplayRowDS& item = treeRows[idx];
                if (item.isCategory) {
                    WORD cateAttr = isSel ? C_CATE_HEADER_SEL : C_CATE_HEADER;
                    char catTitle[100];
                    sprintf(catTitle, "[-] %s (%d)", item.categoryName, item.countInCate);
                    screen.writeCell(colX[0] + 1, rowY, colW[0], "", rowAttr);
                    screen.writeCell(colX[1] + 1, rowY, colW[1], catTitle, cateAttr);
                } else {
                    char sNam[10], sQty[10];
                    sprintf(sNam, "%d", item.ds->namXB);
                    sprintf(sQty, "%d", item.ds->soLuongSach);
                    screen.writeCell(colX[0] + 1, rowY, colW[0], item.ds->ISBN, rowAttr);
                    screen.putWChar(colX[1] + 3, rowY, item.isLastInCategory ? U_BL_TREE : U_L, isSel ? C_ROW_SEL : C_BRAND);
                    screen.putWChar(colX[1] + 4, rowY, U_H, isSel ? C_ROW_SEL : C_BRAND);
                    screen.writeCell(colX[1] + 6, rowY, colW[1] - 5, item.ds->tenSach, rowAttr);
                    screen.writeCell(colX[2] + 1, rowY, colW[2], item.ds->tacGia, rowAttr);
                    screen.writeCell(colX[3] + 1, rowY, colW[3], sNam, rowAttr);
                    screen.writeCell(colX[4] + 1, rowY, colW[4], sQty, rowAttr);
                }
            }
        } else {
            adjustViewport(nDS, visibleRows);
            const int colX[6] = { 3, 17, 53, 76, 97, 108 };
            const int colW[6] = { 12, 34, 21, 19, 9, 10 };
            char hBuf[6][50]; const char* headers[6];
            const char* rawHeaders[6] = { "ISBN", "TEN SACH", "TAC GIA", "THE LOAI", "NAM XB", "SO SACH" };
            buildHeaders(rawHeaders, 6, 1, hBuf, headers);
            screen.drawTableGrid(1, 8, 119, 23, colX, 6, headers, nDS, selectedRow, scrollOffset, "DANH SACH DAU SACH");
            for (int i = 0; i < visibleRows && (scrollOffset + i) < nDS; i++) {
                int idx = scrollOffset + i; int rowY = 11 + i;
                bool isSel = (idx == selectedRow);
                WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 6, isSel);
                char sNam[10]; sprintf(sNam, "%d", arr[idx]->namXB);
                char sQty[10]; sprintf(sQty, "%d", arr[idx]->soLuongSach);
                screen.writeCell(colX[0] + 1, rowY, colW[0], arr[idx]->ISBN, rowAttr);
                screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx]->tenSach, rowAttr);
                screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx]->tacGia, rowAttr);
                screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx]->theLoai, rowAttr);
                screen.writeCell(colX[4] + 1, rowY, colW[4], sNam, rowAttr);
                screen.writeCell(colX[5] + 1, rowY, colW[5], sQty, rowAttr);
            }
        }
    }

    void filterAndSortMuonTra(ActiveBorrow rawArr[], int nRaw, ActiveBorrow arr[], int& nOut) {
        nOut = 0;
        int searchC = selectedCol[2];
        int sortC   = sortCol[2];
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            char dStr[20]; sprintf(dStr, "%02d/%02d/%04d", rawArr[i].ngayMuon.d, rawArr[i].ngayMuon.m, rawArr[i].ngayMuon.y);
            const char* tt = rawArr[i].days > 7 ? "! QUA HAN" : "Dang muon";
            if (searchC == 0) sprintf(val, "%d", rawArr[i].maThe);
            else if (searchC == 1) strcpy(val, rawArr[i].hoTen);
            else if (searchC == 2) strcpy(val, rawArr[i].maSach);
            else if (searchC == 3) strcpy(val, rawArr[i].tenSach);
            else if (searchC == 4) strcpy(val, dStr);
            else if (searchC == 5) strcpy(val, tt);
            if (matchKeyword(val, searchKeyword[2])) arr[nOut++] = rawArr[i];
        }
        if (sortC == 0 && sortState[2] == 1) return;
        if (nOut > 1) {
            qsActiveBorrow(arr, 0, nOut - 1, sortC, sortState[2]);
        }
    }

    void renderTabMuonTra(ActiveBorrow arr[], int nMT) {
        int visibleRows = 20; adjustViewport(nMT, visibleRows);
        const int colX[6] = { 3, 14, 40, 56, 89, 103 };
        const int colW[6] = { 9, 24, 14, 31, 12, 15 };
        char hBuf[6][50]; const char* headers[6];
        const char* rawHeaders[6] = { "MA THE", "HO TEN", "MA SACH", "TEN SACH", "NGAY MUON", "TINH TRANG" };
        buildHeaders(rawHeaders, 6, 2, hBuf, headers);
        screen.drawTableGrid(1, 8, 119, 23, colX, 6, headers, nMT, selectedRow, scrollOffset, "PHIEU DANG MUON");
        for (int i = 0; i < visibleRows && (scrollOffset + i) < nMT; i++) {
            int idx = scrollOffset + i; int rowY = 11 + i;
            bool isSel = (idx == selectedRow);
            WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 6, isSel);
            char sMa[15], dateStr[20];
            sprintf(sMa, "%d", arr[idx].maThe); sprintf(dateStr, "%02d/%02d/%04d", arr[idx].ngayMuon.d, arr[idx].ngayMuon.m, arr[idx].ngayMuon.y);
            screen.writeCell(colX[0] + 1, rowY, colW[0], sMa, rowAttr);
            screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx].hoTen, rowAttr);
            screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx].maSach, rowAttr);
            screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx].tenSach, rowAttr);
            screen.writeCell(colX[4] + 1, rowY, colW[4], dateStr, rowAttr);
            bool over = (arr[idx].days > 7);
            screen.writeCell(colX[5] + 1, rowY, colW[5], over ? "! QUA HAN" : "Dang muon", isSel ? C_ROW_SEL : (over ? C_BADGE_ERR : C_BADGE_OK));
        }
    }

    void filterAndSortQuaHan(OverdueRecord rawArr[], int nRaw, OverdueRecord arr[], int& nOut) {
        nOut = 0;
        int searchC = selectedCol[3];
        int sortC   = sortCol[3];
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            char dStr[20]; sprintf(dStr, "%02d/%02d/%04d", rawArr[i].ngayMuon.d, rawArr[i].ngayMuon.m, rawArr[i].ngayMuon.y);
            if (searchC == 0) sprintf(val, "%d", rawArr[i].maThe);
            else if (searchC == 1) strcpy(val, rawArr[i].hoTen);
            else if (searchC == 2) strcpy(val, rawArr[i].maSach);
            else if (searchC == 3) strcpy(val, rawArr[i].tenSach);
            else if (searchC == 4) strcpy(val, dStr);
            else if (searchC == 5) sprintf(val, "%d", rawArr[i].soNgayQuaHan);
            if (matchKeyword(val, searchKeyword[3])) arr[nOut++] = rawArr[i];
        }
        // Mặc định: cột 5 (QUA HAN) giảm dần — getOverdueList đã sort sẵn theo thứ tự này.
        // Lọc keyword không làm đảo thứ tự tương đối -> giữ nguyên, khỏi sort lại.
        if (sortC == 5 && sortState[3] == 2) return;
        if (nOut > 1) {
            qsOverdueRecord(arr, 0, nOut - 1, sortC, sortState[3]);
        }
    }

    void renderTabQuaHan(OverdueRecord arr[], int nQH) {
        int visibleRows = 20; adjustViewport(nQH, visibleRows);
        const int colX[6] = { 3, 14, 40, 56, 89, 103 };
        const int colW[6] = { 9, 24, 14, 31, 12, 15 };
        char hBuf[6][50]; const char* headers[6];
        const char* rawHeaders[6] = { "MA THE", "HO TEN", "MA SACH", "TEN SACH", "NGAY MUON", "QUA HAN" };
        buildHeaders(rawHeaders, 6, 3, hBuf, headers);
        screen.drawTableGrid(1, 8, 119, 23, colX, 6, headers, nQH, selectedRow, scrollOffset, "DANH SACH QUA HAN");
        for (int i = 0; i < visibleRows && (scrollOffset + i) < nQH; i++) {
            int idx = scrollOffset + i; int rowY = 11 + i;
            bool isSel = (idx == selectedRow); WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 6, isSel);
            char sMa[15], dStr[20], sOver[20];
            sprintf(sMa, "%d", arr[idx].maThe); sprintf(dStr, "%02d/%02d/%04d", arr[idx].ngayMuon.d, arr[idx].ngayMuon.m, arr[idx].ngayMuon.y);
            sprintf(sOver, "%d ngay", arr[idx].soNgayQuaHan);
            screen.writeCell(colX[0] + 1, rowY, colW[0], sMa, rowAttr); screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx].hoTen, rowAttr);
            screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx].maSach, rowAttr); screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx].tenSach, rowAttr);
            screen.writeCell(colX[4] + 1, rowY, colW[4], dStr, rowAttr); screen.writeCell(colX[5] + 1, rowY, colW[5], sOver, isSel ? C_ROW_SEL : C_BADGE_ERR);
        }
    }

    void filterTop10(TopRecord rawArr[], int nRaw, TopRecord arr[], int& nOut) {
        nOut = 0;
        int searchC = selectedCol[4];
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            if (searchC == 0) sprintf(val, "#%d", rawArr[i].rank);
            else if (searchC == 1) strcpy(val, rawArr[i].ds->ISBN);
            else if (searchC == 2) strcpy(val, rawArr[i].ds->tenSach);
            else if (searchC == 3) strcpy(val, rawArr[i].ds->tacGia);
            else if (searchC == 4) sprintf(val, "%d", rawArr[i].ds->soLuotMuon);
            if (matchKeyword(val, searchKeyword[4])) arr[nOut++] = rawArr[i];
        }
    }

    void renderTabTop10(TopRecord arr[], int nT10) {
        int visibleRows = 20; adjustViewport(nT10, visibleRows);
        const int colX[5] = { 3, 12, 28, 68, 97 };
        const int colW[5] = { 7, 14, 38, 27, 21 };
        char hBuf[5][50]; const char* headers[5];
        const char* rawHeaders[5] = { "HANG", "ISBN", "TEN DAU SACH", "TAC GIA", "LUOT MUON" };
        buildHeaders(rawHeaders, 5, 4, hBuf, headers);
        screen.drawTableGrid(1, 8, 119, 23, colX, 5, headers, nT10, selectedRow, scrollOffset, "TOP 10 SACH MUON NHIEU NHAT");
        for (int i = 0; i < visibleRows && (scrollOffset + i) < nT10; i++) {
            int idx = scrollOffset + i; int rowY = 11 + i;
            bool isSel = (idx == selectedRow); WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 5, isSel);
            char sRank[10], sBorrows[25]; sprintf(sRank, "#%d", arr[idx].rank); sprintf(sBorrows, "%d luot", arr[idx].ds->soLuotMuon);
            screen.writeCell(colX[0] + 1, rowY, colW[0], sRank, isSel ? C_ROW_SEL : C_BADGE_WARN);
            screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx].ds->ISBN, rowAttr);
            screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx].ds->tenSach, rowAttr);
            screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx].ds->tacGia, rowAttr);
            screen.writeCell(colX[4] + 1, rowY, colW[4], sBorrows, isSel ? C_ROW_SEL : C_BADGE_OK);
        }
    }

    // Cac dialog giu nguyen (khong doi vi khong lien quan yeu cau)...
    void dialogAddOrEditDocGia(NodeDocGia* editNode = nullptr) {
        const char* labels[4] = { "Ho va ten dem :", "Ten doc gia   :", "Phai (Nam/Nu) :", "Trang thai    :" };
        char values[4][50] = { "", "", "Nam", "Hoat dong" };
        const int maxLens[4] = { 30, 17, 5, 20 };
        const char* toggleOpts[4] = { "", "", "Nam,Nu", "Hoat dong,Bi khoa" };
        int newKey = editNode ? editNode->data.maThe : service.generateReaderKey();
        if (editNode) {
            strcpy(values[0], editNode->data.ho);
            strcpy(values[1], editNode->data.ten);
            strcpy(values[2], editNode->data.phai);
            strcpy(values[3], editNode->data.trangThai == HOAT_DONG ? "Hoat dong" : "Bi khoa");
        }
        char info[60]; sprintf(info, "Ma The Doc Gia (Cap tu dong): #%d", newKey);
        if (screen.showFormDialog(editNode ? "HIEU CHINH DOC GIA" : "THEM DOC GIA", labels, values, maxLens, 4, info, toggleOpts)) {
            if (editNode) {
                strcpy(editNode->data.ho, values[0]);
                strcpy(editNode->data.ten, values[1]);
                strcpy(editNode->data.phai, values[2]);
                editNode->data.trangThai = (_stricmp(values[3], "Bi khoa") == 0) ? KHOA : HOAT_DONG;
            } else {
                TheDocGia dg;
                dg.maThe = newKey;
                strcpy(dg.ho, values[0]);
                strcpy(dg.ten, values[1]);
                strcpy(dg.phai, values[2]);
                dg.trangThai = (_stricmp(values[3], "Bi khoa") == 0) ? KHOA : HOAT_DONG;
                dg.dsMuonTra = nullptr;
                service.addDocGia(dg);
            }
        }
    }

    void dialogAddDauSach() {
        const char* labels[5] = { "Ma ISBN (978...):", "Ten sach        :", "Tac gia         :", "The loai        :", "So trang        :" };
        char values[5][50] = { "", "", "", "", "" };
        const int maxLens[5] = { 12, 35, 35, 35, 6 };
        if (screen.showFormDialog("THEM DAU SACH", labels, values, maxLens, 5)) {
            DauSach* ds = new DauSach;
            strcpy(ds->ISBN, values[0]);
            strcpy(ds->tenSach, values[1]);
            strcpy(ds->tacGia, values[2]);
            strcpy(ds->theLoai, values[3]);
            ds->soTrang = atoi(values[4]);
            ds->namXB = getCurrentDate().y;
            ds->dms = nullptr;
            ds->soLuongSach = 0;
            ds->soLuotMuon = 0;
            if (!service.insertDauSach(ds)) {
                delete ds; screen.showMessage("Loi: Trung ma ISBN!", true);
            }
        }
    }

    void dialogEditDauSach(DauSach* ds) {
        if (!ds) return;
        const char* labels[5] = { "Ten sach        :", "Tac gia         :", "The loai        :", "Nam XB          :", "So trang        :" };
        char values[5][50];
        strcpy(values[0], ds->tenSach);
        strcpy(values[1], ds->tacGia);
        strcpy(values[2], ds->theLoai);
        sprintf(values[3], "%d", ds->namXB);
        sprintf(values[4], "%d", ds->soTrang);
        const int maxLens[5] = { 40, 30, 25, 4, 6 };
        char info[60]; sprintf(info, "Ma ISBN: %s (Khong duoc phep sua)", ds->ISBN);
        if (screen.showFormDialog("HIEU CHINH DAU SACH", labels, values, maxLens, 5, info)) {
            if (service.updateDauSach(ds->ISBN, values[0], values[1], values[2], atoi(values[3]), atoi(values[4]))) {
                screen.showMessage("Da cap nhat thong tin Dau sach!");
            } else screen.showMessage("Loi khi cap nhat!", true);
        }
    }

    void dialogBorrowOrReturn(int mode) {
        const char* titles[3] = { "MUON SACH", "TRA SACH", "BAO MAT SACH" };
        const char* labels[2] = { "Ma The :", "Ma Sach:" }; char values[2][50] = { "", "" }; const int maxLens[2] = { 10, 22 };
        if (screen.showFormDialog(titles[mode], labels, values, maxLens, 2)) {
            const char* res = (mode == 0) ? service.borrowBook(atoi(values[0]), values[1]) : service.returnBook(atoi(values[0]), values[1], mode);
            if (strcmp(res, "OK") != 0) screen.showMessage(res, true);
        }
    }

    // (Cac dialog renderDialogChiTietSach / DocGia / TraCuu giu nguyen, khong in ra day de tiet kiem)
    // ... copy nguyen tu file goc ...

    void renderDialogChiTietSach(DauSach* ds) {
        if (!ds) return;
        int selRow = 0; int scrOffset = 0;
        while (true) {
            NodeSach* arr[500]; int count = 0;
            NodeSach* p = ds->dms;
            while (p && count < 500) { arr[count++] = p; p = p->next; }
            int visibleRows = 11;
            if (count <= 0) { selRow = 0; scrOffset = 0; }
            else {
                if (selRow < 0) selRow = 0;
                if (selRow >= count) selRow = count - 1;
                if (selRow < scrOffset) scrOffset = selRow;
                if (selRow >= scrOffset + visibleRows) scrOffset = selRow - visibleRows + 1;
            }
            int boxW = 82, boxH = 22;
            int startX = (SCREEN_W - boxW) / 2;
            int startY = (SCREEN_H - boxH) / 2;
            screen.drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BORDER, "CHI TIET SACH");
            char titleInfo[150];
            sprintf(titleInfo, "Dau sach: %s  |  So trang: %d  |  Tong so cuon: %d", ds->tenSach, ds->soTrang, ds->soLuongSach);
            screen.writeStr(startX + 3, startY + 2, titleInfo, C_ROW_NORM);
            const int colX[3] = { startX + 4, startX + 23, startX + 45 };
            const int colW[3] = { 18, 21, 33 };
            const char* headers[3] = { "MA SACH", "TRANG THAI", "VI TRI KE SACH" };
            int tableX = startX + 2; int tableY = startY + 4; int tableW = boxW - 4; int tableH = 14;
            screen.drawTableGrid(tableX, tableY, tableW, tableH, colX, 3, headers, count, selRow, scrOffset, "DANH MUC BAN SAO");
            for (int i = 0; i < visibleRows && (scrOffset + i) < count; i++) {
                int idx = scrOffset + i; int rowY = tableY + 3 + i;
                bool isSel = (idx == selRow);
                WORD rowAttr = screen.prepareTableRow(tableX, rowY, tableW, idx, colX, 3, isSel);
                screen.writeCell(colX[0], rowY, colW[0], arr[idx]->data.maSach, rowAttr);
                const char* ttStr = "Cho muon duoc";
                WORD stAttr = isSel ? C_ROW_SEL : C_BADGE_OK;
                if (arr[idx]->data.trangThai == DA_CHO_MUON) { ttStr = "Da muon"; stAttr = isSel ? C_ROW_SEL : C_BADGE_WARN; }
                else if (arr[idx]->data.trangThai == DA_THANH_LY) { ttStr = "Da thanh ly"; stAttr = isSel ? C_ROW_SEL : C_BADGE_ERR; }
                screen.writeCell(colX[1], rowY, colW[1], ttStr, stAttr);
                screen.writeCell(colX[2], rowY, colW[2], arr[idx]->data.viTri, rowAttr);
            }
            screen.writeStr(startX + 3, startY + boxH - 2, "[F1] Them  [F2] Sua VTri  [F3] Xoa  [F4] Thanh ly  [ESC] Dong", C_MUTED);
            screen.render();
            int ch = _getch();
            if (ch == KEY_ESC) break;
            if (ch == 0 || ch == 224) {
                int ext = _getch();
                if (ext == KEY_UP) selRow--;
                else if (ext == KEY_DOWN) selRow++;
                else if (ext == KEY_F1) {
                    const char* lb[2] = { "So luong them:", "Vi tri ke:" };
                    char val[2][50] = { "1", "Ke A" };
                    const int ml[2] = { 3, 25 };
                    if (screen.showFormDialog("NHAP THEM SACH", lb, val, ml, 2)) {
                        int num = atoi(val[0]);
                        if (num > 0) service.addSachAuto(ds, num, val[1]);
                    }
                }
                else if (ext == KEY_F2 && count > 0) {
                    if (arr[selRow]->data.trangThai == DA_CHO_MUON) { screen.showMessage("Khong the sua vi sach dang duoc muon!", true); continue; }
                    if (arr[selRow]->data.trangThai == DA_THANH_LY) { screen.showMessage("Sach da thanh ly, khong the doi vi tri!", true); continue; }
                    const char* lb[1] = { "Vi tri ke:" }; char val[1][50]; strcpy(val[0], arr[selRow]->data.viTri); const int ml[1] = { 25 };
                    if (screen.showFormDialog("HIEU CHINH VI TRI SACH", lb, val, ml, 1)) strcpy(arr[selRow]->data.viTri, val[0]);
                }
                else if (ext == KEY_F3 && count > 0) {
                    if (arr[selRow]->data.trangThai == DA_CHO_MUON) { screen.showMessage("Khong the xoa vi sach dang duoc muon!", true); continue; }
                    if (screen.showConfirm("Xac nhan xoa cuon sach nay?")) {
                        if (service.removeSachVatLy(ds, arr[selRow]->data.maSach)) {
                            if (selRow >= count - 1 && selRow > 0) selRow--;
                        } else screen.showMessage("Loi khi xoa sach!", true);
                    }
                } else if (ext == KEY_F4 && count > 0) {
                    if (arr[selRow]->data.trangThai == DA_CHO_MUON) { screen.showMessage("Khong the thanh ly vi sach dang duoc muon!", true); continue; }
                    if (arr[selRow]->data.trangThai == DA_THANH_LY) { screen.showMessage("Sach nay da duoc thanh ly roi!", true); continue; }
                    if (screen.showConfirm("Xac nhan THANH LY cuon sach nay?")) {
                        arr[selRow]->data.trangThai = DA_THANH_LY; strcpy(arr[selRow]->data.viTri, "");
                    }
                }
            }
        }
        screen.clear();
    }

    void renderDialogChiTietDocGia(NodeDocGia* dg) {
        if (!dg) return;
        int selRow = 0; int scrOffset = 0;
        while (true) {
            NodeMuonTra* arr[500]; int count = 0;
            NodeMuonTra* p = dg->data.dsMuonTra;
            while (p && count < 500) { arr[count++] = p; p = p->next; }
            int visibleRows = 11;
            if (count <= 0) { selRow = 0; scrOffset = 0; }
            else {
                if (selRow < 0) selRow = 0;
                if (selRow >= count) selRow = count - 1;
                if (selRow < scrOffset) scrOffset = selRow;
                if (selRow >= scrOffset + visibleRows) scrOffset = selRow - visibleRows + 1;
            }
            int boxW = 106, boxH = 22;
            int startX = (SCREEN_W - boxW) / 2;
            int startY = (SCREEN_H - boxH) / 2;
            screen.drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BORDER, "CHI TIET DOC GIA");
            char titleInfo[100];
            sprintf(titleInfo, "Doc gia: %d - %s %s  |  Trang thai: %s", dg->data.maThe, dg->data.ho, dg->data.ten,
                    dg->data.trangThai == HOAT_DONG ? "Hoat dong" : "Bi khoa");
            screen.writeStr(startX + 3, startY + 2, titleInfo, C_ROW_NORM);
            const int colX[5] = { startX + 4, startX + 18, startX + 62, startX + 76, startX + 90 };
            const int colW[5] = { 13, 43, 13, 13, 12 };
            const char* headers[5] = { "MA SACH", "TEN SACH", "NGAY MUON", "NGAY TRA", "TRANG THAI" };
            int tableX = startX + 2; int tableY = startY + 4; int tableW = boxW - 4; int tableH = 14;
            screen.drawTableGrid(tableX, tableY, tableW, tableH, colX, 5, headers, count, selRow, scrOffset, "LICH SU MUON TRA");
            Date today = getCurrentDate();
            for (int i = 0; i < visibleRows && (scrOffset + i) < count; i++) {
                int idx = scrOffset + i; int rowY = tableY + 3 + i;
                bool isSel = (idx == selRow);
                WORD rowAttr = screen.prepareTableRow(tableX, rowY, tableW, idx, colX, 5, isSel);
                screen.writeCell(colX[0], rowY, colW[0], arr[idx]->data.maSach, rowAttr);
                NodeSach* ns = nullptr;
                DauSach* ds = service.findDauSachByMaSach(arr[idx]->data.maSach, ns);
                screen.writeCell(colX[1], rowY, colW[1], ds ? ds->tenSach : "N/A", rowAttr);
                char nm[20], nt[20];
                sprintf(nm, "%02d/%02d/%04d", arr[idx]->data.ngayMuon.d, arr[idx]->data.ngayMuon.m, arr[idx]->data.ngayMuon.y);
                if (arr[idx]->data.trangThai == DANG_MUON) strcpy(nt, "--/--/----");
                else sprintf(nt, "%02d/%02d/%04d", arr[idx]->data.ngayTra.d, arr[idx]->data.ngayTra.m, arr[idx]->data.ngayTra.y);
                screen.writeCell(colX[2], rowY, colW[2], nm, rowAttr);
                screen.writeCell(colX[3], rowY, colW[3], nt, rowAttr);
                const char* ttStr = "Dang muon";
                WORD statusAttr = isSel ? C_ROW_SEL : C_BADGE_OK;
                if (arr[idx]->data.trangThai == DA_TRA) { ttStr = "Da tra"; statusAttr = rowAttr; }
                else if (arr[idx]->data.trangThai == LAM_MAT) { ttStr = "Mat sach"; statusAttr = isSel ? C_ROW_SEL : C_BADGE_ERR; }
                else if (arr[idx]->data.trangThai == DANG_MUON) {
                    if (daysBetween(arr[idx]->data.ngayMuon, today) > 7) { ttStr = "! Qua han"; statusAttr = isSel ? C_ROW_SEL : C_BADGE_ERR; }
                }
                screen.writeCell(colX[4], rowY, colW[4], ttStr, statusAttr);
            }
            screen.writeStr(startX + 3, startY + boxH - 2, "[Up/Down] Cuon danh sach   [ESC] Dong", C_MUTED);
            screen.render();
            int ch = _getch();
            if (ch == KEY_ESC) break;
            if (ch == 0 || ch == 224) {
                int ext = _getch();
                if (ext == KEY_UP) selRow--;
                else if (ext == KEY_DOWN) selRow++;
            }
        }
        screen.clear();
    }

    void dialogTraCuuMaSach() {
        const char* labels[1] = { "Nhap Ma Sach (VD: 97812_1):" };
        char values[1][50] = { "" };
        const int maxLens[1] = { 20 };
        if (screen.showFormDialog("TRA CUU SACH THEO MA", labels, values, maxLens, 1)) {
            NodeSach* bookNode = nullptr;
            DauSach* ds = service.findDauSachByMaSach(values[0], bookNode);
            if (ds && bookNode) {
                int boxW = 66, boxH = 12;
                int startX = (SCREEN_W - boxW) / 2;
                int startY = (SCREEN_H - boxH) / 2;
                screen.drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BORDER, "KET QUA TRA CUU");
            
                const char* status = "Cho muon duoc";
                WORD statusColor = C_BADGE_OK;
                if (bookNode->data.trangThai == DA_CHO_MUON) { status = "Dang duoc muon"; statusColor = C_BADGE_WARN; }
                else if (bookNode->data.trangThai == DA_THANH_LY) { status = "Da thanh ly"; statusColor = C_BADGE_ERR; }
            
                char line[160];
                int y = startY + 2;
                const int LX = startX + 3;
                const int VX = startX + 16;
            
                screen.writeStr(LX, y, "Ma ISBN    :", C_MUTED);
                screen.writeStr(VX, y, ds->ISBN, C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "Ten sach   :", C_MUTED);
                screen.writeStr(VX, y, ds->tenSach, C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "Tac gia    :", C_MUTED);
                screen.writeStr(VX, y, ds->tacGia, C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "Nam XB     :", C_MUTED);
                sprintf(line, "%d", ds->namXB);
                screen.writeStr(VX, y, line, C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "The loai   :", C_MUTED);
                screen.writeStr(VX, y, ds->theLoai, C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "Ma sach    :", C_MUTED);
                screen.writeStr(VX, y, bookNode->data.maSach, C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "Vi tri ke  :", C_MUTED);
                screen.writeStr(VX, y, strlen(bookNode->data.viTri) ? bookNode->data.viTri : "-", C_ROW_NORM); y++;
            
                screen.writeStr(LX, y, "Trang thai :", C_MUTED);
                screen.writeStr(VX, y, status, statusColor);
            
                screen.writeStr(startX + 18, startY + boxH - 2, "[ ENTER / ESC de dong ]", C_MUTED);
                screen.render();
                int ch; while ((ch = _getch()) != KEY_ESC && ch != KEY_ENTER);
                screen.clear();
            } else screen.showMessage("Khong tim thay Ma Sach nay trong he thong!", true);
        }
    }

public:
    LibraryTUI(LibraryService& srv) : service(srv) {
        screen.init("LIB-OS // Modern Library Management");
        allocBuffers();
    }

    ~LibraryTUI() { freeBuffers(); }

    void run() {
        bool isSearching = false;
        char backupKeyword[50] = "";
        bool needUpdateData = true;

        int nDG = 0, nDS = 0, nTreeRows = 0, nMT = 0, nQH = 0, nT10 = 0;
        int nDGRaw = 0, nDSRaw = 0;

        while (true) {
            if (needUpdateData) {
                bool sortByNameInService = (currentTab == 0 && (sortCol[0] == 2 || sortCol[0] == 3) && sortState[0] == 1);
                service.getReadersList(bufDgArrRaw, nDGRaw, sortByNameInService);
                service.getBooksList(bufDsViewRaw, nDSRaw, false);

                if (currentTab == 0) filterAndSortDocGia(bufDgArrRaw, nDGRaw, bufDgArr, nDG, sortByNameInService);

                if (currentTab == 1) {
                    if (viewCategoryTree) buildCategoryTreeRows(bufTreeRows, nTreeRows);
                    else filterAndSortDauSach(bufDsViewRaw, nDSRaw, bufDsView, nDS);
                }

                if (currentTab == 2) {
                    int total = 0;
                    for (int i = 0; i < nDGRaw && total < MAX_MT_BUF; i++) {
                        NodeMuonTra* mt = bufDgArrRaw[i]->data.dsMuonTra;
                        while (mt && total < MAX_MT_BUF) {
                            if (mt->data.trangThai == DANG_MUON) {
                                NodeSach* ns = nullptr;
                                DauSach* ds = service.findDauSachByMaSach(mt->data.maSach, ns);
                                bufMtAll[total].maThe = bufDgArrRaw[i]->data.maThe;
                                sprintf(bufMtAll[total].hoTen, "%s %s", bufDgArrRaw[i]->data.ho, bufDgArrRaw[i]->data.ten);
                                strcpy(bufMtAll[total].maSach, mt->data.maSach);
                                strcpy(bufMtAll[total].tenSach, ds ? ds->tenSach : "N/A");
                                bufMtAll[total].ngayMuon = mt->data.ngayMuon;
                                bufMtAll[total].days = daysBetween(mt->data.ngayMuon, getCurrentDate());
                                total++;
                            }
                            mt = mt->next;
                        }
                    }
                    filterAndSortMuonTra(bufMtAll, total, bufMtArr, nMT);
                }

                if (currentTab == 3) {
                    int cnt = 0;
                    service.getOverdueList(bufQhRaw, cnt);
                    filterAndSortQuaHan(bufQhRaw, cnt, bufQhArr, nQH);
                }

                if (currentTab == 4) {
                    int cnt = 0;
                    service.getTop10Books(bufTopRaw, cnt);
                    for (int i = 0; i < cnt; i++) {
                        bufTrRaw[i].rank = i + 1;
                        bufTrRaw[i].ds = bufTopRaw[i];
                    }
                    filterTop10(bufTrRaw, cnt, bufTrArr, nT10);
                }

                needUpdateData = false;
            }

            drawTopDashboard(nDGRaw, nDSRaw);
            drawTabs();
            drawFieldAndSearchBars(isSearching);

            if (currentTab == 0) renderTabDocGia(bufDgArr, nDG);
            else if (currentTab == 1) renderTabDauSach(bufDsView, nDS, bufTreeRows, nTreeRows);
            else if (currentTab == 2) renderTabMuonTra(bufMtArr, nMT);
            else if (currentTab == 3) renderTabQuaHan(bufQhArr, nQH);
            else if (currentTab == 4) renderTabTop10(bufTrArr, nT10);

            screen.render();
            int ch = _getch();

            if (ch == 0 || ch == 224) {
                int ext = _getch();

                if (ext == KEY_UP) selectedRow--;
                else if (ext == KEY_DOWN) selectedRow++;
                else if (ext == KEY_PGUP) selectedRow -= 10;
                else if (ext == KEY_PGDN) selectedRow += 10;

                else if (ext == KEY_LEFT) {
                    if (!(currentTab == 1 && viewCategoryTree)) {
                        if (selectedCol[currentTab] > 0) selectedCol[currentTab]--;
                        else selectedCol[currentTab] = numFiltersForTab[currentTab] - 1;
                        if (strlen(searchKeyword[currentTab]) > 0) {
                            selectedRow = 0; scrollOffset = 0; needUpdateData = true;
                        }
                    }
                }
                else if (ext == KEY_RIGHT) {
                    if (!(currentTab == 1 && viewCategoryTree)) {
                        if (selectedCol[currentTab] < numFiltersForTab[currentTab] - 1) selectedCol[currentTab]++;
                        else selectedCol[currentTab] = 0;
                        if (strlen(searchKeyword[currentTab]) > 0) {
                            selectedRow = 0; scrollOffset = 0; needUpdateData = true;
                        }
                    }
                }

                if (!isSearching) {
                    if (currentTab == 0) {
                        if (ext == KEY_F1) { dialogAddOrEditDocGia(nullptr); needUpdateData = true; }
                        else if (ext == KEY_F2 && nDG > 0) { dialogAddOrEditDocGia(bufDgArr[selectedRow]); needUpdateData = true; }
                        else if (ext == KEY_F3 && nDG > 0) {
                            if (screen.showConfirm("Xac nhan xoa doc gia nay?")) {
                                bool ok = service.removeDocGia(bufDgArr[selectedRow]->data.maThe);
                                screen.showMessage(ok ? "Da xoa!" : "Loi: Doc gia dang muon sach!", !ok);
                                needUpdateData = true;
                            }
                        }
                        else if (ext == KEY_F4) {
                            if ((sortCol[0] == 2 || sortCol[0] == 3) && sortState[0] == 1) {
                                sortCol[0] = 0;
                                selectedCol[0] = 0;
                                sortState[0] = 1;
                            } else {
                                sortCol[0] = 2;
                                selectedCol[0] = 2;
                                sortState[0] = 1;
                            }
                            selectedRow = 0; scrollOffset = 0;
                            needUpdateData = true;
                        }
                    } else if (currentTab == 1) {
                        if (ext == KEY_F1) { dialogAddDauSach(); needUpdateData = true; }
                        else if (ext == KEY_F2) {
                            DauSach* targetDS = nullptr;
                            if (viewCategoryTree) { if (nTreeRows > 0 && !bufTreeRows[selectedRow].isCategory) targetDS = bufTreeRows[selectedRow].ds; }
                            else { if (nDS > 0) targetDS = bufDsView[selectedRow]; }
                            if (targetDS) { dialogEditDauSach(targetDS); needUpdateData = true; }
                        }
                        else if (ext == KEY_F3) {
                            DauSach* targetDS = nullptr;
                            if (viewCategoryTree) { if (nTreeRows > 0 && !bufTreeRows[selectedRow].isCategory) targetDS = bufTreeRows[selectedRow].ds; }
                            else { if (nDS > 0) targetDS = bufDsView[selectedRow]; }
                            if (targetDS) {
                                if (screen.showConfirm("Xac nhan XOA Dau sach nay va toan bo sach con?")) {
                                    if (service.removeDauSach(targetDS->ISBN)) { selectedRow = 0; scrollOffset = 0; needUpdateData = true; }
                                    else screen.showMessage("Loi: Dang co sach duoc muon, khong the xoa!", true);
                                }
                            }
                        }
                        else if (ext == KEY_F4) {
                            viewCategoryTree = !viewCategoryTree;
                            selectedRow = 0; scrollOffset = 0;
                            needUpdateData = true;
                        }
                        else if (ext == KEY_F5) dialogTraCuuMaSach();
                    } else if (currentTab == 2) {
                        if (ext == KEY_F1) { dialogBorrowOrReturn(0); needUpdateData = true; }
                        else if (ext == KEY_F2) { dialogBorrowOrReturn(1); needUpdateData = true; }
                        else if (ext == KEY_F3) { dialogBorrowOrReturn(2); needUpdateData = true; }
                    }
                }
                continue;
            }

            if (isSearching) {
                bool keywordChanged = false;
                if (ch == KEY_ENTER) isSearching = false;
                else if (ch == KEY_ESC) {
                    isSearching = false; strcpy(searchKeyword[currentTab], backupKeyword); keywordChanged = true;
                } else if (ch == 8) {
                    int len = (int)strlen(searchKeyword[currentTab]);
                    if (len > 0) { searchKeyword[currentTab][len - 1] = '\0'; keywordChanged = true; }
                } else if (ch >= 32 && ch <= 126) {
                    int len = (int)strlen(searchKeyword[currentTab]);
                    if (len < 38) {
                        bool isSpace = (ch == ' ');
                        bool lastIsSpace = (len > 0 && searchKeyword[currentTab][len - 1] == ' ');
                        // Chan: space dau tien HOAC 2 space lien tiep
                        if (isSpace && (len == 0 || lastIsSpace)) {
                            // Bo qua
                        } else {
                            searchKeyword[currentTab][len] = (char)ch;
                            searchKeyword[currentTab][len + 1] = '\0';
                            keywordChanged = true;
                        }
                    }
                }

                if (!(currentTab == 1 && viewCategoryTree)) {
                    if (keywordChanged) { selectedRow = 0; scrollOffset = 0; needUpdateData = true; }
                }
                continue;
            }

            if (ch == KEY_ESC) {
                int ans = screen.showExitConfirm();
                if (ans == 1) { service.saveToFile(); break; }
                else if (ans == 0) break;
                continue;
            }

            if (ch == KEY_TAB) {
                if (GetKeyState(VK_SHIFT) < 0) {
                    if (currentTab > 0) currentTab--; else currentTab = 4;
                } else {
                    if (currentTab < 4) currentTab++; else currentTab = 0;
                }
                selectedRow = 0; scrollOffset = 0;
                needUpdateData = true;
                continue;
            }

            if (ch == KEY_ENTER) {
                if (currentTab == 0) {
                    if (nDG > 0) { renderDialogChiTietDocGia(bufDgArr[selectedRow]); needUpdateData = true; }
                } else if (currentTab == 1) {
                    if (viewCategoryTree) {
                        if (nTreeRows > 0 && !bufTreeRows[selectedRow].isCategory && bufTreeRows[selectedRow].ds) {
                            renderDialogChiTietSach(bufTreeRows[selectedRow].ds); needUpdateData = true;
                        }
                    } else {
                        if (nDS > 0) { renderDialogChiTietSach(bufDsView[selectedRow]); needUpdateData = true; }
                    }
                }
            }

            if (ch == 'f' || ch == 'F') {
                if (!isSearching) {
                    if (currentTab == 1 && viewCategoryTree) screen.showMessage("Tinh nang tim kiem bi vo hieu hoa o che do nay!");
                    else { isSearching = true; strcpy(backupKeyword, searchKeyword[currentTab]); }
                }
                continue;
            }

            if (ch == 's' || ch == 'S') {
                if (currentTab == 4) {
                    screen.showMessage("Tab Top 10 luon co dinh theo thu hang luot muon!");
                } else if (currentTab == 1 && viewCategoryTree) {
                    screen.showMessage("Khong the sap xep o che do nay!");
                } else {
                    if (sortCol[currentTab] == selectedCol[currentTab]) {
                        sortState[currentTab] = (sortState[currentTab] == 1) ? 2 : 1;
                    } else {
                        sortCol[currentTab] = selectedCol[currentTab];
                        sortState[currentTab] = 1;
                    }
                    selectedRow = 0; scrollOffset = 0;
                    needUpdateData = true;
                }
                continue;
            }
        }
    }
};

#endif