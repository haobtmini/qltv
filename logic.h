#ifndef LOGIC_H
#define LOGIC_H

#include "models.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#define MAX_DOC_GIA 1000000
#define MAX_OVERDUE 3000000   

class LibraryService {
public:
    static void toUpperStr(char* dest, const char* src) {
        int i = 0;
        while (src[i]) {
            dest[i] = (src[i] >= 'a' && src[i] <= 'z') ? (src[i] - 32) : src[i];
            i++;
        }
        dest[i] = '\0';
    }

private:
    ListDauSach dsDauSach;
    NodeDocGia* rootDocGia;
    KeyManager kmDocGia;

    int getHeight(NodeDocGia* node) { return node ? node->height : 0; }
    int getBalance(NodeDocGia* node) { return node ? getHeight(node->left) - getHeight(node->right) : 0; }
    int maxVal(int a, int b) { return (a > b) ? a : b; }

    NodeDocGia* rotateRight(NodeDocGia* y) {
        NodeDocGia* x = y->left;
        NodeDocGia* T2 = x->right;
        x->right = y; y->left = T2;
        y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }
    NodeDocGia* rotateLeft(NodeDocGia* x) {
        NodeDocGia* y = x->right;
        NodeDocGia* T2 = y->left;
        y->left = x; x->right = T2;
        x->height = maxVal(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = maxVal(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    NodeDocGia* insertBST(NodeDocGia* root, TheDocGia dg) {
        if (!root) {
            NodeDocGia* newNode = new NodeDocGia;
            newNode->data = dg;
            newNode->left = nullptr;
            newNode->right = nullptr;
            newNode->height = 1;
            return newNode;
        }
        if (dg.maThe < root->data.maThe) root->left = insertBST(root->left, dg);
        else if (dg.maThe > root->data.maThe) root->right = insertBST(root->right, dg);
        else return root;

        root->height = 1 + maxVal(getHeight(root->left), getHeight(root->right));
        int balance = getBalance(root);
        if (balance > 1 && dg.maThe < root->left->data.maThe) return rotateRight(root);
        if (balance < -1 && dg.maThe > root->right->data.maThe) return rotateLeft(root);
        if (balance > 1 && dg.maThe > root->left->data.maThe) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && dg.maThe < root->right->data.maThe) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
        return root;
    }

    NodeDocGia* findMin(NodeDocGia* node) {
        while (node && node->left) node = node->left;
        return node;
    }

    NodeDocGia* deleteBST(NodeDocGia* root, int maThe, bool& success) {
        if (!root) return nullptr;
        if (maThe < root->data.maThe) root->left = deleteBST(root->left, maThe, success);
        else if (maThe > root->data.maThe) root->right = deleteBST(root->right, maThe, success);
        else {
            NodeMuonTra* mt = root->data.dsMuonTra;
            while (mt) {
                if (mt->data.trangThai == DANG_MUON) { success = false; return root; }
                mt = mt->next;
            }
            success = true;
            if (!root->left) { NodeDocGia* tmp = root->right; delete root; root = tmp; }
            else if (!root->right) { NodeDocGia* tmp = root->left; delete root; root = tmp; }
            else {
                NodeDocGia* tmp = findMin(root->right);
                root->data = tmp->data;
                root->right = deleteBST(root->right, tmp->data.maThe, success);
            }
        }
        if (!root) return nullptr;
        root->height = 1 + maxVal(getHeight(root->left), getHeight(root->right));
        int balance = getBalance(root);
        if (balance > 1 && getBalance(root->left) >= 0) return rotateRight(root);
        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }
        if (balance < -1 && getBalance(root->right) <= 0) return rotateLeft(root);
        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
        return root;
    }

    void freeBST(NodeDocGia* root) {
        if (!root) return;
        freeBST(root->left);
        freeBST(root->right);
        NodeMuonTra* mt = root->data.dsMuonTra;
        while (mt) { NodeMuonTra* temp = mt; mt = mt->next; delete temp; }
        delete root;
    }

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

public:
    LibraryService() : rootDocGia(nullptr) {
        kmDocGia.init(1);
    }

    ~LibraryService() { clearData(); }

    void clearData() {
        for (int i = 0; i < dsDauSach.n; i++) {
            NodeSach* p = dsDauSach.nodes[i]->dms;
            while (p) { NodeSach* temp = p; p = p->next; delete temp; }
            delete dsDauSach.nodes[i];
        }
        dsDauSach.n = 0;
        freeBST(rootDocGia);
        rootDocGia = nullptr;
    }

    bool loadFromFile(const char* = nullptr) {
        clearData();

        FILE* fDS = fopen("dausach.txt", "r");
        if (fDS) {
            int totalDS = 0;
            if (fscanf(fDS, "%d\n", &totalDS) == 1) {
                char line[512];
                for (int i = 0; i < totalDS; i++) {
                    std::string strLine;
                    do {
                        if (!fgets(line, sizeof(line), fDS)) break;
                        strLine = trim(line);
                    } while (strLine.empty());
                    if (strLine.empty()) break;

                    std::stringstream ss(strLine);
                    std::string token;
                    DauSach* ds = new DauSach();

                    std::getline(ss, token, '|'); strcpy(ds->ISBN, token.c_str());
                    std::getline(ss, token, '|'); strcpy(ds->tenSach, token.c_str());
                    std::getline(ss, token, '|'); ds->soTrang = std::stoi(token);
                    std::getline(ss, token, '|'); strcpy(ds->tacGia, token.c_str());
                    std::getline(ss, token, '|'); ds->namXB = std::stoi(token);
                    std::getline(ss, token, '|'); strcpy(ds->theLoai, token.c_str());
                    std::getline(ss, token, '|'); ds->soLuongSach = std::stoi(token);
                    std::getline(ss, token, '|'); ds->soLuotMuon = std::stoi(token);
                    std::getline(ss, token, '|'); int nDMS = std::stoi(token);

                    ds->kmSach.loadFromFile(fDS);

                    ds->dms = nullptr;
                    NodeSach* tail = nullptr;
                    for (int j = 0; j < nDMS; j++) {
                        do {
                            if (!fgets(line, sizeof(line), fDS)) break;
                            strLine = trim(line);
                        } while (strLine.empty());
                        if (strLine.empty()) break;

                        std::stringstream ssDMS(strLine);
                        NodeSach* p = new NodeSach();

                        std::getline(ssDMS, token, '|'); strcpy(p->data.maSach, token.c_str());
                        std::getline(ssDMS, token, '|'); p->data.trangThai = (TrangThaiSach)std::stoi(token);
                        std::getline(ssDMS, token, '|'); strcpy(p->data.viTri, token.c_str());
                        p->next = nullptr;

                        if (!ds->dms) ds->dms = p;
                        else tail->next = p;
                        tail = p;
                    }
                    dsDauSach.nodes[dsDauSach.n++] = ds;
                }
            }
            fclose(fDS);
        }

        FILE* fDG = fopen("docgia.txt", "r");
        if (fDG) {
            kmDocGia.loadFromFile(fDG);

            int totalDG = 0;
            if (fscanf(fDG, "%d\n", &totalDG) == 1) {
                char line[512];
                for (int i = 0; i < totalDG; i++) {
                    std::string strLine;
                    do {
                        if (!fgets(line, sizeof(line), fDG)) break;
                        strLine = trim(line);
                    } while (strLine.empty());
                    if (strLine.empty()) break;

                    std::stringstream ss(strLine);
                    std::string token;
                    TheDocGia dg{};

                    std::getline(ss, token, '|'); dg.maThe = std::stoi(token);
                    std::getline(ss, token, '|'); strcpy(dg.ho, token.c_str());
                    std::getline(ss, token, '|'); strcpy(dg.ten, token.c_str());
                    std::getline(ss, token, '|'); strcpy(dg.phai, token.c_str());
                    std::getline(ss, token, '|'); dg.trangThai = (TrangThaiThe)std::stoi(token);
                    std::getline(ss, token, '|'); int nMT = std::stoi(token);

                    dg.dsMuonTra = nullptr;
                    NodeMuonTra* tailMT = nullptr;
                    for (int j = 0; j < nMT; j++) {
                        do {
                            if (!fgets(line, sizeof(line), fDG)) break;
                            strLine = trim(line);
                        } while (strLine.empty());
                        if (strLine.empty()) break;

                        std::stringstream ssMT(strLine);
                        NodeMuonTra* p = new NodeMuonTra();

                        std::getline(ssMT, token, '|'); strcpy(p->data.maSach, token.c_str());

                        std::getline(ssMT, token, '|');
                        std::stringstream ssDateM(token);
                        ssDateM >> p->data.ngayMuon.d >> p->data.ngayMuon.m >> p->data.ngayMuon.y;

                        std::getline(ssMT, token, '|');
                        std::stringstream ssDateT(token);
                        ssDateT >> p->data.ngayTra.d >> p->data.ngayTra.m >> p->data.ngayTra.y;

                        std::getline(ssMT, token, '|'); p->data.trangThai = (TrangThaiMuonTra)std::stoi(token);
                        p->next = nullptr;

                        if (!dg.dsMuonTra) dg.dsMuonTra = p;
                        else tailMT->next = p;
                        tailMT = p;
                    }
                    addDocGia(dg);
                }
            }
            fclose(fDG);
        }
        return true;
    }

    void saveToFile(const char* = nullptr) {
        FILE* fDS = fopen("dausach.txt", "w");
        if (fDS) {
            fprintf(fDS, "%d\n", dsDauSach.n);
            for (int i = 0; i < dsDauSach.n; i++) {
                DauSach* ds = dsDauSach.nodes[i];
                int nDMS = 0;
                for (NodeSach* p = ds->dms; p != nullptr; p = p->next) nDMS++;

                fprintf(fDS, "%s|%s|%d|%s|%d|%s|%d|%d|%d\n",
                    ds->ISBN, ds->tenSach, ds->soTrang, ds->tacGia, ds->namXB,
                    ds->theLoai, ds->soLuongSach, ds->soLuotMuon, nDMS);

                ds->kmSach.saveToFile(fDS);

                for (NodeSach* p = ds->dms; p != nullptr; p = p->next) {
                    fprintf(fDS, "%s|%d|%s\n", p->data.maSach, (int)p->data.trangThai, p->data.viTri);
                }
            }
            fclose(fDS);
        }

        FILE* fDG = fopen("docgia.txt", "w");
        if (fDG) {
            kmDocGia.saveToFile(fDG);

            NodeDocGia** arr = new NodeDocGia*[MAX_DOC_GIA];
            int nDG = 0;
            getReadersList(arr, nDG, false);
            fprintf(fDG, "%d\n", nDG);

            for (int i = 0; i < nDG; i++) {
                TheDocGia& dg = arr[i]->data;
                int nMT = 0;
                for (NodeMuonTra* p = dg.dsMuonTra; p != nullptr; p = p->next) nMT++;

                fprintf(fDG, "%d|%s|%s|%s|%d|%d\n",
                    dg.maThe, dg.ho, dg.ten, dg.phai, (int)dg.trangThai, nMT);

                for (NodeMuonTra* p = dg.dsMuonTra; p != nullptr; p = p->next) {
                    fprintf(fDG, "%s|%d %d %d|%d %d %d|%d\n",
                        p->data.maSach,
                        p->data.ngayMuon.d, p->data.ngayMuon.m, p->data.ngayMuon.y,
                        p->data.ngayTra.d, p->data.ngayTra.m, p->data.ngayTra.y,
                        (int)p->data.trangThai);
                }
            }
            delete[] arr;
            fclose(fDG);
        }
    }

    int generateReaderKey() { return kmDocGia.getNewKey(); }
    void addDocGia(TheDocGia dg) { rootDocGia = insertBST(rootDocGia, dg); }

    bool removeDocGia(int maThe) {
        bool ok = false;
        rootDocGia = deleteBST(rootDocGia, maThe, ok);
        if (ok) kmDocGia.returnKey(maThe);
        return ok;
    }

    NodeDocGia* searchDocGia(int maThe) {
        NodeDocGia* p = rootDocGia;
        while (p) {
            if (maThe == p->data.maThe) return p;
            p = (maThe < p->data.maThe) ? p->left : p->right;
        }
        return nullptr;
    }

    void getReadersList(NodeDocGia* arr[], int& count, bool sortByName) {
        count = 0;
        NodeDocGia* curr = rootDocGia;
        while (curr) {
            if (!curr->left) {
                arr[count++] = curr;
                curr = curr->right;
            } else {
                NodeDocGia* pre = curr->left;
                while (pre->right && pre->right != curr) pre = pre->right;
                if (!pre->right) {
                    pre->right = curr;
                    curr = curr->left;
                } else {
                    pre->right = nullptr;
                    arr[count++] = curr;
                    curr = curr->right;
                }
            }
        }
        if (sortByName && count > 1) quickSortDocGia(arr, 0, count - 1);
    }

    static int compareDocGiaTenHo(NodeDocGia* a, NodeDocGia* b) {
        int cmp = _stricmp(a->data.ten, b->data.ten);
        return (cmp != 0) ? cmp : _stricmp(a->data.ho, b->data.ho);
    }

    void quickSortDocGia(NodeDocGia* arr[], int left, int right) {
        int i = left, j = right;
        NodeDocGia* pivot = arr[(left + right) / 2];
        while (i <= j) {
            while (compareDocGiaTenHo(arr[i], pivot) < 0) i++;
            while (compareDocGiaTenHo(arr[j], pivot) > 0) j--;
            if (i <= j) { NodeDocGia* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; i++; j--; }
        }
        if (left < j) quickSortDocGia(arr, left, j);
        if (i < right) quickSortDocGia(arr, i, right);
    }

    bool insertDauSach(DauSach* newDS) {
        if (dsDauSach.n >= MAX_DAUSACH) return false;
        for (int i = 0; i < dsDauSach.n; i++) {
            if (_stricmp(dsDauSach.nodes[i]->ISBN, newDS->ISBN) == 0) return false;
        }
        int left = 0, right = dsDauSach.n - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (_stricmp(dsDauSach.nodes[mid]->tenSach, newDS->tenSach) < 0) left = mid + 1;
            else right = mid - 1;
        }
        for (int i = dsDauSach.n; i > left; i--) dsDauSach.nodes[i] = dsDauSach.nodes[i - 1];
        dsDauSach.nodes[left] = newDS;
        dsDauSach.n++;
        return true;
    }

