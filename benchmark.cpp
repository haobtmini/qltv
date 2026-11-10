#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 1000000        // 1.000.000 phần tử
#define NUM_DEL 100000   // 100.000 Delete

double get_time_ms() {
    return ((double)clock() / CLOCKS_PER_SEC) * 1000.0;
}

// ==========================================================
// 1. BST THƯỜNG (KHÔNG ĐỆ QUY)
// ==========================================================
struct NodeBST {
    int key;
    NodeBST* left;
    NodeBST* right;
};

NodeBST* insertBST(NodeBST* root, int key) {
    NodeBST* newNode = (NodeBST*)malloc(sizeof(NodeBST));
    newNode->key = key;
    newNode->left = nullptr;
    newNode->right = nullptr;

    if (!root) return newNode;

    NodeBST* curr = root;
    NodeBST* parent = nullptr;
    while (curr) {
        parent = curr;
        if (key < curr->key) curr = curr->left;
        else if (key > curr->key) curr = curr->right;
        else { free(newNode); return root; }
    }
    if (key < parent->key) parent->left = newNode;
    else parent->right = newNode;
    return root;
}

NodeBST* deleteBST(NodeBST* root, int key) {
    NodeBST* curr = root;
    NodeBST* prev = nullptr;

    while (curr && curr->key != key) {
        prev = curr;
        if (key < curr->key) curr = curr->left;
        else curr = curr->right;
    }
    if (!curr) return root;

    if (!curr->left || !curr->right) {
        NodeBST* newCurr = (!curr->left) ? curr->right : curr->left;
        if (!prev) {
            free(curr);
            return newCurr;
        }
        if (curr == prev->left) prev->left = newCurr;
        else prev->right = newCurr;
        free(curr);
    } else {
        NodeBST* p = nullptr;
        NodeBST* temp = curr->right;
        while (temp->left) {
            p = temp;
            temp = temp->left;
        }
        if (p) p->left = temp->right;
        else curr->right = temp->right;
        curr->key = temp->key;
        free(temp);
    }
    return root;
}

void freeBST(NodeBST* root) {
    while (root) {
        if (!root->left) {
            NodeBST* temp = root->right;
            free(root);
            root = temp;
        } else {
            NodeBST* pre = root->left;
            while (pre->right && pre->right != root) pre = pre->right;
            if (!pre->right) {
                pre->right = root->right;
                NodeBST* temp = root->left;
                free(root);
                root = temp;
            }
        }
    }
}

int computeHeightBST(NodeBST* root) {
    if (!root) return 0;
    NodeBST** queue = (NodeBST**)malloc(N * sizeof(NodeBST*));
    int head = 0, tail = 0;
    queue[tail++] = root;
    int height = 0;

    while (head < tail) {
        int count = tail - head;
        height++;
        for (int i = 0; i < count; i++) {
            NodeBST* curr = queue[head++];
            if (curr->left) queue[tail++] = curr->left;
            if (curr->right) queue[tail++] = curr->right;
        }
    }
    free(queue);
    return height;
}

// ==========================================================
// 2. CÂY AVL (LOGIC XOAY VÒNG LẶP CHUẨN XÁC)
// ==========================================================
struct NodeAVL {
    int key;
    int height;
    NodeAVL* left;
    NodeAVL* right;
};

int getHeight(NodeAVL* n) {
    return n ? n->height : 0;
}

int maxVal(int a, int b) {
    return (a > b) ? a : b;
}

int getBalance(NodeAVL* n) {
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}

void updateHeight(NodeAVL* n) {
    if (n) n->height = 1 + maxVal(getHeight(n->left), getHeight(n->right));
}

NodeAVL* rightRotate(NodeAVL* y) {
    NodeAVL* x = y->left;
    NodeAVL* T2 = x->right;
    x->right = y;
    y->left = T2;
    updateHeight(y);
    updateHeight(x);
    return x;
}

