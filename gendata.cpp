// gendata.cpp — Gen 100.000 độc giả KHÔNG TRÙNG HỌ VÀ TÊN
// Không gian tên: 70 họ × 60 đệm × 200 tên = 840.000 tổ hợp
// Build: g++ -std=c++17 -O2 gendata.cpp -o gendata
// Run:   ./gendata

#include "logic.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>

// ==================== CẤU HÌNH ====================
static const int    NUM_DOCGIA      = 100000;
static const int    NUM_DAUSACH     = 100;
static const int    MIN_SACH        = 10;
static const int    MAX_SACH        = 50;
static const double RATIO_CO_MUON   = 0.35;
static const int    MAX_MUON_PER_DG = 3;
static const double RATIO_DANG_MUON = 0.25;

// ==================== HỌ (70) ====================
static const char* HO[] = {
    "Nguyen","Tran","Le","Pham","Hoang","Huynh","Phan","Vu","Vo","Dang",
    "Bui","Do","Ho","Ngo","Duong","Ly","Truong","Dinh","Lam","Mai",
    "Trinh","Dao","Doan","Vuong","Ton","Luong","Thai","Ta","Chau","Quach",
    "Phung","Ha","Tang","Mac","Kieu",
    // ===== Thêm 35 họ =====
    "La","Dam","Trieu","Quan","Giap","Khuc","Ninh","Ong","Luc","Vi",
    "Ban","Cam","Chu","Danh","Diep","Giang","Hac","Han","Hoa","Khong",
    "Lac","Lieu","Lo","Ma","Nghiem","Nhan","Phi","Phuong","So","Tam",
    "Thach","Thieu","Ung","Van","Xa"
};
static const int N_HO = sizeof(HO) / sizeof(HO[0]); // 70

// ==================== ĐỆM NAM (30) ====================
static const char* DEM_NAM[] = {
    "Van","Huu","Duc","Minh","Quang","Hoang","Gia","Quoc","Thanh","Tuan",
    "Bao","Anh","Cong","Duy","Ngoc",
    // ===== Thêm 15 đệm nam =====
    "Dinh","Khanh","Phu","Phuc","Tai","Thien","Trong","Trung","Tu","Vien",
    "Vu","Xuan","Binh","Cuong","Hieu"
};
static const int N_DEM_NAM = sizeof(DEM_NAM) / sizeof(DEM_NAM[0]); // 30

// ==================== ĐỆM NỮ (30) ====================
static const char* DEM_NU[] = {
    "Thi","Ngoc","Thu","Thanh","Minh","Phuong","Hong","Kim","My","Bao",
    "Anh","Thuy","Khanh","Hoai","Dieu",
    // ===== Thêm 15 đệm nữ =====
    "Bich","Cam","Dao","Giang","Ha","Hanh","Hoa","Hue","Huong","Lan",
    "Linh","Loan","Mai","Nga","Yen"
};
static const int N_DEM_NU = sizeof(DEM_NU) / sizeof(DEM_NU[0]); // 30

// ==================== TÊN NAM (100) ====================
static const char* TEN_NAM[] = {
    // 50 tên cũ
    "An","Bao","Cuong","Dung","Duc","Hai","Hieu","Hoang","Hung","Huy",
    "Khang","Kien","Long","Minh","Nam","Nghia","Phong","Phuc","Quan","Quang",
    "Son","Tai","Tam","Thanh","Thang","Thinh","Thong","Tien","Toan","Trung",
    "Tuan","Tu","Tung","Viet","Vinh","Vu","Dat","Duy","Giang","Hao",
    "Hiep","Khoa","Lam","Linh","Loc","Manh","Nhat","Phat","Sang","Tri",
    // ===== Thêm 50 tên nam =====
    "Bach","Bang","Binh","Cao","Chien","Chinh","Chung","Danh","Dao","Dien",
    "Dong","Duyen","Gia","Hieu","Hoa","Hoan","Hong","Hung","Kien","Lac",
    "Lai","Lan","Lap","Loi","Luong","My","Nghiem","Ngoc","Nhan","Ninh",
    "Phan","Phi","Phu","Quoc","Quy","Si","Tan","Thach","Thien","Tho",
    "Thu","Tin","Toan","Tri","Tuan","Tuong","Uy","Vinh","Vu","Xuan"
};
static const int N_TEN_NAM = sizeof(TEN_NAM) / sizeof(TEN_NAM[0]); // 100

