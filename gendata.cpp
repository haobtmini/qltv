// gendata.cpp — Gen 100.000 độc giả, 100 đầu sách, 10–50 cuốn/đầu sách
// Build: g++ -std=c++17 -O2 gendata.cpp -o gendata
// Run:   ./gendata

#include "logic.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <string>

// ==================== CẤU HÌNH ====================
static const int NUM_DOCGIA   = 100000;    // ← 10^5
static const int NUM_DAUSACH  = 100;
static const int MIN_SACH     = 10;
static const int MAX_SACH     = 50;
static const double RATIO_CO_MUON   = 0.35;
static const int    MAX_MUON_PER_DG = 3;
static const double RATIO_DANG_MUON = 0.15;

// ==================== DỮ LIỆU MẪU ====================
static const char* HO[] = {
    "Nguyen","Tran","Le","Pham","Hoang","Huynh","Phan","Vu","Vo","Dang",
    "Bui","Do","Ho","Ngo","Duong","Ly","Truong","Dinh","Lam","Mai"
};
static const int N_HO = sizeof(HO)/sizeof(HO[0]);

static const char* TEN_NAM[] = {
    "An","Bao","Cuong","Dung","Duc","Hai","Hieu","Hoang","Hung","Huy",
    "Khang","Kien","Long","Minh","Nam","Nghia","Phong","Phuc","Quan","Quang",
    "Son","Tai","Tam","Thanh","Thang","Thinh","Thong","Tien","Toan","Trung"
};
static const int N_TEN_NAM = sizeof(TEN_NAM)/sizeof(TEN_NAM[0]);

static const char* TEN_NU[] = {
    "Anh","Bich","Chau","Chi","Diep","Dung","Giang","Ha","Hanh","Hoa",
    "Hue","Huong","Lan","Linh","Loan","Mai","My","Nga","Ngan","Ngoc",
    "Nhi","Nhung","Oanh","Phuong","Quynh","Tam","Thao","Thi","Thu","Thuy"
};
static const int N_TEN_NU = sizeof(TEN_NU)/sizeof(TEN_NU[0]);

static const char* THE_LOAI[] = {
    "Van hoc","Khoa hoc","Lich su","Dia ly","Toan hoc","Vat ly","Hoa hoc",
    "Sinh hoc","Triet hoc","Kinh te","Tam ly","Ky nang song","Cong nghe",
    "Tin hoc","Nghe thuat","Ton giao","Chinh tri","Giao duc","The thao","Am nhac"
};
static const int N_THE_LOAI = sizeof(THE_LOAI)/sizeof(THE_LOAI[0]);

static const char* TINH_TU[] = {
    "Co dai","Hien dai","Viet Nam","The gioi","Nhap mon","Nang cao",
    "Thuc hanh","Ly thuyet","Toan dien","Chuyen sau","Can ban","Ung dung",
    "Thu vi","Hap dan","Ky ao","Lich su","Tuong lai","Song ngu"
};
static const int N_TINH_TU = sizeof(TINH_TU)/sizeof(TINH_TU[0]);

static const char* DANH_TU_SACH[] = {
    "Hanh trinh","Kham pha","Bai hoc","Cau chuyen","The gioi","Tu duy",
    "Ky nang","Phuong phap","Nghe thuat","Khoa hoc","Lich su","Van hoa",
    "Con nguoi","Cuoc song","Tuong lai","Tri thuc","Nhan thuc","Giao duc"
};
static const int N_DANH_TU = sizeof(DANH_TU_SACH)/sizeof(DANH_TU_SACH[0]);

static const char* TAC_GIA_HO[] = {
    "Nguyen Van","Tran Thi","Le Quang","Pham Minh","Hoang Anh",
    "Vu Duc","Dang Thu","Bui Xuan","Do Hai","Ngo Bao"
};
static const int N_TG_HO = sizeof(TAC_GIA_HO)/sizeof(TAC_GIA_HO[0]);

// ==================== TIỆN ÍCH ====================
static int randRange(int lo, int hi) { return lo + rand() % (hi - lo + 1); }
static double randDouble() { return (double)rand() / ((double)RAND_MAX + 1.0); }
static bool randChance(double p) { return randDouble() < p; }
static const char* pick(const char* arr[], int n) { return arr[rand() % n]; }

