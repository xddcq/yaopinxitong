#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include<bits/stdc++.h>
namespace fs = std::filesystem;
struct Medicine {
    std::string id;
    std::string name;
    std::string category;
    std::string productionDate;
    std::string expiryDate;
    std::string lastStockDate;
    int quantity = 0;
    double price = 0.0;
    int warningThreshold = 10;
    bool isPrescription = false;
    void loadFromLines(const std::vector<std::string>& lines) {
        if (lines.size() >= 6) {
            id = lines[1];
            price = std::stod(lines[2]);
            expiryDate = lines[3];
            category = lines[4];
            isPrescription = (lines[5] == "1");

            if (lines.size() > 6) quantity = std::stoi(lines[6]);
            if (lines.size() > 7) lastStockDate = lines[7];
        }
    }
    std::vector<std::string> toFileLines() const {
        return {
            name,
            id,
            std::to_string(price),
            expiryDate,
            category,
            isPrescription ? "1" : "0",
            std::to_string(quantity),
            lastStockDate
        };
    }

    void display() const {
        std::cout << "\n=== 药品信息 ===\n";
        std::cout << "名称: " << name << "\n";
        std::cout << "ID: " << id << "\n";
        std::cout << "类别: " << category << "\n";
        std::cout << "生产日期: " << productionDate << "\n";
        std::cout << "过期日期: " << expiryDate << "\n";
        std::cout << "最后入库: " << lastStockDate << "\n";
        std::cout << "库存量: " << quantity << "\n";
        std::cout << "价格: " << std::fixed << std::setprecision(2) << price << "\n";
        std::cout << "警告阈值: " << warningThreshold << "\n";
        std::cout << "处方药: " << (isPrescription ? "是" : "否") << "\n";
    }
};

// 读取药品文件
Medicine readMedicineFile(const std::string& filepath) {
    Medicine med;
    std::ifstream file(filepath);
    std::vector<std::string> lines;
    std::string line;

    if (file.is_open()) {
        // 从文件名获取药品名称
        med.name = fs::path(filepath).stem().string();

        while (std::getline(file, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
        med.loadFromLines(lines);
    }
    else {
        std::cerr << "无法打开文件: " << filepath << std::endl;
    }

    return med;
}

// 保存药品文件
void saveMedicineFile(const Medicine& med, const std::string& dir = "数据保存/药品名单") {
    // 创建目录（如果不存在）
    fs::create_directories(dir);

    std::string filename = dir + "/" + med.name + ".txt";
    std::ofstream file(filename);

    if (file.is_open()) {
        auto lines = med.toFileLines();
        for (const auto& line : lines) {
            file << line << "\n";
        }
        std::cout << "药品数据已保存到: " << filename << std::endl;
    }
    else {
        std::cerr << "无法保存文件: " << filename << std::endl;
    }
}

// 编辑药品信息
void editMedicine(Medicine& med) {
    med.display();

    std::cout << "\n=== 编辑选项 ===\n";
    std::cout << "1. 修改库存量\n";
    std::cout << "2. 修改价格\n";
    std::cout << "3. 修改过期日期\n";
    std::cout << "4. 修改最后入库日期\n";
    std::cout << "5. 修改所有信息\n";
    std::cout << "0. 取消\n";
    std::cout << "请选择: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(); // 清除输入缓冲区

    switch (choice) {
    case 1: {
        std::cout << "当前库存量: " << med.quantity << "\n";
        std::cout << "输入新库存量: ";
        std::cin >> med.quantity;
        break;
    }
    case 2: {
        std::cout << "当前价格: " << med.price << "\n";
        std::cout << "输入新价格: ";
        std::cin >> med.price;
        break;
    }
    case 3: {
        std::cout << "当前过期日期: " << med.expiryDate << "\n";
        std::cout << "输入新过期日期(YYYY-MM-DD): ";
        std::getline(std::cin, med.expiryDate);
        break;
    }
    case 4: {
        std::cout << "当前最后入库日期: " << med.lastStockDate << "\n";
        std::cout << "输入新最后入库日期(YYYY-MM-DD): ";
        std::getline(std::cin, med.lastStockDate);
        break;
    }
    case 5: {
        std::cout << "输入药品ID: ";
        std::getline(std::cin, med.id);
        std::cout << "输入类别: ";
        std::getline(std::cin, med.category);
        std::cout << "输入生产日期(YYYY-MM-DD): ";
        std::getline(std::cin, med.productionDate);
        std::cout << "输入过期日期(YYYY-MM-DD): ";
        std::getline(std::cin, med.expiryDate);
        std::cout << "输入最后入库日期(YYYY-MM-DD): ";
        std::getline(std::cin, med.lastStockDate);
        std::cout << "输入库存量: ";
        std::cin >> med.quantity;
        std::cout << "输入价格: ";
        std::cin >> med.price;
        std::cout << "输入警告阈值: ";
        std::cin >> med.warningThreshold;
        std::cout << "是处方药吗? (1=是/0=否): ";
        std::cin >> med.isPrescription;
        break;
    }
    case 0:
        std::cout << "取消编辑\n";
        return;
    default:
        std::cout << "无效选择\n";
        return;
    }

    std::cout << "修改成功!\n";
    med.display();
}

// 主菜单
void mainMenu() {
    while (true) {
        std::cout << "\n=== 药品数据编辑器 ===\n";
        std::cout << "1. 编辑现有药品\n";
        std::cout << "2. 创建新药品\n";
        std::cout << "3. 列出所有药品\n";
        std::cout << "0. 退出\n";
        std::cout << "请选择: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(); // 清除输入缓冲区

        switch (choice) {
        case 1: {
            std::cout << "输入药品名称: ";
            std::string name;
            std::getline(std::cin, name);

            std::string filepath = "数据保存/药品名单/" + name + ".txt";
            if (fs::exists(filepath)) {
                Medicine med = readMedicineFile(filepath);
                editMedicine(med);
                saveMedicineFile(med);
            }
            else {
                std::cout << "药品不存在!\n";
            }
            break;
        }
        case 2: {
            Medicine newMed;
            std::cout << "输入药品名称: ";
            std::getline(std::cin, newMed.name);

            editMedicine(newMed);
            saveMedicineFile(newMed);
            break;
        }
        case 3: {
            std::cout << "\n=== 药品列表 ===\n";
            std::string dir = "数据保存/药品名单";
            if (fs::exists(dir)) {
                for (const auto& entry : fs::directory_iterator(dir)) {
                    if (entry.path().extension() == ".txt") {
                        std::cout << "- " << entry.path().stem() << "\n";
                    }
                }
            }
            else {
                std::cout << "药品目录不存在!\n";
            }
            break;
        }
        case 0:
            std::cout << "退出程序\n";
            return;
        default:
            std::cout << "无效选择\n";
        }
    }
}

int main() {
    std::cout << "药品数据编辑器\n";
    mainMenu();
    return 0;
}