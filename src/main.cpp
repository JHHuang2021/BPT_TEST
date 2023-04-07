#include <cstdio>
#include <iostream>
#include <vector>

#include "bpt.hpp"

struct String {
    char index[65];

    String(const String &other) {
        for (int i = 0; i < 65; i++) index[i] = other.index[i];
    }

    String() = default;

    friend bool operator>(const String &lhs, const String &rhs) {
        return std::string(lhs.index) > std::string(rhs.index);
    }

    friend bool operator>=(const String &lhs, const String &rhs) {
        return std::string(lhs.index) >= std::string(rhs.index);
    }

    friend bool operator<(const String &lhs, const String &rhs) {
        return std::string(lhs.index) < std::string(rhs.index);
    }

    friend bool operator<=(const String &lhs, const String &rhs) {
        return std::string(lhs.index) <= std::string(rhs.index);
    }

    friend bool operator==(const String &lhs, const String &rhs) {
        return std::string(lhs.index) == std::string(rhs.index);
    }

    friend bool operator!=(const String &lhs, const String &rhs) {
        return std::string(lhs.index) != std::string(rhs.index);
    }

    friend std::ostream &operator<<(std::ostream &os, const String &obj) {
        os << obj.index;
        return os;
    }
};

int main() {
    //  freopen("5.in", "r", stdin);
    //   freopen("me.out", "w", stdout);
    huang::BPlusTree<String, int> bpTree("test");
    std::pair<String, int> val;
    int cnt;
    char cmd[10];
    int data;
    scanf("%d", &cnt);
    for (int i = 1; i <= cnt; i++) {
        scanf("%s", cmd);
        if (cmd[0] == 'i') {
            scanf("%s%d", val.first.index, &val.second);
            bpTree.Insert(val.first, val.second);
        } else if (cmd[0] == 'f') {
            scanf("%s", val.first.index);
            std::vector<int> ans;
            bpTree.GetValue(val.first, val.first, &ans);
            if (!ans.empty()) {
                for (int i = 0; i < ans.size() - 1; i++) printf("%d ", ans[i]);
                printf("%d\n", ans[ans.size() - 1]);
            } else
                puts("null");

        } else if (cmd[0] == 'd') {
            scanf("%s%d", val.first.index, &val.second);
            bpTree.Remove(val.first);
        }
    }
    return 0;
}