// ==================== TÊN NỮ (100) ====================
static const char* TEN_NU[] = {
    // 50 tên cũ
    "Anh","Bich","Chau","Chi","Diep","Dung","Giang","Ha","Hanh","Hoa",
    "Hue","Huong","Lan","Linh","Loan","Mai","My","Nga","Ngan","Ngoc",
    "Nhi","Nhung","Oanh","Phuong","Quynh","Tam","Thao","Thi","Thu","Thuy",
    "Tien","Tram","Trang","Trinh","Truc","Tu","Tuyen","Tuyet","Uyen","Van",
    "Vi","Vy","Xuan","Yen","Han","Khanh","Ly","Nhu","Suong","Thuyet",
    // ===== Thêm 50 tên nữ =====
    "Bao","Binh","Cam","Dao","Diem","Diep","Dong","Gam","Giao","Ha",
    "Hai","Hang","Hao","Hien","Hoa","Hoai","Hong","Hop","Hue","Huyen",
    "Kieu","Lam","Le","Lien","Lieu","Linh","Loan","Mai","Man","Minh",
    "My","Nga","Ngoc","Nhan","Nhi","Nhung","Oanh","Phuong","Quynh","Sang",
    "Tam","Thanh","Thao","Thi","Thu","Thuy","Tien","Trang","Truc","Van"
};
static const int N_TEN_NU = sizeof(TEN_NU) / sizeof(TEN_NU[0]); // 100

// ==================== DỮ LIỆU MẪU ĐẦU SÁCH ====================
static const char* THE_LOAI[] = {
    "Van hoc","Khoa hoc","Lich su","Dia ly","Toan hoc","Vat ly","Hoa hoc",
    "Sinh hoc","Triet hoc","Kinh te","Tam ly","Ky nang song","Cong nghe",
    "Tin hoc","Nghe thuat","Ton giao","Chinh tri","Giao duc","The thao","Am nhac"
};
static const int N_THE_LOAI = sizeof(THE_LOAI) / sizeof(THE_LOAI[0]);

static const char* TINH_TU[] = {
    "Co dai","Hien dai","Viet Nam","The gioi","Nhap mon","Nang cao",
    "Thuc hanh","Ly thuyet","Toan dien","Chuyen sau","Can ban","Ung dung",
    "Thu vi","Hap dan","Ky ao","Lich su","Tuong lai","Song ngu"
};
static const int N_TINH_TU = sizeof(TINH_TU) / sizeof(TINH_TU[0]);

static const char* DANH_TU_SACH[] = {
    "Hanh trinh","Kham pha","Bai hoc","Cau chuyen","The gioi","Tu duy",
    "Ky nang","Phuong phap","Nghe thuat","Khoa hoc","Lich su","Van hoa",
    "Con nguoi","Cuoc song","Tuong lai","Tri thuc","Nhan thuc","Giao duc"
};
static const int N_DANH_TU = sizeof(DANH_TU_SACH) / sizeof(DANH_TU_SACH[0]);

static const char* TAC_GIA_HO[] = {
    "Nguyen Van","Tran Thi","Le Quang","Pham Minh","Hoang Anh",
    "Vu Duc","Dang Thu","Bui Xuan","Do Hai","Ngo Bao"
};
static const int N_TG_HO = sizeof(TAC_GIA_HO) / sizeof(TAC_GIA_HO[0]);

// ==================== TIỆN ÍCH ====================
static int randRange(int lo, int hi) { return lo + rand() % (hi - lo + 1); }
static double randDouble() { return (double)rand() / ((double)RAND_MAX + 1.0); }
static bool randChance(double p) { return randDouble() < p; }
static const char* pick(const char* arr[], int n) { return arr[rand() % n]; }

static Date genDateOffset(int daysOffset) {
    time_t t = time(nullptr) + (time_t)daysOffset * 86400;
    struct tm* tm = localtime(&t);
    return { tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900 };
}