// Date helper — KHÔNG dựa vào models.h để tránh phụ thuộc
struct SimpleDate { int d, m, y; };
static SimpleDate genPastDate(int minDaysAgo, int maxDaysAgo) {
    time_t now = time(nullptr);
    int daysAgo = randRange(minDaysAgo, maxDaysAgo);
    time_t t = now - (time_t)daysAgo * 86400;
    struct tm* tm = localtime(&t);
    SimpleDate r; r.d = tm->tm_mday; r.m = tm->tm_mon + 1; r.y = tm->tm_year + 1900;
    return r;
}
static SimpleDate genDateAfterDaysAgo(int daysAgo, int plusDays) {
    time_t now = time(nullptr);
    time_t t = now - (time_t)daysAgo * 86400 + (time_t)plusDays * 86400;
    struct tm* tm = localtime(&t);
    SimpleDate r; r.d = tm->tm_mday; r.m = tm->tm_mon + 1; r.y = tm->tm_year + 1900;
    return r;
}

// ==================== MAIN ====================
int main() {
    // ✅ Fix codepage trên Windows để hiển thị đúng (không bị rác)
#ifdef _WIN32
    system("chcp 437 > nul");   // logic.h dùng CP437 cho khung
#endif

    srand((unsigned)time(nullptr));

    printf("=== GEN DATA THU VIEN ===\n");
    printf("Doc gia  : %d\n", NUM_DOCGIA);
    printf("Dau sach : %d\n", NUM_DAUSACH);
    printf("Sach/dau : %d - %d\n\n", MIN_SACH, MAX_SACH);

    LibraryService svc;
    svc.clearData();

    // ---------- 1. ĐẦU SÁCH ----------
    printf("[1/3] Tao %d dau sach...\n", NUM_DAUSACH);

    // ✅ Mảng động C-style (không vector)
    DauSach** dsList = new DauSach*[NUM_DAUSACH];
    int dsCount = 0;

    for (int i = 0; i < NUM_DAUSACH; i++) {
        DauSach* ds = new DauSach();

        // ✅ ISBN: đảm bảo không tràn buffer (giả định ISBN[20] trong models.h)
        snprintf(ds->ISBN, sizeof(ds->ISBN), "S%03d", i + 1);       

        // ✅ Ten sach: snprintf LUÔN đảm bảo '\0' + không tràn
        // Giả định tenSach[50]
        snprintf(ds->tenSach, 50, "%s %s %s (Tap %d)",
                 pick(DANH_TU_SACH, N_DANH_TU),
                 pick(THE_LOAI, N_THE_LOAI),
                 pick(TINH_TU, N_TINH_TU),
                 i + 1);

        // Giả định tacGia[30]
        snprintf(ds->tacGia, 30, "%s %s",
                 pick(TAC_GIA_HO, N_TG_HO),
                 pick(TEN_NAM, N_TEN_NAM));

        // Giả định theLoai[30]
        snprintf(ds->theLoai, 30, "%s", pick(THE_LOAI, N_THE_LOAI));

        ds->soTrang     = randRange(80, 800);
        ds->namXB       = randRange(1990, 2024);
        ds->soLuongSach = 0;
        ds->soLuotMuon  = 0;
        ds->dms         = nullptr;

        if (!svc.insertDauSach(ds)) {
            fprintf(stderr, "LOI: trung ISBN %s\n", ds->ISBN);
            delete ds;
            continue;
        }

        dsList[dsCount++] = ds;

        // Tạo sách vật lý
        int soCuon = randRange(MIN_SACH, MAX_SACH);
        char viTri[20];
        snprintf(viTri, sizeof(viTri), "Ke %c%d", (char)('A' + (i % 10)), randRange(1, 5));
        svc.addSachAuto(ds, soCuon, viTri);

        if ((i + 1) % 20 == 0) printf("      ... %d/%d\n", i + 1, NUM_DAUSACH);
    }
    printf("      Xong: %d dau sach\n\n", dsCount);

    // ---------- 2. ĐỘC GIẢ ----------
    printf("[2/3] Tao %d doc gia...\n", NUM_DOCGIA);

    for (int i = 0; i < NUM_DOCGIA; i++) {
        TheDocGia dg;
        // ✅ zero-init để tránh byte rác trong struct
        memset(&dg, 0, sizeof(dg));

        dg.maThe = svc.generateReaderKey();

        bool isNam = randChance(0.5);

        // Giả định các buffer: ho[30], ten[20], phai[10]
        snprintf(dg.ho,   30, "%s", pick(HO, N_HO));
        snprintf(dg.ten,  20, "%s",
                 isNam ? pick(TEN_NAM, N_TEN_NAM) : pick(TEN_NU, N_TEN_NU));
        snprintf(dg.phai, 10, "%s", isNam ? "Nam" : "Nu");

        dg.trangThai = randChance(0.03) ? KHOA : HOAT_DONG;
        dg.dsMuonTra = nullptr;

        // Mượn sách
        if (dg.trangThai == HOAT_DONG && randChance(RATIO_CO_MUON)) {
            int nMuon = randRange(1, MAX_MUON_PER_DG);
            NodeMuonTra* tailMT = nullptr;
            int borrowedThisRound = 0;

            for (int k = 0; k < nMuon; k++) {
                if (dsCount == 0) break;
                DauSach* ds = dsList[rand() % dsCount];
                if (!ds || !ds->dms) continue;

                // Đếm sách
                int total = 0;
                for (NodeSach* p = ds->dms; p; p = p->next) total++;
                if (total == 0) continue;

                // Chọn 1 cuốn
                int pickIdx = rand() % total;
                NodeSach* ns = ds->dms;
                for (int t = 0; t < pickIdx && ns; t++) ns = ns->next;
                if (!ns) continue;

                // ✅ Cấp phát và zero-init NodeMuonTra
                NodeMuonTra* mt = new NodeMuonTra();
                memset(&mt->data, 0, sizeof(mt->data));
                mt->next = nullptr;

                snprintf(mt->data.maSach, 25, "%s", ns->data.maSach);

                bool dangMuon = randChance(RATIO_DANG_MUON);

                if (dangMuon) {
                    if (ns->data.trangThai != CHO_MUON_DUOC) {
                        delete mt;
                        continue;
                    }
                    SimpleDate dm = genPastDate(0, 60);
                    mt->data.ngayMuon.d = dm.d;
                    mt->data.ngayMuon.m = dm.m;
                    mt->data.ngayMuon.y = dm.y;
                    mt->data.ngayTra.d = 0;
                    mt->data.ngayTra.m = 0;
                    mt->data.ngayTra.y = 0;
                    mt->data.trangThai = DANG_MUON;

                    ns->data.trangThai = DA_CHO_MUON;
                    ds->soLuotMuon++;
                    borrowedThisRound++;
                } else {
                    // Lịch sử đã trả
                    int muonDaysAgo = randRange(10, 400);
                    int duration = randRange(1, 30);
                    SimpleDate dm = genPastDate(muonDaysAgo, muonDaysAgo);
                    SimpleDate dt = genDateAfterDaysAgo(muonDaysAgo, duration);

                    mt->data.ngayMuon.d = dm.d;
                    mt->data.ngayMuon.m = dm.m;
                    mt->data.ngayMuon.y = dm.y;
                    mt->data.ngayTra.d = dt.d;
                    mt->data.ngayTra.m = dt.m;
                    mt->data.ngayTra.y = dt.y;
                    mt->data.trangThai = randChance(0.02) ? LAM_MAT : DA_TRA;

                    ds->soLuotMuon++;
                }

                if (!dg.dsMuonTra) dg.dsMuonTra = mt;
                else tailMT->next = mt;
                tailMT = mt;
            }
        }

        svc.addDocGia(dg);

        if ((i + 1) % 20000 == 0) printf("      ... %d/%d\n", i + 1, NUM_DOCGIA);
    }
    printf("      Xong: %d doc gia\n\n", NUM_DOCGIA);

    // ---------- 3. LƯU ----------
    printf("[3/3] Dang ghi file...\n");
    svc.saveToFile();
    printf("      Hoan tat!\n\n");

    printf("=== KET QUA ===\n");
    printf("Doc gia  : %d\n", NUM_DOCGIA);
    printf("Dau sach : %d\n", dsCount);
    int totalSach = 0;
    for (int i = 0; i < dsCount; i++) totalSach += dsList[i]->soLuongSach;
    printf("Tong sach: %d\n", totalSach);

    // ✅ Giải phóng mảng động
    delete[] dsList;

    return 0;
}