NodeAVL* leftRotate(NodeAVL* x) {
    NodeAVL* y = x->right;
    NodeAVL* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

NodeAVL* rebalance(NodeAVL* node) {
    updateHeight(node);
    int balance = getBalance(node);

    if (balance > 1) {
        if (getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
        }
        return rightRotate(node);
    }
    if (balance < -1) {
        if (getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
        }
        return leftRotate(node);
    }
    return node;
}

// Chèn AVL: lưu trực tiếp con trỏ cấp 2 để nối cha con an toàn 100%
void insertAVLItr(NodeAVL** rootPtr, int key) {
    NodeAVL** path[64];
    int top = 0;
    NodeAVL** curr = rootPtr;

    while (*curr) {
        path[top++] = curr;
        if (key < (*curr)->key) curr = &((*curr)->left);
        else if (key > (*curr)->key) curr = &((*curr)->right);
        else return; // Trùng key
    }

    NodeAVL* newNode = (NodeAVL*)malloc(sizeof(NodeAVL));
    newNode->key = key;
    newNode->height = 1;
    newNode->left = nullptr;
    newNode->right = nullptr;
    *curr = newNode;

    // Lội ngược lên cập nhật và cân bằng
    while (top > 0) {
        NodeAVL** pNode = path[--top];
        *pNode = rebalance(*pNode);
    }
}

// Xóa AVL bằng con trỏ cấp 2
void deleteAVLItr(NodeAVL** rootPtr, int key) {
    NodeAVL** path[64];
    int top = 0;
    NodeAVL** curr = rootPtr;

    while (*curr && (*curr)->key != key) {
        path[top++] = curr;
        if (key < (*curr)->key) curr = &((*curr)->left);
        else curr = &((*curr)->right);
    }
    if (!*curr) return; // Không tìm thấy

    NodeAVL* target = *curr;

    if (!target->left || !target->right) {
        *curr = target->left ? target->left : target->right;
        free(target);
    } else {
        path[top++] = curr;
        NodeAVL** succ = &(target->right);
        while ((*succ)->left) {
            path[top++] = succ;
            succ = &((*succ)->left);
        }
        target->key = (*succ)->key;
        NodeAVL* temp = *succ;
        *succ = (*succ)->right;
        free(temp);
    }

    // Tái cân bằng đường đi
    while (top > 0) {
        NodeAVL** pNode = path[--top];
        *pNode = rebalance(*pNode);
    }
}

void freeAVL(NodeAVL* root) {
    while (root) {
        if (!root->left) {
            NodeAVL* temp = root->right;
            free(root);
            root = temp;
        } else {
            NodeAVL* pre = root->left;
            while (pre->right && pre->right != root) pre = pre->right;
            if (!pre->right) {
                pre->right = root->right;
                NodeAVL* temp = root->left;
                free(root);
                root = temp;
            }
        }
    }
}

// ==========================================================
// 3. SINH MÃ THỨ TỰ NHỊ PHÂN
// ==========================================================
struct Segment {
    int left;
    int right;
};

void generateBinaryOrderKeys(int* dest, int n, int startKey) {
    Segment* queue = (Segment*)malloc(n * sizeof(Segment));
    int head = 0, tail = 0;
    queue[tail++] = { 0, n - 1 };
    int idx = 0;

    while (head < tail) {
        Segment seg = queue[head++];
        if (seg.left > seg.right) continue;

        int mid = seg.left + (seg.right - seg.left) / 2;
        dest[idx++] = startKey + mid;

        if (seg.left <= mid - 1) queue[tail++] = { seg.left, mid - 1 };
        if (mid + 1 <= seg.right) queue[tail++] = { mid + 1, seg.right };
    }
    free(queue);
}

// ==========================================================
// MAIN BENCHMARK
// ==========================================================
int main() {
    printf("=========================================================================================================\n");
    printf("   BENCHMARK 4 PHUONG AN: [1] TANG DAN+AVL | [2] RANDOM+BST | [3] NHI PHAN+AVL | [4] RANDOM+AVL\n");
    printf("   Quy mo: %d Insert | %d Delete (100%% Vong lap)\n", N, NUM_DEL);
    printf("=========================================================================================================\n\n");
    fflush(stdout);

    int* sortedKeys = (int*)malloc(N * sizeof(int));
    int* randomKeys = (int*)malloc(N * sizeof(int));
    int* binaryKeys = (int*)malloc(N * sizeof(int));

    for (int i = 0; i < N; i++) {
        sortedKeys[i] = 1000000 + i;
        randomKeys[i] = 1000000 + i;
    }

    srand(12345);
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = randomKeys[i];
        randomKeys[i] = randomKeys[j];
        randomKeys[j] = tmp;
    }

    generateBinaryOrderKeys(binaryKeys, N, 1000000);

    int* deleteKeys = (int*)malloc(NUM_DEL * sizeof(int));
    for (int i = 0; i < NUM_DEL; i++) {
        deleteKeys[i] = randomKeys[i * 7 % N];
    }
    printf("[+] Chuan bi 3 tap key xong!\n\n");
    fflush(stdout);

    // [1] TĂNG DẦN + AVL
    printf("[1] DANG TEST: MA TANG DAN + CAY AVL...\n");
    fflush(stdout);
    NodeAVL* root1 = nullptr;
    double t0 = get_time_ms();
    for (int i = 0; i < N; i++) insertAVLItr(&root1, sortedKeys[i]);
    double tInsert1 = get_time_ms() - t0;
    int h1 = getHeight(root1);

    t0 = get_time_ms();
    for (int i = 0; i < NUM_DEL; i++) deleteAVLItr(&root1, deleteKeys[i]);
    double tDelete1 = get_time_ms() - t0;
    freeAVL(root1);
    printf("-> Kich ban 1 xong! (Chieu cao: %d)\n\n", h1);
    fflush(stdout);

    // [2] NGẪU NHIÊN + BST THƯỜNG
    printf("[2] DANG TEST: MA NGAU NHIEN + BST THUONG...\n");
    fflush(stdout);
    NodeBST* root2 = nullptr;
    t0 = get_time_ms();
    for (int i = 0; i < N; i++) root2 = insertBST(root2, randomKeys[i]);
    double tInsert2 = get_time_ms() - t0;
    int h2 = computeHeightBST(root2);

    t0 = get_time_ms();
    for (int i = 0; i < NUM_DEL; i++) root2 = deleteBST(root2, deleteKeys[i]);
    double tDelete2 = get_time_ms() - t0;
    freeBST(root2);
    printf("-> Kich ban 2 xong! (Chieu cao: %d)\n\n", h2);
    fflush(stdout);

    // [3] THỨ TỰ NHỊ PHÂN + AVL
    printf("[3] DANG TEST: MA THU TU NHI PHAN + CAY AVL...\n");
    fflush(stdout);
    NodeAVL* root3 = nullptr;
    t0 = get_time_ms();
    for (int i = 0; i < N; i++) insertAVLItr(&root3, binaryKeys[i]);
    double tInsert3 = get_time_ms() - t0;
    int h3 = getHeight(root3);

    t0 = get_time_ms();
    for (int i = 0; i < NUM_DEL; i++) deleteAVLItr(&root3, deleteKeys[i]);
    double tDelete3 = get_time_ms() - t0;
    freeAVL(root3);
    printf("-> Kich ban 3 xong! (Chieu cao: %d)\n\n", h3);
    fflush(stdout);

    // [4] NGẪU NHIÊN + AVL
    printf("[4] DANG TEST: MA NGAU NHIEN + CAY AVL...\n");
    fflush(stdout);
    NodeAVL* root4 = nullptr;
    t0 = get_time_ms();
    for (int i = 0; i < N; i++) insertAVLItr(&root4, randomKeys[i]);
    double tInsert4 = get_time_ms() - t0;
    int h4 = getHeight(root4);

    t0 = get_time_ms();
    for (int i = 0; i < NUM_DEL; i++) deleteAVLItr(&root4, deleteKeys[i]);
    double tDelete4 = get_time_ms() - t0;
    freeAVL(root4);
    printf("-> Kich ban 4 xong! (Chieu cao: %d)\n\n", h4);
    fflush(stdout);

    // BẢNG TỔNG KẾT
    printf("=========================================================================================================\n");
    printf("%-26s %-18s %-18s %-18s %-18s\n", "Tieu Chi So Sanh", "[1] TangDan+AVL", "[2] Random+BST", "[3] Binary+AVL", "[4] Random+AVL");
    printf("---------------------------------------------------------------------------------------------------------\n");
    printf("%-26s %-18d %-18d %-18d %-18d\n", "Chieu cao cay (Height)", h1, h2, h3, h4);
    printf("%-26s %-15.2f ms %-15.2f ms %-15.2f ms %-15.2f ms\n", "INSERT 1.000.000 Node", tInsert1, tInsert2, tInsert3, tInsert4);
    printf("%-26s %-15.2f ms %-15.2f ms %-15.2f ms %-15.2f ms\n", "DELETE 100.000 Node", tDelete1, tDelete2, tDelete3, tDelete4);
    printf("=========================================================================================================\n");

    free(sortedKeys);
    free(randomKeys);
    free(binaryKeys);
    free(deleteKeys);

    printf("\nNhan Enter de thoat...");
    fflush(stdout);
    getchar();
    return 0;
}