// ==================== MAIN ====================
int main() {
#ifdef _WIN32
    system("chcp 437 > nul");
#endif
    srand((unsigned)time(nullptr));

    const int TOTAL_DEM = N_DEM_NAM + N_DEM_NU; // 60
    const int TOTAL_TEN = N_TEN_NAM + N_TEN_NU; // 200
    const long long TOTAL_COMBOS = (long long)N_HO * TOTAL_DEM * TOTAL_TEN; // 840.000

    // STEP phải nguyên tố cùng nhau với TOTAL_COMBOS = 840.000 = 2^5 * 3 * 5^4 * 7
    // Chọn STEP không chia hết cho 2, 3, 5, 7 -> song ánh 1-1
    const long long STEP = 31337;
    const long long SEED = rand() % TOTAL_COMBOS;

    printf("=== GEN DATA THU VIEN (100%% KHONG TRUNG HO & TEN) ===\n");
    printf("Doc gia  : %d (To hop toi da: %lld)\n", NUM_DOCGIA, TOTAL_COMBOS);
    printf("Dau sach : %d\n", NUM_DAUSACH);
    printf("Sach/dau : %d - %d\n\n", MIN_SACH, MAX_SACH);

    LibraryService svc;
    svc.clearData();

    // ---------- 1. ĐẦU SÁCH & DANH MỤC SÁCH ----------
    printf("[1/3] Tao %d dau sach...\n", NUM_DAUSACH);
    DauSach** dsList = new DauSach*[NUM_DAUSACH];
    int dsCount = 0;

    for (int i = 0; i < NUM_DAUSACH; i++) {
        DauSach* ds = new DauSach();
        memset(ds, 0, sizeof(DauSach));

        snprintf(ds->ISBN, sizeof(ds->ISBN), "978%04d", i + 1);
        snprintf(ds->tenSach, sizeof(ds->tenSach), "%s %s %s (T%d)",
                 pick(DANH_TU_SACH, N_DANH_TU),
                 pick(THE_LOAI, N_THE_LOAI),
                 pick(TINH_TU, N_TINH_TU),
                 i + 1);
        snprintf(ds->tacGia, sizeof(ds->tacGia), "%s %s",
                 pick(TAC_GIA_HO, N_TG_HO),
                 pick(TEN_NAM, N_TEN_NAM));
        snprintf(ds->theLoai, sizeof(ds->theLoai), "%s", pick(THE_LOAI, N_THE_LOAI));

        ds->soTrang     = randRange(80, 800);
        ds->namXB       = randRange(1995, 2026);
        ds->soLuongSach = 0;
        ds->soLuotMuon  = 0;
        ds->dms         = nullptr;

        if (!svc.insertDauSach(ds)) {
            delete ds;
            continue;
        }
        dsList[dsCount++] = ds;

        int soCuon = randRange(MIN_SACH, MAX_SACH);
        char viTri[30];
        snprintf(viTri, sizeof(viTri), "Ke %c%d", (char)('A' + (i % 10)), randRange(1, 5));
        svc.addSachAuto(ds, soCuon, viTri);

        if ((i + 1) % 25 == 0) printf("      ... %d/%d dau sach\n", i + 1, NUM_DAUSACH);
    }
    printf("      Xong: %d dau sach\n\n", dsCount);

    // ---------- 2. ĐỘC GIẢ & LỊCH SỬ MƯỢN TRẢ ----------
    printf("[2/3] Tao %d doc gia...\n", NUM_DOCGIA);

    for (int i = 0; i < NUM_DOCGIA; i++) {
        TheDocGia dg;
        memset(&dg, 0, sizeof(dg));

        dg.maThe = svc.generateReaderKey();

        // Ánh xạ 1-1 từ chỉ số i -> comboIdx duy nhất trong [0 .. TOTAL_COMBOS-1]
        long long comboIdx = (SEED + (long long)i * STEP) % TOTAL_COMBOS;

        int idxHo  = (int)(comboIdx % N_HO);
        comboIdx  /= N_HO;
        int idxDem = (int)(comboIdx % TOTAL_DEM);
        int idxTen = (int)(comboIdx / TOTAL_DEM);

        bool isNam = (idxTen < N_TEN_NAM);
        const char* hoStr  = HO[idxHo];
        const char* tenStr = isNam ? TEN_NAM[idxTen] : TEN_NU[idxTen - N_TEN_NAM];

        // Giữ tổng số trạng thái đệm đúng 60 mà vẫn ưu tiên đúng giới tính
        const char* demStr = nullptr;
        if (isNam) {
            demStr = (idxDem < N_DEM_NAM) ? DEM_NAM[idxDem] : DEM_NU[idxDem - N_DEM_NAM];
        } else {
            demStr = (idxDem < N_DEM_NU) ? DEM_NU[idxDem] : DEM_NAM[idxDem - N_DEM_NU];
        }

        snprintf(dg.ho,   sizeof(dg.ho),   "%s %s", hoStr, demStr);
        snprintf(dg.ten,  sizeof(dg.ten),  "%s", tenStr);
        snprintf(dg.phai, sizeof(dg.phai), "%s", isNam ? "Nam" : "Nu");

        dg.trangThai = randChance(0.02) ? KHOA : HOAT_DONG;
        dg.dsMuonTra = nullptr;

        if (dg.trangThai == HOAT_DONG && randChance(RATIO_CO_MUON) && dsCount > 0) {
            int nMuon = randRange(1, MAX_MUON_PER_DG);
            NodeMuonTra* tailMT = nullptr;
            DauSach* pickedDS[MAX_MUON_PER_DG] = { nullptr };
            int pickedCount = 0;

            for (int k = 0; k < nMuon; k++) {
                DauSach* ds = dsList[rand() % dsCount];
                if (!ds || !ds->dms || ds->soLuongSach <= 0) continue;

                bool dupISBN = false;
                for (int p = 0; p < pickedCount; p++) {
                    if (pickedDS[p] == ds) { dupISBN = true; break; }
                }
                if (dupISBN) continue;
                pickedDS[pickedCount++] = ds;

                int pickIdx = rand() % ds->soLuongSach;
                NodeSach* ns = ds->dms;
                for (int t = 0; t < pickIdx && ns; t++) ns = ns->next;
                if (!ns || ns->data.trangThai != CHO_MUON_DUOC) continue;

                NodeMuonTra* mt = new NodeMuonTra();
                memset(mt, 0, sizeof(NodeMuonTra));
                snprintf(mt->data.maSach, sizeof(mt->data.maSach), "%s", ns->data.maSach);

                bool dangMuon = randChance(RATIO_DANG_MUON);
                if (dangMuon) {
                    int daysAgo = randRange(0, 20);
                    mt->data.ngayMuon = genDateOffset(-daysAgo);
                    mt->data.ngayTra = { 0, 0, 0 };
                    mt->data.trangThai = DANG_MUON;

                    ns->data.trangThai = DA_CHO_MUON;
                    ds->soLuotMuon++;
                } else {
                    int muonDaysAgo = randRange(15, 365);
                    int duration = randRange(1, 14);
                    mt->data.ngayMuon = genDateOffset(-muonDaysAgo);
                    mt->data.ngayTra = genDateOffset(-muonDaysAgo + duration);

                    if (randChance(0.01)) {
                        mt->data.trangThai = LAM_MAT;
                        ns->data.trangThai = DA_THANH_LY;
                        dg.trangThai = KHOA;
                    } else {
                        mt->data.trangThai = DA_TRA;
                    }
                    ds->soLuotMuon++;
                }

                if (!dg.dsMuonTra) dg.dsMuonTra = mt;
                else tailMT->next = mt;
                tailMT = mt;
            }
        }

        svc.addDocGia(dg);
        if ((i + 1) % 25000 == 0) printf("      ... %d/%d doc gia\n", i + 1, NUM_DOCGIA);
    }
    printf("      Xong: %d doc gia\n\n", NUM_DOCGIA);

    // ---------- 3. LƯU FILE ----------
    printf("[3/3] Dang ghi dausach.txt & docgia.txt...\n");
    svc.saveToFile();
    printf("      Hoan tat!\n\n");

    int totalSach = 0;
    for (int i = 0; i < dsCount; i++) totalSach += dsList[i]->soLuongSach;
    printf("=== TONG KET ===\n");
    printf("- Doc gia  : %d (Khong trung Ho & Ten)\n", NUM_DOCGIA);
    printf("- Dau sach : %d\n", dsCount);
    printf("- Tong sach: %d\n", totalSach);
    printf("- Khong gian ten: %lld to hop\n", TOTAL_COMBOS);

    delete[] dsList;
    return 0;
}