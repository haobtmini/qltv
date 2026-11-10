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
class KeyManager {
private:
    struct Node {
        int key;
        Node* next;
        Node(int k) : key(k), next(nullptr) {}
    };
    Node* head;  
    int maxKey; 

public:
    KeyManager() : head(nullptr), maxKey(1) {}

    void init(int startKey) { maxKey = startKey; }

    int getNewKey() {
        if (head) {
            int reusedKey = head->key;
            Node* temp = head;
            head = head->next;
            delete temp;
            return reusedKey;
        }
        return maxKey++;
    }

    void returnKey(int key) {
        Node* newNode = new Node(key);
        newNode->next = head;
        head = newNode;
    }

    void saveToFile(FILE* f) {
        if (!f) return;
        int count = 0;
        Node* p = head;
        while (p) { count++; p = p->next; }
        
        fprintf(f, "%d %d\n", maxKey, count);
        
        p = head;
        while (p) {
            fprintf(f, "%d ", p->key);
            p = p->next;
        }
        if (count > 0) fprintf(f, "\n");
    }

    void loadFromFile(FILE* f) {
        if (!f) return;
        
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }

        int count = 0;
        if (fscanf(f, "%d %d", &maxKey, &count) == 2) {
            if (count > 0) {
                int* tempArr = new int[count];
                for (int i = 0; i < count; i++) {
                    fscanf(f, "%d", &tempArr[i]);
                }
                for (int i = count - 1; i >= 0; i--) {
                    returnKey(tempArr[i]);
                }
                delete[] tempArr;
            }
        }
        
        int c;
        while ((c = fgetc(f)) != '\n' && c != EOF);
    }

    ~KeyManager() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
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
    KeyManager kmSach;
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
    int height;
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