    bool updateDauSach(const char* ISBN, const char* newTen, const char* newTacGia, const char* newTheLoai, int newNam, int newSoTrang) {
        int pos = -1;
        for (int i = 0; i < dsDauSach.n; i++) {
            if (_stricmp(dsDauSach.nodes[i]->ISBN, ISBN) == 0) { pos = i; break; }
        }
        if (pos == -1) return false;
        DauSach* ds = dsDauSach.nodes[pos];
        for (int i = pos; i < dsDauSach.n - 1; i++) dsDauSach.nodes[i] = dsDauSach.nodes[i + 1];
        dsDauSach.n--;
        strcpy(ds->tenSach, newTen);
        strcpy(ds->tacGia, newTacGia);
        strcpy(ds->theLoai, newTheLoai);
        ds->namXB = newNam;
        ds->soTrang = newSoTrang;
        insertDauSach(ds);
        return true;
    }

    bool removeDauSach(const char* ISBN) {
        int pos = -1;
        for (int i = 0; i < dsDauSach.n; i++) {
            if (_stricmp(dsDauSach.nodes[i]->ISBN, ISBN) == 0) { pos = i; break; }
        }
        if (pos == -1) return false;

        NodeSach* p = dsDauSach.nodes[pos]->dms;
        while (p) { if (p->data.trangThai == DA_CHO_MUON) return false; p = p->next; }

        p = dsDauSach.nodes[pos]->dms;
        while (p) { NodeSach* temp = p; p = p->next; delete temp; }

        delete dsDauSach.nodes[pos];
        for (int i = pos; i < dsDauSach.n - 1; i++) dsDauSach.nodes[i] = dsDauSach.nodes[i + 1];
        dsDauSach.n--;
        return true;
    }

