#ifndef UI_H
#define UI_H

#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <cstring>
#include "logic.h"

#define SCREEN_W 122
#define SCREEN_H 30

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

#define C_APP_BG        7
#define C_PANEL_BG      8
#define C_BORDER        11
#define C_HEADER        14
#define C_ROW_NORM      7
#define C_ROW_SEL       240
#define C_TAB_ACTIVE    15
#define C_TAB_INACTIVE  8
#define C_BADGE_OK      10
#define C_BADGE_WARN    14
#define C_BADGE_ERR     12
#define C_KEYCAP        11
#define C_DLG_BG        112
#define C_INPUT_BG      15
#define C_INPUT_FOCUS   207

#define C_CATE_HEADER     (14 | (C_APP_BG & 0xF0))
#define C_CATE_HEADER_SEL (0 | (C_ROW_SEL & 0xF0))

#define B_TL (char)218
#define B_TR (char)191
#define B_BL (char)192
#define B_BR (char)217
#define B_H  (char)196
#define B_V  (char)179
#define B_T  (char)194
#define B_B  (char)193
#define B_L  (char)195
#define B_R  (char)180
#define B_C  (char)197
#define B_BLOCK (char)219

class ConsoleEngine {
private:
    HANDLE hOut;
    CHAR_INFO buffer[SCREEN_W * SCREEN_H];

public:
    void init(const char* windowTitle) {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        SetConsoleTitleA(windowTitle);
        hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(437);

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
            buffer[i].Char.AsciiChar = ' ';
            buffer[i].Attributes = attr;
        }
    }

    void putChar(int x, int y, char c, WORD attr) {
        if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
            buffer[y * SCREEN_W + x].Char.AsciiChar = c;
            buffer[y * SCREEN_W + x].Attributes = attr;
        }
    }

    char getChar(int x, int y) {
        if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
            return buffer[y * SCREEN_W + x].Char.AsciiChar;
        }
        return ' ';
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
            char ch = (i < len) ? str[i] : ' ';
            if (i == width - 1 && len > width) ch = '~';
            putChar(x + i, y, ch, attr);
        }
    }

    void fillRect(int x, int y, int w, int h, WORD attr) {
        for (int r = y; r < y + h && r < SCREEN_H; r++) {
            for (int c = x; c < x + w && c < SCREEN_W; c++) {
                if (r >= 0 && c >= 0) putChar(c, r, ' ', attr);
            }
        }
    }

    void drawBox(int x, int y, int w, int h, WORD fillAttr, WORD borderAttr, const char* title = "") {
        for (int i = 0; i <= h; i++) {
            for (int j = 0; j <= w; j++) {
                char c = ' ';
                if (i == 0 && j == 0) c = B_TL;
                else if (i == 0 && j == w) c = B_TR;
                else if (i == h && j == 0) c = B_BL;
                else if (i == h && j == w) c = B_BR;
                else if (i == 0 || i == h) c = B_H;
                else if (j == 0 || j == w) c = B_V;
                putChar(x + j, y + i, c, borderAttr);
                if (i > 0 && i < h && j > 0 && j < w) putChar(x + j, y + i, ' ', fillAttr);
            }
        }
        if (title && strlen(title) > 0) {
            char buf[100];
            sprintf(buf, " %s ", title);
            writeStr(x + 2, y, buf, borderAttr);
        }
    }

    void drawTableGrid(int x, int y, int w, int h, const int colX[], int numCols, const char* headers[], int totalItems, int selectedIdx) {
        for (int i = 0; i <= h; i++) {
            for (int j = 0; j <= w; j++) {
                char c = ' ';
                if (i == 0 && j == 0) c = B_TL;
                else if (i == 0 && j == w) c = B_TR;
                else if (i == h && j == 0) c = B_BL;
                else if (i == h && j == w) c = B_BR;
                else if (i == 0 || i == h) c = B_H;
                else if (j == 0 || j == w) c = B_V;
                putChar(x + j, y + i, c, C_BORDER);
                if (i > 0 && i < h && j > 0 && j < w) putChar(x + j, y + i, ' ', C_APP_BG);
            }
        }

        putChar(x, y + 2, B_L, C_BORDER);
        for (int j = 1; j < w; j++) putChar(x + j, y + 2, B_H, C_BORDER);
        putChar(x + w, y + 2, B_R, C_BORDER);

        for (int c = 0; c < numCols - 1; c++) {
            int cx = colX[c + 1] - 1;
            putChar(cx, y,     B_T, C_BORDER);
            putChar(cx, y + 1, B_V, C_BORDER);
            putChar(cx, y + 2, B_C, C_BORDER);
            for (int r = y + 3; r < y + h; r++) putChar(cx, r, B_V, C_BORDER);
            putChar(cx, y + h, B_B, C_BORDER);
        }

        for (int c = 0; c < numCols; c++) {
            writeStr(colX[c] + 1, y + 1, headers[c], C_HEADER);
        }

        char pageInfo[40];
        sprintf(pageInfo, " Dong %d/%d ", totalItems > 0 ? selectedIdx + 1 : 0, totalItems);
        writeStr(x + w - (int)strlen(pageInfo) - 2, y + h, pageInfo, C_BORDER);
    }

    WORD prepareTableRow(int x, int y, int w, int rowIdx, const int colX[], int numCols, bool isSelected) {
        WORD rowAttr = isSelected ? C_ROW_SEL : C_ROW_NORM;
        WORD sepAttr = isSelected ? C_ROW_SEL : C_BORDER;
        fillRect(x + 1, y, w - 1, 1, rowAttr);
        for (int c = 0; c < numCols - 1; c++) {
            putChar(colX[c + 1] - 1, y, B_V, sepAttr);
        }
        return rowAttr;
    }

    void render() {
        COORD bufSize = { SCREEN_W, SCREEN_H };
        COORD bufCoord = { 0, 0 };
        SMALL_RECT writeArea = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
        WriteConsoleOutputA(hOut, buffer, bufSize, bufCoord, &writeArea);
    }

    void showMessage(const char* msg, bool isError = false) {
        drawBox(30, 11, 60, 6, C_DLG_BG, C_DLG_BG, isError ? "[ L O I ]" : "[ THONG BAO ]");
        writeStr(33, 13, msg, C_DLG_BG);
        writeStr(42, 15, " [ ENTER / ESC de dong ] ", C_DLG_BG);
        render();
        _getch();
    }

    bool showConfirm(const char* msg) {
        drawBox(30, 11, 60, 6, C_DLG_BG, C_DLG_BG, "[ XAC NHAN ]");
        writeStr(33, 13, msg, C_DLG_BG);
        writeStr(35, 15, " [ ENTER/Y: Co  |  ESC/N: Khong ] ", C_DLG_BG);
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

        drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BG, "[ THOAT CHUONG TRINH ]");
        writeStr(startX + 10, startY + 2, "Ban co muon LUU DU LIEU truoc khi thoat chuong trinh?", C_DLG_BG);
        writeStr(startX + 10, startY + 4, " [ ENTER/Y: Luu & Thoat | N: Thoat luon | ESC: Huy ] ", C_DLG_BG);
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
            drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BG, title);
            int curY = startY + 2;

            if (readOnlyInfo) {
                writeStr(startX + 3, curY, readOnlyInfo, C_DLG_BG);
                curY += 2;
            }

            for (int i = 0; i < numFields; i++) {
                bool isFocus = (i == curField);
                bool isToggle = (toggleOpts && toggleOpts[i] && strlen(toggleOpts[i]) > 0);
                writeStr(startX + 3, curY, labels[i], C_DLG_BG);

                int inpX = startX + 30;
                int inpW = boxW - 33;
                WORD inpCol = isFocus ? C_INPUT_FOCUS : C_INPUT_BG;

                fillRect(inpX, curY, inpW, 1, inpCol);

                if (isToggle) {
                    char displayVal[60];
                    if (isFocus) sprintf(displayVal, "< %s >", values[i]);
                    else sprintf(displayVal, "  %s  ", values[i]);
                    writeStr(inpX + 1, curY, displayVal, inpCol);
                } else {
                    writeStr(inpX + 1, curY, values[i], inpCol);
                }
                curY += 2;
            }

            bool hasToggle = false;
            if (toggleOpts) {
                for (int i = 0; i < numFields; i++) if (toggleOpts[i] && strlen(toggleOpts[i]) > 0) hasToggle = true;
            }
            if (hasToggle) writeStr(startX + 2, startY + boxH - 2, "[Up/Down]: Chon  [<-/->]: Doi G.Tri  [Enter]: Luu", C_DLG_BG);
            else writeStr(startX + 3, startY + boxH - 2, "[Up/Down]: Chon  [Enter]: Luu  [ESC]: Huy", C_DLG_BG);

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
                        values[curField][len] = (char)ch;
                        values[curField][len + 1] = '\0';
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

    int selectedCol[5] = {0, 0, 0, 0, 0};
    int sortState[5] = {0, 0, 0, 0, 0};
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
        return strstr(t, k) != nullptr;
    }

    void buildHeaders(const char* raw[], int numCols, int tab, char outBuf[][50], const char* outPtr[]) {
        for (int i = 0; i < numCols; i++) {
            strcpy(outBuf[i], raw[i]);
            outPtr[i] = outBuf[i];
        }
    }

    void drawTopDashboard(int totalDG, int totalDS) {
        screen.clear();

        screen.writeStr(2, 0, "[ HE THONG QUAN LY THU VIEN ]", C_HEADER);
        Date d = getCurrentDate();
        char dateBuf[40];
        sprintf(dateBuf, "Ngay: %02d/%02d/%04d", d.d, d.m, d.y);
        screen.writeStr(SCREEN_W - 20, 0, dateBuf, C_HEADER);

        char metricBuf[100];
        sprintf(metricBuf, " Doc gia: %d | Dau sach: %d ", totalDG, totalDS);
        screen.writeStr(SCREEN_W - (int)strlen(metricBuf) - 1, 0, metricBuf, C_BORDER);

        screen.fillRect(0, SCREEN_H - 1, SCREEN_W, 1, C_APP_BG);
        if (currentTab == 0) {
            screen.writeStr(1, SCREEN_H - 1, "ENTER:Chi tiet  F1:Them  F2:Sua  F3:Xoa  |  <- ->:Chon Tab  F:Tim  S:Sort  ESC:Thoat", C_KEYCAP);
        } else if (currentTab == 1) {
            if (viewCategoryTree) {
                screen.writeStr(1, SCREEN_H - 1, "ENTER:Chi tiet  F1:Them  F2:Sua  F3:Xoa  F4:Xem thuong  ESC:Thoat", C_KEYCAP);
            } else {
                screen.writeStr(1, SCREEN_H - 1, "ENTER:Chi tiet  F1:Them  F2:Sua  F3:Xoa  F4:Theo the loai  F5:Tim Ma", C_KEYCAP);
            }
        } else if (currentTab == 2) {
            screen.writeStr(1, SCREEN_H - 1, "F1:Muon  F2:Tra  F3:Mat Sach  |  <- ->:Chon Tab  F:Tim  S:Sort  ESC:Thoat", C_KEYCAP);
        } else {
            screen.writeStr(1, SCREEN_H - 1, "<- ->:Chon Tab  F:Tim  S:Sort  ESC:Thoat", C_KEYCAP);
        }
    }

    void drawTabs() {
        const char* tabs[5] = { " 1. DOC GIA ", " 2. DAU SACH ", " 3. MUON/TRA ", " 4. QUA HAN ", " 5. TOP 10 " };
        int xPos = 2;
        int tableY = 3;

        for (int j = 1; j < SCREEN_W - 1; j++) screen.putChar(j, tableY, B_H, C_BORDER);

        for (int i = 0; i < 5; i++) {
            int len = strlen(tabs[i]);
            if (i == currentTab) {
                screen.putChar(xPos, tableY - 2, B_TL, C_BORDER);
                for (int j = 0; j < len; j++) screen.putChar(xPos + 1 + j, tableY - 2, B_H, C_BORDER);
                screen.putChar(xPos + len + 1, tableY - 2, B_TR, C_BORDER);

                screen.putChar(xPos, tableY - 1, B_V, C_BORDER);
                screen.writeStr(xPos + 1, tableY - 1, tabs[i], C_TAB_ACTIVE);
                screen.putChar(xPos + len + 1, tableY - 1, B_V, C_BORDER);

                screen.putChar(xPos, tableY, B_B, C_BORDER);
                screen.putChar(xPos + len + 1, tableY, B_B, C_BORDER);
                for (int j = 0; j < len; j++) screen.putChar(xPos + 1 + j, tableY, ' ', C_APP_BG);
            } else {
                screen.writeStr(xPos + 1, tableY - 1, tabs[i], C_TAB_INACTIVE);
            }
            xPos += len + 2;
        }
    }

    void drawSearchBar(bool isTyping) {
        const char* filters0[] = {"MA THE", "HO VA TEN", "HO VA TEN DEM", "TEN", "PHAI", "TRANG THAI", "DANG MUON"};
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

        int fIdx = selectedCol[currentTab];
        const char* filterName = currentFilters[fIdx];

        const char* sState = (sortState[currentTab] == 1) ? "^" : ((sortState[currentTab] == 2) ? "v" : "-");

        char prompt[150];
        if (currentTab == 1 && viewCategoryTree) {
            sprintf(prompt, " [ ViewMode ]");
        } else {
            sprintf(prompt, " [ %s | Sort: %s ] [TAB]: Doi Truong [F]: Tim ", filterName, sState);
        }

        screen.writeStr(2, 4, prompt, isTyping ? C_HEADER : C_TAB_INACTIVE);

        if (!(currentTab == 1 && viewCategoryTree)) {
            int inputX = 2 + strlen(prompt);
            int inputW = 35;
            WORD attr = isTyping ? C_INPUT_FOCUS : C_INPUT_BG;

            screen.fillRect(inputX, 4, inputW, 1, attr);
            if (strlen(searchKeyword[currentTab]) == 0) {
                screen.writeStr(inputX + 1, 4, isTyping ? "" : "<Nhap tu khoa...>", isTyping ? attr : 8 | (attr & 0xF0));
            } else {
                screen.writeStr(inputX + 1, 4, searchKeyword[currentTab], attr);
            }

            if (isTyping) {
                screen.putChar(inputX + 1 + strlen(searchKeyword[currentTab]), 4, '_', attr);
                screen.writeStr(inputX + inputW + 2, 4, " [ENTER]: Xong [ESC]: Huy ", C_BADGE_WARN);
            }
        }
    }

    void filterAndSortDocGia(NodeDocGia* rawArr[], int nRaw, NodeDocGia* arr[], int& nOut) {
        nOut = 0;
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            int c = selectedCol[0];

            int b = 0; bool ov = false;
            LibraryService::checkBorrowStatus(rawArr[i]->data, b, ov);

            if (c == 0) sprintf(val, "%d", rawArr[i]->data.maThe);
            else if (c == 1) sprintf(val, "%s %s", rawArr[i]->data.ho, rawArr[i]->data.ten);
            else if (c == 2) strcpy(val, rawArr[i]->data.ho);
            else if (c == 3) strcpy(val, rawArr[i]->data.ten);
            else if (c == 4) strcpy(val, rawArr[i]->data.phai);
            else if (c == 5) strcpy(val, rawArr[i]->data.trangThai == HOAT_DONG ? "Hoat dong" : "Bi khoa");
            else if (c == 6) sprintf(val, "%d", b);

            if (matchKeyword(val, searchKeyword[0])) arr[nOut++] = rawArr[i];
        }

        if (sortState[0] != 0) {
            for (int i = 0; i < nOut - 1; i++) {
                for (int j = i + 1; j < nOut; j++) {
                    bool swap = false;
                    int c = selectedCol[0];
                    if (c == 0) {
                        if (sortState[0] == 1 && arr[i]->data.maThe > arr[j]->data.maThe) swap = true;
                        if (sortState[0] == 2 && arr[i]->data.maThe < arr[j]->data.maThe) swap = true;
                    } else if (c == 1 || c == 3) {
                        int cmp = _stricmp(arr[i]->data.ten, arr[j]->data.ten);
                        if (cmp == 0) cmp = _stricmp(arr[i]->data.ho, arr[j]->data.ho);
                        if (sortState[0] == 1 && cmp > 0) swap = true;
                        if (sortState[0] == 2 && cmp < 0) swap = true;
                    } else if (c == 2) {
                        int cmp = _stricmp(arr[i]->data.ho, arr[j]->data.ho);
                        if (cmp == 0) cmp = _stricmp(arr[i]->data.ten, arr[j]->data.ten);
                        if (sortState[0] == 1 && cmp > 0) swap = true;
                        if (sortState[0] == 2 && cmp < 0) swap = true;
                    } else if (c == 4) {
                        int cmp = _stricmp(arr[i]->data.phai, arr[j]->data.phai);
                        if (sortState[0] == 1 && cmp > 0) swap = true;
                        if (sortState[0] == 2 && cmp < 0) swap = true;
                    } else if (c == 5) {
                        if (sortState[0] == 1 && arr[i]->data.trangThai > arr[j]->data.trangThai) swap = true;
                        if (sortState[0] == 2 && arr[i]->data.trangThai < arr[j]->data.trangThai) swap = true;
                    } else if (c == 6) {
                        int b1 = 0, b2 = 0; bool ov1, ov2;
                        LibraryService::checkBorrowStatus(arr[i]->data, b1, ov1);
                        LibraryService::checkBorrowStatus(arr[j]->data, b2, ov2);
                        if (sortState[0] == 1 && b1 > b2) swap = true;
                        if (sortState[0] == 2 && b1 < b2) swap = true;
                    }
                    if (swap) { NodeDocGia* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
                }
            }
        }
    }

    void renderTabDocGia(NodeDocGia* arr[], int nDG) {
        int visibleRows = 20;
        adjustViewport(nDG, visibleRows);

        const int colX[6] = { 2, 15, 48, 68, 80, 100 };
        const int colW[6] = { 11, 31, 18, 10, 18, 19 };

        char hBuf[6][50]; const char* headers[6];
        const char* rawHeaders[6] = { "MA THE", "HO VA TEN DEM", "TEN", "PHAI", "TRANG THAI", "DANG MUON" };
        buildHeaders(rawHeaders, 6, 0, hBuf, headers);

        screen.drawTableGrid(1, 5, 119, 23, colX, 6, headers, nDG, selectedRow);

        for (int i = 0; i < visibleRows && (scrollOffset + i) < nDG; i++) {
            int idx = scrollOffset + i;
            int rowY = 8 + i;
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
            screen.writeCell(colX[5] + 1, rowY, colW[5], sMuon, isSel ? C_ROW_SEL : (ov ? C_BADGE_ERR : C_ROW_NORM));
        }
    }

    void filterAndSortDauSach(DauSach* rawArr[], int nRaw, DauSach* arr[], int& nOut) {
        nOut = 0;
        for (int i = 0; i < nRaw; i++) {
            char val[100] = "";
            int c = selectedCol[1];
            if (c == 0) strcpy(val, rawArr[i]->ISBN);
            else if (c == 1) strcpy(val, rawArr[i]->tenSach);
            else if (c == 2) strcpy(val, rawArr[i]->tacGia);
            else if (c == 3) strcpy(val, rawArr[i]->theLoai);
            else if (c == 4) sprintf(val, "%d", rawArr[i]->namXB);
            else if (c == 5) sprintf(val, "%d", rawArr[i]->soLuongSach);

            if (matchKeyword(val, searchKeyword[1])) arr[nOut++] = rawArr[i];
        }
        if (sortState[1] != 0) {
            for (int i = 0; i < nOut - 1; i++) {
                for (int j = i + 1; j < nOut; j++) {
                    bool swap = false;
                    int cmp = 0;
                    int c = selectedCol[1];
                    if (c == 0) cmp = _stricmp(arr[i]->ISBN, arr[j]->ISBN);
                    else if (c == 1) cmp = _stricmp(arr[i]->tenSach, arr[j]->tenSach);
                    else if (c == 2) cmp = _stricmp(arr[i]->tacGia, arr[j]->tacGia);
                    else if (c == 3) cmp = _stricmp(arr[i]->theLoai, arr[j]->theLoai);
                    else if (c == 4) {
                        if (sortState[1] == 1 && arr[i]->namXB > arr[j]->namXB) swap = true;
                        if (sortState[1] == 2 && arr[i]->namXB < arr[j]->namXB) swap = true;
                    }
                    else if (c == 5) {
                        if (sortState[1] == 1 && arr[i]->soLuongSach > arr[j]->soLuongSach) swap = true;
                        if (sortState[1] == 2 && arr[i]->soLuongSach < arr[j]->soLuongSach) swap = true;
                    }
                    if (c != 4 && c != 5) {
                        if (sortState[1] == 1 && cmp > 0) swap = true;
                        if (sortState[1] == 2 && cmp < 0) swap = true;
                    }
                    if (swap) { DauSach* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
                }
            }
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

            const int colX[5] = { 2, 16, 68, 98, 109 };
            const int colW[5] = { 12, 50, 28, 9, 10 };
            const char* headers[5] = { "ISBN", "THE LOAI / TEN DAU SACH", "TAC GIA", "NAM XB", "SO SACH" };

            screen.drawTableGrid(1, 5, 119, 23, colX, 5, headers, nTreeRows, selectedRow);

            for (int i = 0; i < visibleRows && (scrollOffset + i) < nTreeRows; i++) {
                int idx = scrollOffset + i;
                int rowY = 8 + i;
                bool isSel = (idx == selectedRow);
                WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 5, isSel);

                DisplayRowDS& item = treeRows[idx];

                if (item.isCategory) {
                    WORD cateAttr = isSel ? C_CATE_HEADER_SEL : C_CATE_HEADER;

                    char catTitle[100];
                    sprintf(catTitle, "[+] %s (%d)", item.categoryName, item.countInCate);

                    screen.writeCell(colX[0] + 1, rowY, colW[0], "------", isSel ? C_ROW_SEL : C_BORDER);
                    screen.writeCell(colX[1] + 1, rowY, colW[1], catTitle, cateAttr);
                    screen.writeCell(colX[2] + 1, rowY, colW[2], "---------------------------", rowAttr);
                    screen.writeCell(colX[3] + 1, rowY, colW[3], "--------", rowAttr);
                    screen.writeCell(colX[4] + 1, rowY, colW[4], "--------", rowAttr);

                    WORD sepAttr = isSel ? C_ROW_SEL : C_BORDER;
                    for (int c = 0; c < 4; c++) {
                        screen.putChar(colX[c + 1] - 1, rowY, B_V, sepAttr);
                    }
                } else {
                    char branch[150];
                    if (item.isLastInCategory) {
                        sprintf(branch, "  %c%c %s", B_BL, B_H, item.ds->tenSach);
                    } else {
                        sprintf(branch, "  %c%c %s", B_L, B_H, item.ds->tenSach);
                    }

                    char sNam[10], sQty[10];
                    sprintf(sNam, "%d", item.ds->namXB);
                    sprintf(sQty, "%d", item.ds->soLuongSach);

                    screen.writeCell(colX[0] + 1, rowY, colW[0], item.ds->ISBN, rowAttr);
                    screen.writeCell(colX[1] + 1, rowY, colW[1], branch, rowAttr);
                    screen.writeCell(colX[2] + 1, rowY, colW[2], item.ds->tacGia, rowAttr);
                    screen.writeCell(colX[3] + 1, rowY, colW[3], sNam, rowAttr);
                    screen.writeCell(colX[4] + 1, rowY, colW[4], sQty, rowAttr);
                }
            }
        } else {
            adjustViewport(nDS, visibleRows);

            const int colX[6] = { 2, 16, 52, 75, 96, 107 };
            const int colW[6] = { 12, 34, 21, 19, 9, 11 };

            char hBuf[6][50]; const char* headers[6];
            const char* rawHeaders[6] = { "ISBN", "TEN SACH", "TAC GIA", "THE LOAI", "NAM XB", "SO SACH" };
            buildHeaders(rawHeaders, 6, 1, hBuf, headers);

            screen.drawTableGrid(1, 5, 119, 23, colX, 6, headers, nDS, selectedRow);

            for (int i = 0; i < visibleRows && (scrollOffset + i) < nDS; i++) {
                int idx = scrollOffset + i; int rowY = 8 + i;
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
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            int c = selectedCol[2];
            char dStr[20]; sprintf(dStr, "%02d/%02d/%04d", rawArr[i].ngayMuon.d, rawArr[i].ngayMuon.m, rawArr[i].ngayMuon.y);
            const char* tt = rawArr[i].days > 7 ? "! QUA HAN" : "Dang muon";

            if (c == 0) sprintf(val, "%d", rawArr[i].maThe);
            else if (c == 1) strcpy(val, rawArr[i].hoTen);
            else if (c == 2) strcpy(val, rawArr[i].maSach);
            else if (c == 3) strcpy(val, rawArr[i].tenSach);
            else if (c == 4) strcpy(val, dStr);
            else if (c == 5) strcpy(val, tt);

            if (matchKeyword(val, searchKeyword[2])) arr[nOut++] = rawArr[i];
        }
        if (sortState[2] != 0) {
            for (int i = 0; i < nOut - 1; i++) {
                for (int j = i + 1; j < nOut; j++) {
                    bool swap = false;
                    int c = selectedCol[2];
                    if (c == 0) {
                        if (sortState[2] == 1 && arr[i].maThe > arr[j].maThe) swap = true;
                        if (sortState[2] == 2 && arr[i].maThe < arr[j].maThe) swap = true;
                    } else if (c == 1) {
                        int cmp = _stricmp(arr[i].hoTen, arr[j].hoTen);
                        if (sortState[2] == 1 && cmp > 0) swap = true;
                        if (sortState[2] == 2 && cmp < 0) swap = true;
                    } else if (c == 2) {
                        int cmp = _stricmp(arr[i].maSach, arr[j].maSach);
                        if (sortState[2] == 1 && cmp > 0) swap = true;
                        if (sortState[2] == 2 && cmp < 0) swap = true;
                    } else if (c == 3) {
                        int cmp = _stricmp(arr[i].tenSach, arr[j].tenSach);
                        if (sortState[2] == 1 && cmp > 0) swap = true;
                        if (sortState[2] == 2 && cmp < 0) swap = true;
                    } else if (c == 4) {
                        long d1 = dateToDays(arr[i].ngayMuon); long d2 = dateToDays(arr[j].ngayMuon);
                        if (sortState[2] == 1 && d1 > d2) swap = true;
                        if (sortState[2] == 2 && d1 < d2) swap = true;
                    } else if (c == 5) {
                        if (sortState[2] == 1 && arr[i].days > arr[j].days) swap = true;
                        if (sortState[2] == 2 && arr[i].days < arr[j].days) swap = true;
                    }
                    if (swap) { ActiveBorrow tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
                }
            }
        }
    }

    void renderTabMuonTra(ActiveBorrow arr[], int nMT) {
        int visibleRows = 20; adjustViewport(nMT, visibleRows);

        const int colX[6] = { 2, 13, 39, 55, 88, 102 };
        const int colW[6] = { 9, 24, 14, 31, 12, 16 };

        char hBuf[6][50]; const char* headers[6];
        const char* rawHeaders[6] = { "MA THE", "HO TEN", "MA SACH", "TEN SACH", "NGAY MUON", "TINH TRANG" };
        buildHeaders(rawHeaders, 6, 2, hBuf, headers);

        screen.drawTableGrid(1, 5, 119, 23, colX, 6, headers, nMT, selectedRow);

        for (int i = 0; i < visibleRows && (scrollOffset + i) < nMT; i++) {
            int idx = scrollOffset + i; int rowY = 8 + i;
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
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            int c = selectedCol[3];
            char dStr[20]; sprintf(dStr, "%02d/%02d/%04d", rawArr[i].ngayMuon.d, rawArr[i].ngayMuon.m, rawArr[i].ngayMuon.y);

            if (c == 0) sprintf(val, "%d", rawArr[i].maThe);
            else if (c == 1) strcpy(val, rawArr[i].hoTen);
            else if (c == 2) strcpy(val, rawArr[i].maSach);
            else if (c == 3) strcpy(val, rawArr[i].tenSach);
            else if (c == 4) strcpy(val, dStr);
            else if (c == 5) sprintf(val, "%d", rawArr[i].soNgayQuaHan);

            if (matchKeyword(val, searchKeyword[3])) arr[nOut++] = rawArr[i];
        }
        if (sortState[3] != 0) {
            for (int i = 0; i < nOut - 1; i++) {
                for (int j = i + 1; j < nOut; j++) {
                    bool swap = false;
                    int c = selectedCol[3];
                    if (c == 0) {
                        if (sortState[3] == 1 && arr[i].maThe > arr[j].maThe) swap = true;
                        if (sortState[3] == 2 && arr[i].maThe < arr[j].maThe) swap = true;
                    } else if (c == 1) {
                        int cmp = _stricmp(arr[i].hoTen, arr[j].hoTen);
                        if (sortState[3] == 1 && cmp > 0) swap = true;
                        if (sortState[3] == 2 && cmp < 0) swap = true;
                    } else if (c == 2) {
                        int cmp = _stricmp(arr[i].maSach, arr[j].maSach);
                        if (sortState[3] == 1 && cmp > 0) swap = true;
                        if (sortState[3] == 2 && cmp < 0) swap = true;
                    } else if (c == 3) {
                        int cmp = _stricmp(arr[i].tenSach, arr[j].tenSach);
                        if (sortState[3] == 1 && cmp > 0) swap = true;
                        if (sortState[3] == 2 && cmp < 0) swap = true;
                    } else if (c == 4) {
                        long d1 = dateToDays(arr[i].ngayMuon); long d2 = dateToDays(arr[j].ngayMuon);
                        if (sortState[3] == 1 && d1 > d2) swap = true;
                        if (sortState[3] == 2 && d1 < d2) swap = true;
                    } else if (c == 5) {
                        if (sortState[3] == 1 && arr[i].soNgayQuaHan > arr[j].soNgayQuaHan) swap = true;
                        if (sortState[3] == 2 && arr[i].soNgayQuaHan < arr[j].soNgayQuaHan) swap = true;
                    }
                    if (swap) { OverdueRecord tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
                }
            }
        }
    }

    void renderTabQuaHan(OverdueRecord arr[], int nQH) {
        int visibleRows = 20; adjustViewport(nQH, visibleRows);
        const int colX[6] = { 2, 13, 39, 55, 88, 102 };
        const int colW[6] = { 9, 24, 14, 31, 12, 16 };

        char hBuf[6][50]; const char* headers[6];
        const char* rawHeaders[6] = { "MA THE", "HO TEN", "MA SACH", "TEN SACH", "NGAY MUON", "QUA HAN" };
        buildHeaders(rawHeaders, 6, 3, hBuf, headers);

        screen.drawTableGrid(1, 5, 119, 23, colX, 6, headers, nQH, selectedRow);

        for (int i = 0; i < visibleRows && (scrollOffset + i) < nQH; i++) {
            int idx = scrollOffset + i; int rowY = 8 + i;
            bool isSel = (idx == selectedRow); WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 6, isSel);
            char sMa[15], dStr[20], sOver[20];
            sprintf(sMa, "%d", arr[idx].maThe); sprintf(dStr, "%02d/%02d/%04d", arr[idx].ngayMuon.d, arr[idx].ngayMuon.m, arr[idx].ngayMuon.y);
            sprintf(sOver, "%d ngay", arr[idx].soNgayQuaHan);
            screen.writeCell(colX[0] + 1, rowY, colW[0], sMa, rowAttr); screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx].hoTen, rowAttr);
            screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx].maSach, rowAttr); screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx].tenSach, rowAttr);
            screen.writeCell(colX[4] + 1, rowY, colW[4], dStr, rowAttr); screen.writeCell(colX[5] + 1, rowY, colW[5], sOver, isSel ? C_ROW_SEL : C_BADGE_ERR);
        }
    }

    void filterAndSortTop10(TopRecord rawArr[], int nRaw, TopRecord arr[], int& nOut) {
        nOut = 0;
        for (int i = 0; i < nRaw; i++) {
            char val[300] = "";
            int c = selectedCol[4];
            if (c == 0) sprintf(val, "#%d", rawArr[i].rank);
            else if (c == 1) strcpy(val, rawArr[i].ds->ISBN);
            else if (c == 2) strcpy(val, rawArr[i].ds->tenSach);
            else if (c == 3) strcpy(val, rawArr[i].ds->tacGia);
            else if (c == 4) sprintf(val, "%d", rawArr[i].ds->soLuotMuon);

            if (matchKeyword(val, searchKeyword[4])) arr[nOut++] = rawArr[i];
        }
        if (sortState[4] != 0) {
            for (int i = 0; i < nOut - 1; i++) {
                for (int j = i + 1; j < nOut; j++) {
                    bool swap = false;
                    int c = selectedCol[4];
                    if (c == 0) {
                        if (sortState[4] == 1 && arr[i].rank > arr[j].rank) swap = true;
                        if (sortState[4] == 2 && arr[i].rank < arr[j].rank) swap = true;
                    } else if (c == 1) {
                        int cmp = _stricmp(arr[i].ds->ISBN, arr[j].ds->ISBN);
                        if (sortState[4] == 1 && cmp > 0) swap = true;
                        if (sortState[4] == 2 && cmp < 0) swap = true;
                    } else if (c == 2) {
                        int cmp = _stricmp(arr[i].ds->tenSach, arr[j].ds->tenSach);
                        if (sortState[4] == 1 && cmp > 0) swap = true;
                        if (sortState[4] == 2 && cmp < 0) swap = true;
                    } else if (c == 3) {
                        int cmp = _stricmp(arr[i].ds->tacGia, arr[j].ds->tacGia);
                        if (sortState[4] == 1 && cmp > 0) swap = true;
                        if (sortState[4] == 2 && cmp < 0) swap = true;
                    } else if (c == 4) {
                        if (sortState[4] == 1 && arr[i].ds->soLuotMuon > arr[j].ds->soLuotMuon) swap = true;
                        if (sortState[4] == 2 && arr[i].ds->soLuotMuon < arr[j].ds->soLuotMuon) swap = true;
                    }
                    if (swap) { TopRecord tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; }
                }
            }
        }
    }

    void renderTabTop10(TopRecord arr[], int nT10) {
        int visibleRows = 20; adjustViewport(nT10, visibleRows);
        const int colX[5] = { 2, 11, 27, 67, 96 };
        const int colW[5] = { 7, 14, 38, 27, 22 };

        char hBuf[5][50]; const char* headers[5];
        const char* rawHeaders[5] = { "HANG", "ISBN", "TEN DAU SACH", "TAC GIA", "LUOT MUON" };
        buildHeaders(rawHeaders, 5, 4, hBuf, headers);

        screen.drawTableGrid(1, 5, 119, 23, colX, 5, headers, nT10, selectedRow);

        for (int i = 0; i < visibleRows && (scrollOffset + i) < nT10; i++) {
            int idx = scrollOffset + i; int rowY = 8 + i;
            bool isSel = (idx == selectedRow); WORD rowAttr = screen.prepareTableRow(1, rowY, 119, idx, colX, 5, isSel);
            char sRank[10], sBorrows[25]; sprintf(sRank, "#%d", arr[idx].rank); sprintf(sBorrows, "%d luot", arr[idx].ds->soLuotMuon);
            screen.writeCell(colX[0] + 1, rowY, colW[0], sRank, isSel ? C_ROW_SEL : C_HEADER);
            screen.writeCell(colX[1] + 1, rowY, colW[1], arr[idx].ds->ISBN, rowAttr);
            screen.writeCell(colX[2] + 1, rowY, colW[2], arr[idx].ds->tenSach, rowAttr);
            screen.writeCell(colX[3] + 1, rowY, colW[3], arr[idx].ds->tacGia, rowAttr);
            screen.writeCell(colX[4] + 1, rowY, colW[4], sBorrows, isSel ? C_ROW_SEL : C_BADGE_OK);
        }
    }

    void dialogAddOrEditDocGia(NodeDocGia* editNode = nullptr) {
        const char* labels[4] = { "Ho va ten dem :", "Ten doc gia   :", "Phai (Nam/Nu) :", "Trang thai    :" };
        char values[4][50] = { "", "", "Nam", "Hoat dong" };
        const int maxLens[4] = { 35, 18, 5, 20 };
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
        const int maxLens[5] = { 12, 40, 30, 25, 6 };

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
                delete ds;
                screen.showMessage("Loi: Trung ma ISBN!");
                return;
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

        char info[60];
        sprintf(info, "Ma ISBN: %s (Khong duoc phep sua)", ds->ISBN);

        if (screen.showFormDialog("HIEU CHINH DAU SACH", labels, values, maxLens, 5, info)) {
            if (service.updateDauSach(ds->ISBN, values[0], values[1], values[2], atoi(values[3]), atoi(values[4]))) {
                screen.showMessage("Da cap nhat thong tin Dau sach!");
            } else {
                screen.showMessage("Loi khi cap nhat!", true);
            }
        }
    }

    void dialogBorrowOrReturn(int mode) {
        const char* titles[3] = { "MUON SACH", "TRA SACH", "BAO MAT SACH" };
        const char* labels[2] = { "Ma The :", "Ma Sach:" }; char values[2][50] = { "", "" }; const int maxLens[2] = { 10, 22 };

        if (screen.showFormDialog(titles[mode], labels, values, maxLens, 2)) {
            const char* res = (mode == 0) ? service.borrowBook(atoi(values[0]), values[1]) : service.returnBook(atoi(values[0]), values[1], mode);
            if (strcmp(res, "OK") == 0) screen.showMessage("Thanh cong!"); else screen.showMessage(res, true);
        }
    }

    void renderDialogChiTietSach(DauSach* ds) {
        if (!ds) return;
        int selRow = 0;
        int scrOffset = 0;
        while (true) {
            NodeSach* arr[500]; int count = 0;
            NodeSach* p = ds->dms;
            while (p && count < 500) { arr[count++] = p; p = p->next; }

            int visibleRows = 12;
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

            screen.drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BG, "CHI TIET SACH");

            char titleInfo[150];
            sprintf(titleInfo, "Dau sach: %s | So trang: %d | Tong so cuon: %d", ds->tenSach, ds->soTrang, ds->soLuongSach);
            screen.writeStr(startX + 3, startY + 2, titleInfo, C_DLG_BG);

            const int colX[3] = { startX + 3, startX + 22, startX + 44 };
            const int colW[3] = { 18, 21, 34 };
            const char* headers[3] = { "MA SACH", "TRANG THAI", "VI TRI KE SACH" };

            int tableX = startX + 2;
            int tableY = startY + 4;
            int tableW = boxW - 4;
            int tableH = 14;

            screen.drawTableGrid(tableX, tableY, tableW, tableH, colX, 3, headers, count, selRow);

            for (int i = 0; i < visibleRows && (scrOffset + i) < count; i++) {
                int idx = scrOffset + i;
                int rowY = tableY + 3 + i;
                bool isSel = (idx == selRow);

                WORD rowAttr = isSel ? C_ROW_SEL : C_ROW_NORM;
                WORD sepAttr = isSel ? C_ROW_SEL : C_BORDER;

                screen.fillRect(tableX + 1, rowY, tableW - 1, 1, rowAttr);

                screen.putChar(colX[1] - 1, rowY, B_V, sepAttr);
                screen.putChar(colX[2] - 1, rowY, B_V, sepAttr);

                screen.writeCell(colX[0], rowY, colW[0], arr[idx]->data.maSach, rowAttr);

                const char* ttStr = "Cho muon duoc";
                if (arr[idx]->data.trangThai == DA_CHO_MUON) ttStr = "Da muon";
                else if (arr[idx]->data.trangThai == DA_THANH_LY) ttStr = "Da thanh ly";

                screen.writeCell(colX[1], rowY, colW[1], ttStr, rowAttr);
                screen.writeCell(colX[2], rowY, colW[2], arr[idx]->data.viTri, rowAttr);

                screen.putChar(tableX, rowY, B_V, C_BORDER);
                screen.putChar(tableX + tableW, rowY, B_V, C_BORDER);
            }

            screen.writeStr(startX + 2, startY + boxH - 2, "F1:Them  F2:Sua VTri  F3:Xoa  F4:Thanh ly  ESC:Dong", C_DLG_BG);
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
                    if (arr[selRow]->data.trangThai == DA_CHO_MUON) {
                        screen.showMessage("Khong the sua vi sach dang duoc muon!");
                        continue;
                    }
                    if (arr[selRow]->data.trangThai == DA_THANH_LY) {
                        screen.showMessage("Sach da thanh ly, khong the doi vi tri!");
                        continue;
                    }
                    const char* lb[1] = { "Vi tri ke:" };
                    char val[1][50];
                    strcpy(val[0], arr[selRow]->data.viTri);
                    const int ml[1] = { 25 };

                    if (screen.showFormDialog("HIEU CHINH VI TRI SACH", lb, val, ml, 1)) {
                        strcpy(arr[selRow]->data.viTri, val[0]);
                    }
                }
                else if (ext == KEY_F3 && count > 0) {
                    if (arr[selRow]->data.trangThai == DA_CHO_MUON) {
                        screen.showMessage("Khong the xoa vi sach dang duoc muon!");
                        continue;
                    }
                    if (screen.showConfirm("Xac nhan xoa cuon sach nay?")) {
                        if (service.removeSachVatLy(ds, arr[selRow]->data.maSach)) {
                            if (selRow >= count - 1 && selRow > 0) selRow--;
                        } else {
                            screen.showMessage("Loi khi xoa sach!");
                        }
                    }
                } else if (ext == KEY_F4 && count > 0) {
                    if (arr[selRow]->data.trangThai == DA_CHO_MUON) {
                        screen.showMessage("Khong the thanh ly vi sach dang duoc muon!");
                        continue;
                    }
                    if (arr[selRow]->data.trangThai == DA_THANH_LY) {
                        screen.showMessage("Sach nay da duoc thanh ly roi!");
                        continue;
                    }
                    if (screen.showConfirm("Xac nhan THANH LY cuon sach nay?")) {
                        arr[selRow]->data.trangThai = DA_THANH_LY;
                        strcpy(arr[selRow]->data.viTri, "");
                    }
                }
            }
        }
        screen.clear();
    }

    void renderDialogChiTietDocGia(NodeDocGia* dg) {
        if (!dg) return;
        int selRow = 0;
        int scrOffset = 0;
        while (true) {
            NodeMuonTra* arr[500]; int count = 0;
            NodeMuonTra* p = dg->data.dsMuonTra;
            while (p && count < 500) { arr[count++] = p; p = p->next; }

            int visibleRows = 12;
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

            screen.drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BG, "CHI TIET DOC GIA - LICH SU MUON TRA");

            char titleInfo[100];
            sprintf(titleInfo, "Doc gia: %d - %s %s | Trang thai: %s",
                    dg->data.maThe, dg->data.ho, dg->data.ten,
                    dg->data.trangThai == HOAT_DONG ? "Hoat dong" : "Bi khoa");
            screen.writeStr(startX + 3, startY + 2, titleInfo, C_DLG_BG);

            const int colX[5] = { startX + 3, startX + 17, startX + 61, startX + 75, startX + 89 };
            const int colW[5] = { 13, 43, 13, 13, 13 };
            const char* headers[5] = { "MA SACH", "TEN SACH", "NGAY MUON", "NGAY TRA", "TRANG THAI" };

            int tableX = startX + 2;
            int tableY = startY + 4;
            int tableW = boxW - 4;
            int tableH = 14;

            screen.drawTableGrid(tableX, tableY, tableW, tableH, colX, 5, headers, count, selRow);

            Date today = getCurrentDate();
            for (int i = 0; i < visibleRows && (scrOffset + i) < count; i++) {
                int idx = scrOffset + i;
                int rowY = tableY + 3 + i;
                bool isSel = (idx == selRow);

                WORD rowAttr = isSel ? C_ROW_SEL : C_ROW_NORM;
                WORD sepAttr = isSel ? C_ROW_SEL : C_BORDER;

                screen.fillRect(tableX + 1, rowY, tableW - 1, 1, rowAttr);
                for (int c = 1; c < 5; c++) screen.putChar(colX[c] - 1, rowY, B_V, sepAttr);

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
                WORD statusAttr = rowAttr;
                if (arr[idx]->data.trangThai == DA_TRA) {
                    ttStr = "Da tra";
                } else if (arr[idx]->data.trangThai == LAM_MAT) {
                    ttStr = "Mat sach";
                    statusAttr = isSel ? C_ROW_SEL : C_BADGE_ERR;
                } else if (arr[idx]->data.trangThai == DANG_MUON) {
                    if (daysBetween(arr[idx]->data.ngayMuon, today) > 7) {
                        ttStr = "! Qua han";
                        statusAttr = isSel ? C_ROW_SEL : C_BADGE_ERR;
                    }
                }
                screen.writeCell(colX[4], rowY, colW[4], ttStr, statusAttr);

                screen.putChar(tableX, rowY, B_V, C_BORDER);
                screen.putChar(tableX + tableW, rowY, B_V, C_BORDER);
            }

            screen.writeStr(startX + 3, startY + boxH - 2, "[Up/Down]: Cuon danh sach  |  [ESC]: Dong", C_DLG_BG);
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
                int boxW = 60, boxH = 12;
                int startX = (SCREEN_W - boxW) / 2;
                int startY = (SCREEN_H - boxH) / 2;

                screen.drawBox(startX, startY, boxW, boxH, C_DLG_BG, C_DLG_BG, "KET QUA TRA CUU");

                char buf1[100], buf2[100], buf3[100], buf4[100];
                sprintf(buf1, "- Ma Sach    : %s", bookNode->data.maSach);
                sprintf(buf2, "- Dau Sach   : %s", ds->tenSach);
                sprintf(buf3, "- Vi tri ke  : %s", bookNode->data.viTri);

                const char* status = "Cho muon duoc";
                if (bookNode->data.trangThai == DA_CHO_MUON) status = "Dang duoc muon";
                else if (bookNode->data.trangThai == DA_THANH_LY) status = "Da thanh ly";
                sprintf(buf4, "- Trang thai : %s", status);

                screen.writeStr(startX + 3, startY + 3, buf1, C_DLG_BG);
                screen.writeStr(startX + 3, startY + 5, buf2, C_DLG_BG);
                screen.writeStr(startX + 3, startY + 7, buf3, C_DLG_BG);
                WORD statusColor = (bookNode->data.trangThai == CHO_MUON_DUOC) ? C_BADGE_OK : C_BADGE_ERR;

                screen.writeStr(startX + 3, startY + 9, buf4, statusColor | (C_DLG_BG & 0xF0));

                screen.writeStr(startX + 15, startY + boxH - 2, " [ ENTER / ESC de dong ] ", C_DLG_BG);
                screen.render();

                int ch;
                while ((ch = _getch()) != KEY_ESC && ch != KEY_ENTER);

                screen.clear();
            } else {
                screen.showMessage("Khong tim thay Ma Sach nay trong he thong!", true);
            }
        }
    }

