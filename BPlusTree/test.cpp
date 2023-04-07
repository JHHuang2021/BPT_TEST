#include "include/b_plus_tree.hpp"
struct node {
    char key[64];
    int value;
    node() {}
    node(const char* ch, int value) {
        strcpy(key, ch);
        this->value = value;
    }
    bool operator==(const node& rhs) const {
        if (strcmp(key, rhs.key) != 0 || value != rhs.value)
            return false;
        else
            return true;
    }
    bool operator<(const node& rhs) const {
        if (strcmp(key, rhs.key) == 0)
            return value < rhs.value;
        else
            return strcmp(key, rhs.key) < 0;
    }
    bool operator<=(const node& rhs) const {
        if (strcmp(key, rhs.key) == 0)
            return value <= rhs.value;
        else
            return strcmp(key, rhs.key) <= 0;
    }
};
#define INT_MAX (1 << 31) - 1
#define INT_MIN 1 << 31
huang::DiskManager dm("test_disk_manager");
huang::Replacer<huang::Page*> rep;
huang::BufferPoolManager bpm(512, &dm, &rep);
huang::BPlusTree<node, int> bpt(&bpm);
int main() {
    // freopen("test0.in", "r", stdin);
    // freopen("test.out", "w", stdout);
    int n;
    char ch[64];
    std::cin >> n;
    for (int i = 1; i <= n; i++) {
        // std::cout << "Case :" << i << std::endl;
        // bpt.Debug();
        // std::cout << std::endl;
        std::string op;
        node n;
        std::cin >> op;
        if (op == "insert") {
            std::cin >> n.key >> n.value;
            bpt.Insert(n, n.value);
        } else if (op == "delete") {
            std::cin >> n.key >> n.value;
            bpt.Remove(n);
        } else if (op == "find") {
            std::vector<int> ans;
            std::cin >> ch;
            bpt.GetValue({ch, INT_MIN}, {ch, INT_MAX}, &ans);
            if (ans.empty())
                std::cout << "null" << std::endl;
            else {
                for (auto i : ans) std::cout << i << " ";
                std::cout << std::endl;
            }
        }
    }
    return 0;
}