    void addSachAuto(DauSach* ds, int soLuong, const char* viTriMacDinh) {
        if (!ds) return;
        auto getSuffix = [](const char* ma) {
            const char* us = strchr(ma, '_');
            return us ? atoi(us + 1) : 0;
        };
        for (int i = 0; i < soLuong; i++) {
            ds->soLuongSach++;
            NodeSach* p = new NodeSach;
            int nextId = ds->kmSach.getNewKey();
            sprintf(p->data.maSach, "%s_%d", ds->ISBN, nextId);
            p->data.trangThai = CHO_MUON_DUOC;
            strcpy(p->data.viTri, viTriMacDinh);
            p->next = nullptr;

            if (!ds->dms) {
                ds->dms = p;
            } else {
                int newSuffix = nextId;
                if (getSuffix(ds->dms->data.maSach) > newSuffix) {
                    p->next = ds->dms;
                    ds->dms = p;
                } else {
                    NodeSach* curr = ds->dms;
                    while (curr->next && getSuffix(curr->next->data.maSach) < newSuffix) curr = curr->next;
                    p->next = curr->next;
                    curr->next = p;
                }
            }
        }
    }

    bool removeSachVatLy(DauSach* ds, const char* maSach) {
        if (!ds || !ds->dms) return false;
        auto recycleKey = [](DauSach* d, const char* ms) {
            const char* underscore = strchr(ms, '_');
            if (underscore) d->kmSach.returnKey(atoi(underscore + 1));
        };
        if (_stricmp(ds->dms->data.maSach, maSach) == 0) {
            if (ds->dms->data.trangThai == DA_CHO_MUON) return false;
            recycleKey(ds, ds->dms->data.maSach);
            NodeSach* temp = ds->dms;
            ds->dms = ds->dms->next;
            delete temp;
            ds->soLuongSach--;
            return true;
        }
        NodeSach* p = ds->dms;
        while (p->next) {
            if (_stricmp(p->next->data.maSach, maSach) == 0) {
                if (p->next->data.trangThai == DA_CHO_MUON) return false;
                recycleKey(ds, p->next->data.maSach);
                NodeSach* temp = p->next;
                p->next = p->next->next;
                delete temp;
                ds->soLuongSach--;
                return true;
            }
            p = p->next;
        }
        return false;
    }

