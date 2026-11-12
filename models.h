#ifndef MODELS_H
#define MODELS_H

#include <ctime>
#include <cstdio>

#define MAX_DAUSACH 1000
#define MAX_KEY_POOL 10000

struct Date {
    int d, m, y;
};

inline Date getCurrentDate() {
    time_t t = time(0);
    tm* now = localtime(&t);
    return { now->tm_mday, now->tm_mon + 1, now->tm_year + 1900 };
}

inline long dateToDays(Date dt) {
    int y = dt.y, m = dt.m, d = dt.d;
    if (m < 3) { y--; m += 12; }
    return 365L * y + y / 4 - y / 100 + y / 400 + (153 * (m - 3) + 2) / 5 + d - 1;
}

inline int daysBetween(Date d1, Date d2) {
    return (int)(dateToDays(d2) - dateToDays(d1));
}

class BinaryKeyQueue {
private:
    struct RangeNode {
        int l, r;
        RangeNode* next;
        RangeNode(int _l, int _r) : l(_l), r(_r), next(nullptr) {}
    };
    struct ReuseNode {
        int key;
        ReuseNode* next;
        ReuseNode(int k) : key(k), next(nullptr) {}
    };

    RangeNode *qHead, *qTail;
    ReuseNode *reuseHead;
    int maxLimit;

    void enqueue(int l, int r) {
        if (l > r) return;
        RangeNode* node = new RangeNode(l, r);
        if (!qTail) qHead = qTail = node;
        else { qTail->next = node; qTail = node; }
    }

    bool dequeue(int& l, int& r) {
        if (!qHead) return false;
        RangeNode* temp = qHead;
        l = temp->l; r = temp->r;
        qHead = qHead->next;
        if (!qHead) qTail = nullptr;
        delete temp;
        return true;
    }

    void clearAll() {
        int l, r;
        while (dequeue(l, r));
        while (reuseHead) {
            ReuseNode* temp = reuseHead;
            reuseHead = reuseHead->next;
            delete temp;
        }
    }

public:
    BinaryKeyQueue() : qHead(nullptr), qTail(nullptr), reuseHead(nullptr), maxLimit(1023) {}

    void init(int limit = 1023) {
        clearAll();
        maxLimit = limit;
        enqueue(1, maxLimit);
    }

    int getNewKey() {
        if (reuseHead) {
            int k = reuseHead->key;
            ReuseNode* temp = reuseHead;
            reuseHead = reuseHead->next;
            delete temp;
            return k;
        }
        int l, r;
        if (dequeue(l, r)) {
            int mid = l + (r - l) / 2;
            enqueue(l, mid - 1);
            enqueue(mid + 1, r);
            return mid;
        }
        int oldLimit = maxLimit;
        maxLimit = maxLimit * 2 + 1;
        enqueue(oldLimit + 1, maxLimit);
        return getNewKey();
    }

    void returnKey(int key) {
        ReuseNode* node = new ReuseNode(key);
        node->next = reuseHead;
        reuseHead = node;
    }

    void saveToFile(FILE* f) {
        if (!f) return;
        int rCount = 0, qCount = 0;
        for (ReuseNode* p = reuseHead; p; p = p->next) rCount++;
        for (RangeNode* p = qHead; p; p = p->next) qCount++;

        fprintf(f, "%d %d %d\n", maxLimit, rCount, qCount);
        for (ReuseNode* p = reuseHead; p; p = p->next) fprintf(f, "%d ", p->key);
        if (rCount > 0) fprintf(f, "\n");
        for (RangeNode* p = qHead; p; p = p->next) fprintf(f, "%d %d ", p->l, p->r);
        if (qCount > 0) fprintf(f, "\n");
    }

    void loadFromFile(FILE* f) {
        if (!f) return;
        clearAll();
        int rCount = 0, qCount = 0;
        if (fscanf(f, "%d %d %d", &maxLimit, &rCount, &qCount) == 3) {
            if (rCount > 0) {
                int* tmp = new int[rCount];
                for (int i = 0; i < rCount; i++) fscanf(f, "%d", &tmp[i]);
                for (int i = rCount - 1; i >= 0; i--) returnKey(tmp[i]);
                delete[] tmp;
            }
            for (int i = 0; i < qCount; i++) {
                int l, r;
                if (fscanf(f, "%d %d", &l, &r) == 2) enqueue(l, r);
            }
        }
        int c;
        while ((c = fgetc(f)) != '\n' && c != EOF);
    }

    ~BinaryKeyQueue() { clearAll(); }
};

enum TrangThaiSach {
    CHO_MUON_DUOC = 0,
    DA_CHO_MUON = 1,
    DA_THANH_LY = 2
};

enum TrangThaiMuonTra {
    DANG_MUON = 0,
    DA_TRA = 1,
    LAM_MAT = 2
};

enum TrangThaiThe {
    KHOA = 0,
    HOAT_DONG = 1
};

struct Sach {
    char maSach[25];
    TrangThaiSach trangThai;
    char viTri[30];
};

struct NodeSach {
    Sach data;
    NodeSach* next;
};

struct DauSach {
    char ISBN[15];
    char tenSach[50];
    int soTrang;
    char tacGia[40];
    int namXB;
    char theLoai[30];
    NodeSach* dms;
    int soLuongSach;
    int soLuotMuon;
};

struct ListDauSach {
    int n = 0;
    DauSach* nodes[MAX_DAUSACH];
};

struct MuonTra {
    char maSach[25];
    Date ngayMuon;
    Date ngayTra;
    TrangThaiMuonTra trangThai;
};

struct NodeMuonTra {
    MuonTra data;
    NodeMuonTra* next;
};

struct TheDocGia {
    int maThe;
    char ho[40];
    char ten[20];
    char phai[10];
    TrangThaiThe trangThai;
    NodeMuonTra* dsMuonTra;
};

struct NodeDocGia {
    TheDocGia data;
    NodeDocGia* left;
    NodeDocGia* right;
};

struct OverdueRecord {
    int maThe;
    char hoTen[65];
    char maSach[25];
    char tenSach[50];
    Date ngayMuon;
    int soNgayQuaHan;
};

template <typename T>
struct CustomStack {
    struct Node {
        T data;
        Node* next;
    };
    Node* topNode = nullptr;

    bool isEmpty() { return topNode == nullptr; }
    void push(T val) { topNode = new Node{ val, topNode }; }
    T pop() {
        if (isEmpty()) return T();
        Node* p = topNode;
        T val = p->data;
        topNode = topNode->next;
        delete p;
        return val;
    }
    ~CustomStack() { while (!isEmpty()) pop(); }
};

#endif