public:
    LibraryTUI(LibraryService& srv) : service(srv) {
        screen.init("LIBRARY TUI");
        allocBuffers();
    }

    ~LibraryTUI() { freeBuffers(); }

    void run() {
        bool isSearching = false;
        char backupKeyword[50] = "";

        while (true) {
            int nDGRaw = 0;
            service.getReadersList(bufDgArrRaw, nDGRaw, false);

            int nDSRaw = 0;
            service.getBooksList(bufDsViewRaw, nDSRaw, false);

            int nDG = 0;
            if (currentTab == 0) filterAndSortDocGia(bufDgArrRaw, nDGRaw, bufDgArr, nDG);

            int nDS = 0;
            int nTreeRows = 0;

            if (currentTab == 1) {
                if (viewCategoryTree) {
                    buildCategoryTreeRows(bufTreeRows, nTreeRows);
                } else {
                    filterAndSortDauSach(bufDsViewRaw, nDSRaw, bufDsView, nDS);
                }
            }

            int nMT = 0;
            if (currentTab == 2) {
                int total = 0;
                for (int i = 0; i < nDGRaw && total < MAX_MT_BUF; i++) {
                    NodeMuonTra* mt = bufDgArrRaw[i]->data.dsMuonTra;
                    while (mt && total < MAX_MT_BUF) {
                        if (mt->data.trangThai == DANG_MUON) {
                            NodeSach* ns = nullptr;
                            DauSach* ds = service.findDauSachByMaSach(mt->data.maSach, ns);
                            bufMtAll[total].maThe = bufDgArrRaw[i]->data.maThe;
                            sprintf(bufMtAll[total].hoTen, "%s %s",
                                    bufDgArrRaw[i]->data.ho, bufDgArrRaw[i]->data.ten);
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

            int nQH = 0;
            if (currentTab == 3) {
                int cnt = 0;
                service.getOverdueList(bufQhRaw, cnt);
                filterAndSortQuaHan(bufQhRaw, cnt, bufQhArr, nQH);
            }

            int nT10 = 0;
            if (currentTab == 4) {
                int cnt = 0;
                service.getTop10Books(bufTopRaw, cnt);
                for (int i = 0; i < cnt; i++) {
                    bufTrRaw[i].rank = i + 1;
                    bufTrRaw[i].ds = bufTopRaw[i];
                }
                filterAndSortTop10(bufTrRaw, cnt, bufTrArr, nT10);
            }

            drawTopDashboard(nDGRaw, nDSRaw);
            drawTabs();
            drawSearchBar(isSearching);

            if (currentTab == 0) renderTabDocGia(bufDgArr, nDG);
            else if (currentTab == 1) renderTabDauSach(bufDsView, nDS, bufTreeRows, nTreeRows);
            else if (currentTab == 2) renderTabMuonTra(bufMtArr, nMT);
            else if (currentTab == 3) renderTabQuaHan(bufQhArr, nQH);
            else if (currentTab == 4) renderTabTop10(bufTrArr, nT10);

            screen.putChar(1, 3, B_TL, C_BORDER);
            screen.putChar(SCREEN_W - 2, 3, B_TR, C_BORDER);
            screen.putChar(1, 4, B_V, C_BORDER);
            screen.putChar(SCREEN_W - 2, 4, B_V, C_BORDER);
            screen.putChar(1, 5, B_L, C_BORDER);
            screen.putChar(SCREEN_W - 2, 5, B_R, C_BORDER);

            screen.render();

            int ch = _getch();

            if (ch == 0 || ch == 224) {
                int ext = _getch();

                if (ext == KEY_UP) selectedRow--;
                else if (ext == KEY_DOWN) selectedRow++;
                else if (ext == KEY_PGUP) selectedRow -= 10;
                else if (ext == KEY_PGDN) selectedRow += 10;

                else if (ext == KEY_LEFT) {
                    if (!isSearching) {
                        if (currentTab > 0) currentTab--; else currentTab = 4;
                        selectedRow = 0; scrollOffset = 0;
                    }
                }
                else if (ext == KEY_RIGHT) {
                    if (!isSearching) {
                        if (currentTab < 4) currentTab++; else currentTab = 0;
                        selectedRow = 0; scrollOffset = 0;
                    }
                }

                if (!isSearching) {
                    if (currentTab == 0) {
                        if (ext == KEY_F1) dialogAddOrEditDocGia(nullptr);
                        else if (ext == KEY_F2 && nDG > 0) dialogAddOrEditDocGia(bufDgArr[selectedRow]);
                        else if (ext == KEY_F3 && nDG > 0) {
                            if (screen.showConfirm("Xac nhan xoa doc gia nay?")) {
                                bool ok = service.removeDocGia(bufDgArr[selectedRow]->data.maThe);
                                screen.showMessage(ok ? "Da xoa!" : "Loi: Doc gia dang muon sach!");
                            }
                        }
                    } else if (currentTab == 1) {
                        if (ext == KEY_F1) dialogAddDauSach();
                        else if (ext == KEY_F2) {
                            DauSach* targetDS = nullptr;
                            if (viewCategoryTree) {
                                if (nTreeRows > 0 && !bufTreeRows[selectedRow].isCategory)
                                    targetDS = bufTreeRows[selectedRow].ds;
                            } else {
                                if (nDS > 0) targetDS = bufDsView[selectedRow];
                            }
                            if (targetDS) dialogEditDauSach(targetDS);
                        }
                        else if (ext == KEY_F3) {
                            DauSach* targetDS = nullptr;
                            if (viewCategoryTree) {
                                if (nTreeRows > 0 && !bufTreeRows[selectedRow].isCategory)
                                    targetDS = bufTreeRows[selectedRow].ds;
                            } else {
                                if (nDS > 0) targetDS = bufDsView[selectedRow];
                            }
                            if (targetDS) {
                                if (screen.showConfirm("Xac nhan XOA Dau sach nay va toan bo sach con?")) {
                                    if (service.removeDauSach(targetDS->ISBN)) {
                                        selectedRow = 0; scrollOffset = 0;
                                    } else {
                                        screen.showMessage("Loi: Dang co sach duoc muon, khong the xoa!", true);
                                    }
                                }
                            }
                        }
                        else if (ext == KEY_F4) {
                            viewCategoryTree = !viewCategoryTree;
                            selectedRow = 0; scrollOffset = 0;
                        }
                        else if (ext == KEY_F5) dialogTraCuuMaSach();
                    } else if (currentTab == 2) {
                        if (ext == KEY_F1) dialogBorrowOrReturn(0);
                        else if (ext == KEY_F2) dialogBorrowOrReturn(1);
                        else if (ext == KEY_F3) dialogBorrowOrReturn(2);
                    }
                }
                continue;
            }

            if (isSearching) {
                bool keywordChanged = false;

                if (ch == KEY_ENTER) {
                    isSearching = false;
                } else if (ch == KEY_ESC) {
                    isSearching = false;
                    strcpy(searchKeyword[currentTab], backupKeyword);
                    keywordChanged = true;
                } else if (ch == KEY_TAB) {
                    if (GetKeyState(VK_SHIFT) < 0) {
                        if (selectedCol[currentTab] > 0) selectedCol[currentTab]--;
                        else selectedCol[currentTab] = numFiltersForTab[currentTab] - 1;
                    } else {
                        if (selectedCol[currentTab] < numFiltersForTab[currentTab] - 1) selectedCol[currentTab]++;
                        else selectedCol[currentTab] = 0;
                    }
                    sortState[currentTab] = 0;
                    keywordChanged = true;
                } else if (ch == 8) {
                    int len = strlen(searchKeyword[currentTab]);
                    if (len > 0) {
                        searchKeyword[currentTab][len - 1] = '\0';
                        keywordChanged = true;
                    }
                } else if (ch >= 32 && ch <= 126) {
                    int len = strlen(searchKeyword[currentTab]);
                    if (len < 38) {
                        searchKeyword[currentTab][len] = (char)ch;
                        searchKeyword[currentTab][len + 1] = '\0';
                        keywordChanged = true;
                    }
                }

                if (!(currentTab == 1 && viewCategoryTree)) {
                    if (keywordChanged) { selectedRow = 0; scrollOffset = 0; }
                }
                continue;
            }

            if (ch == KEY_ESC) {
                int ans = screen.showExitConfirm();
                if (ans == 1) { service.saveToFile(); break; }
                else if (ans == 0) { break; }
                continue;
            }

            if (ch == KEY_TAB) {
                if (GetKeyState(VK_SHIFT) < 0) {
                    if (selectedCol[currentTab] > 0) selectedCol[currentTab]--;
                    else selectedCol[currentTab] = numFiltersForTab[currentTab] - 1;
                } else {
                    if (selectedCol[currentTab] < numFiltersForTab[currentTab] - 1) selectedCol[currentTab]++;
                    else selectedCol[currentTab] = 0;
                }
                sortState[currentTab] = 0; selectedRow = 0; scrollOffset = 0;
                continue;
            }

            if (ch == KEY_ENTER) {
                if (currentTab == 0) {
                    if (nDG > 0) renderDialogChiTietDocGia(bufDgArr[selectedRow]);
                } else if (currentTab == 1) {
                    if (viewCategoryTree) {
                        if (nTreeRows > 0 && !bufTreeRows[selectedRow].isCategory && bufTreeRows[selectedRow].ds)
                            renderDialogChiTietSach(bufTreeRows[selectedRow].ds);
                    } else {
                        if (nDS > 0) renderDialogChiTietSach(bufDsView[selectedRow]);
                    }
                }
            }

            if (ch == 'f' || ch == 'F') {
                if (!isSearching) {
                    if (currentTab == 1 && viewCategoryTree) {
                        screen.showMessage("Tinh nang tim kiem bi vo hieu hoa o che do nay!");
                    } else {
                        isSearching = true;
                        strcpy(backupKeyword, searchKeyword[currentTab]);
                    }
                }
                continue;
            }

            if (ch == 's' || ch == 'S') {
                if (currentTab == 1 && viewCategoryTree) {
                    screen.showMessage("Khong the xap xep o che do nay!");
                } else {
                    sortState[currentTab] = (sortState[currentTab] + 1) % 3;
                    selectedRow = 0; scrollOffset = 0;
                }
                continue;
            }
        }
    }
};

#endif