    void getBooksList(DauSach* viewArr[], int& nView, bool sortByCategory) {
        nView = dsDauSach.n;
        for (int i = 0; i < nView; i++) viewArr[i] = dsDauSach.nodes[i];
        if (sortByCategory && nView > 1) quickSortDauSach(viewArr, 0, nView - 1);
    }

    void getBooksSortedByCategory(DauSach* outArr[], int& count) {
        count = dsDauSach.n;
        for (int i = 0; i < count; i++) outArr[i] = dsDauSach.nodes[i];
        if (count > 1) quickSortDauSach(outArr, 0, count - 1);
    }

    static int compareTheLoaiTen(DauSach* a, DauSach* b) {
        int cmp = _stricmp(a->theLoai, b->theLoai);
        return (cmp != 0) ? cmp : _stricmp(a->tenSach, b->tenSach);
    }

    void quickSortDauSach(DauSach* arr[], int left, int right) {
        int i = left, j = right;
        DauSach* pivot = arr[(left + right) / 2];
        while (i <= j) {
            while (compareTheLoaiTen(arr[i], pivot) < 0) i++;
            while (compareTheLoaiTen(arr[j], pivot) > 0) j--;
            if (i <= j) { DauSach* tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp; i++; j--; }
        }
        if (left < j) quickSortDauSach(arr, left, j);
        if (i < right) quickSortDauSach(arr, i, right);
    }

    DauSach* searchBookByTitle(const char* keyword) {
        char s1[100], s2[100];
        toUpperStr(s2, keyword);
        for (int i = 0; i < dsDauSach.n; i++) {
            toUpperStr(s1, dsDauSach.nodes[i]->tenSach);
            if (strstr(s1, s2) != nullptr) return dsDauSach.nodes[i];
        }
        return nullptr;
    }

    static void extractISBN(const char* maSach, char* isbnOut) {
        int i = 0;
        while (maSach[i] != '\0' && maSach[i] != '_') { isbnOut[i] = maSach[i]; i++; }
        isbnOut[i] = '\0';
    }

    DauSach* findDauSachByMaSach(const char* maSach, NodeSach*& outNodeSach) {
        char isbn[20];
        extractISBN(maSach, isbn);
        outNodeSach = nullptr;
        const char* usTarget = strchr(maSach, '_');
        int targetSuffix = usTarget ? atoi(usTarget + 1) : -1;

        for (int i = 0; i < dsDauSach.n; i++) {
            if (_stricmp(dsDauSach.nodes[i]->ISBN, isbn) == 0) {
                NodeSach* p = dsDauSach.nodes[i]->dms;
                while (p) {
                    const char* usCurr = strchr(p->data.maSach, '_');
                    int currSuffix = usCurr ? atoi(usCurr + 1) : -1;
                    if (currSuffix == targetSuffix) {
                        outNodeSach = p;
                        return dsDauSach.nodes[i];
                    }
                    if (currSuffix > targetSuffix) break;
                    p = p->next;
                }
                return dsDauSach.nodes[i];
            }
        }
        return nullptr;
    }

    static void checkBorrowStatus(TheDocGia& dg, int& countBorrowing, bool& hasOverdue) {
        countBorrowing = 0; hasOverdue = false;
        Date today = getCurrentDate();
        NodeMuonTra* p = dg.dsMuonTra;
        while (p) {
            if (p->data.trangThai == DANG_MUON) {
                countBorrowing++;
                if (daysBetween(p->data.ngayMuon, today) > 7) hasOverdue = true;
            }
            p = p->next;
        }
    }

    const char* borrowBook(int maThe, const char* maSach) {
        NodeDocGia* dg = searchDocGia(maThe);
        if (!dg) return "Khong tim thay ma the doc gia!";
        if (dg->data.trangThai == KHOA) return "The doc gia dang bi KHOA!";

        int countBorrow = 0; bool overdue = false;
        checkBorrowStatus(dg->data, countBorrow, overdue);
        if (countBorrow >= 3) return "Doc gia da muon toi da 3 cuon!";
        if (overdue) return "Doc gia dang giu sach qua han (>7 ngay)!";

        NodeSach* nodeSach = nullptr;
        DauSach* dauSach = findDauSachByMaSach(maSach, nodeSach);
        if (!dauSach || !nodeSach) return "Ma sach khong ton tai trong thu vien!";
        if (nodeSach->data.trangThai == DA_CHO_MUON) return "Cuon sach nay da co nguoi muon!";
        if (nodeSach->data.trangThai == DA_THANH_LY) return "Cuon sach nay da thanh ly!";

        char targetISBN[20], curISBN[20];
        extractISBN(maSach, targetISBN);
        NodeMuonTra* mt = dg->data.dsMuonTra;
        while (mt) {
            if (mt->data.trangThai == DANG_MUON) {
                extractISBN(mt->data.maSach, curISBN);
                if (_stricmp(targetISBN, curISBN) == 0)
                    return "Doc gia dang muon 1 cuon thuoc dau sach nay roi!";
            }
            mt = mt->next;
        }

        nodeSach->data.trangThai = DA_CHO_MUON;
        dauSach->soLuotMuon++;

        NodeMuonTra* newMT = new NodeMuonTra;
        strcpy(newMT->data.maSach, nodeSach->data.maSach);
        newMT->data.ngayMuon = getCurrentDate();
        newMT->data.ngayTra = { 0, 0, 0 };
        newMT->data.trangThai = DANG_MUON;
        newMT->next = dg->data.dsMuonTra;
        dg->data.dsMuonTra = newMT;

        return "OK";
    }

    const char* returnBook(int maThe, const char* maSach, int trangThaiMoi) {
        NodeDocGia* dg = searchDocGia(maThe);
        if (!dg) return "Khong tim thay ma the doc gia!";
        NodeMuonTra* mt = dg->data.dsMuonTra;
        while (mt) {
            if (_stricmp(mt->data.maSach, maSach) == 0 && mt->data.trangThai == DANG_MUON) {
                mt->data.trangThai = (trangThaiMoi == 1) ? DA_TRA : LAM_MAT;
                mt->data.ngayTra = getCurrentDate();
                NodeSach* nodeSach = nullptr;
                findDauSachByMaSach(maSach, nodeSach);
                if (nodeSach) nodeSach->data.trangThai = (trangThaiMoi == 1) ? CHO_MUON_DUOC : DA_THANH_LY;
                if (trangThaiMoi == 2) dg->data.trangThai = KHOA;
                return "OK";
            }
            mt = mt->next;
        }
        return "Doc gia khong muon ma sach nay!";
    }

    void getOverdueList(OverdueRecord list[], int& count) {
        count = 0;
        NodeDocGia** dgArr = new NodeDocGia*[MAX_DOC_GIA];
        int nDG = 0;
        getReadersList(dgArr, nDG, false);
        Date today = getCurrentDate();

        for (int i = 0; i < nDG; i++) {
            NodeMuonTra* mt = dgArr[i]->data.dsMuonTra;
            while (mt) {
                if (mt->data.trangThai == DANG_MUON) {
                    int diff = daysBetween(mt->data.ngayMuon, today);
                    if (diff > 7) {
                        list[count].maThe = dgArr[i]->data.maThe;
                        sprintf(list[count].hoTen, "%s %s", dgArr[i]->data.ho, dgArr[i]->data.ten);
                        strcpy(list[count].maSach, mt->data.maSach);
                        NodeSach* ns = nullptr;
                        DauSach* ds = findDauSachByMaSach(mt->data.maSach, ns);
                        strcpy(list[count].tenSach, ds ? ds->tenSach : "N/A");
                        list[count].ngayMuon = mt->data.ngayMuon;
                        list[count].soNgayQuaHan = diff - 7;
                        count++;
                    }
                }
                mt = mt->next;
            }
        }
        delete[] dgArr;

        for (int i = 0; i < count - 1; i++) {
            for (int j = i + 1; j < count; j++) {
                if (list[i].soNgayQuaHan < list[j].soNgayQuaHan) {
                    OverdueRecord tmp = list[i]; list[i] = list[j]; list[j] = tmp;
                }
            }
        }
    }

    void getTop10Books(DauSach* outArr[], int& count) {
        int total = dsDauSach.n;
        for (int i = 0; i < total; i++) outArr[i] = dsDauSach.nodes[i];
        for (int i = 0; i < total - 1; i++) {
            for (int j = i + 1; j < total; j++) {
                if (outArr[i]->soLuotMuon < outArr[j]->soLuotMuon) {
                    DauSach* t = outArr[i]; outArr[i] = outArr[j]; outArr[j] = t;
                }
            }
        }
        count = (total < 10) ? total : 10;
    }
};

#endif