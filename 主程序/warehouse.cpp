#include "warehouse.h"
#include <algorithm>

bool Warehouse::addMedicine(const Medicine& medicine) {
    if (medicines.contains(medicine.getId())) return false;

    medicines.insert(medicine.getId(), medicine);
    categoryIndex[medicine.getCategory()].append(medicine.getId());
    return true;
}

bool Warehouse::removeMedicine(const QString& id) {
    if (!medicines.contains(id)) return false;

    Medicine med = medicines.value(id);
    QList<QString>& ids = categoryIndex[med.getCategory()];
    ids.removeOne(id);
    if (ids.isEmpty()) {
        categoryIndex.remove(med.getCategory());
    }

    medicines.remove(id);
    return true;
}

bool Warehouse::updateMedicine(const Medicine& medicine) {
    if (!medicines.contains(medicine.getId())) return false;

    Medicine& oldMed = medicines[medicine.getId()];
    if (oldMed.getCategory() != medicine.getCategory()) {
        // 如果类别改变，更新索引
        QList<QString>& oldIds = categoryIndex[oldMed.getCategory()];
        oldIds.removeOne(medicine.getId());
        if (oldIds.isEmpty()) {
            categoryIndex.remove(oldMed.getCategory());
        }
        categoryIndex[medicine.getCategory()].append(medicine.getId());
    }

    medicines[medicine.getId()] = medicine;
    return true;
}

Medicine* Warehouse::findMedicineById(const QString& id) {
    if (!medicines.contains(id)) return nullptr;
    return &medicines[id];
}

QList<Medicine> Warehouse::findMedicineByName(const QString& name) {
    QList<Medicine> result;
    for (const Medicine& med : medicines.values()) {
        if (med.getName().contains(name, Qt::CaseInsensitive)) {
            result.append(med);
        }
    }
    return result;
}

QList<Medicine> Warehouse::findMedicineByCategory(const QString& category) {
    QList<Medicine> result;
    if (!categoryIndex.contains(category)) return result;

    for (const QString& id : categoryIndex.value(category)) {
        result.append(medicines.value(id));
    }
    return result;
}

QList<Medicine> Warehouse::getAllMedicines() const {
    return medicines.values();
}


bool Warehouse::stockIn(const QString& id, int quantity, const Date& currentDate) {
    if (!medicines.contains(id) || quantity <= 0) return false;

    Medicine& med = medicines[id];
    med.setQuantity(med.getQuantity() + quantity);
    med.setLastStockDate(currentDate);
    return true;
}

bool Warehouse::stockOut(const QString& id, int quantity) {
    if (!medicines.contains(id) || quantity <= 0) return false;

    Medicine& med = medicines[id];
    if (med.getQuantity() < quantity) return false;

    med.setQuantity(med.getQuantity() - quantity);
    return true;
}
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <QDebug>
#include <QMessageBox>

bool Warehouse::loadFromFile() {
    QDir dir("数据保存/药品名单");
    qDebug() << "Current working directory:" << QDir::currentPath();
    qDebug() << "Looking for directory:" << dir.absolutePath();

    // 检查目录是否存在，不存在则创建
    if (!dir.exists()) {
        qDebug() << "药品名单目录不存在，尝试创建...";
        if (!dir.mkpath(".")) {
            qDebug() << "创建药品名单目录失败";
            QMessageBox::critical(nullptr, "错误", "无法创建药品数据目录");
            return false;
        }
        qDebug() << "药品名单目录创建成功";
        QMessageBox::information(nullptr, "提示", "药品数据目录不存在，已新建空目录");
        return true; // 新创建的目录是空的，返回true但不会加载任何药品
    }

    QFile listFile("数据保存/药品名单.txt");
    if (!listFile.exists()) {
        qDebug() << "药品名单.txt不存在";
        QMessageBox::information(nullptr, "提示", "药品名单文件不存在，将使用空数据库");
        return true; // 文件不存在不算错误，返回true但不会加载任何药品
    }

    if (!listFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开药品名单.txt:" << listFile.errorString();
        QMessageBox::critical(nullptr, "错误",
                              QString("无法打开药品名单文件:\n%1").arg(listFile.errorString()));
        return false;
    }

    int loadedCount = 0;
    int errorCount = 0;
    QTextStream in(&listFile);
    while (!in.atEnd()) {
        QString medicineName = in.readLine().trimmed();
        if (medicineName.isEmpty()) continue;

        QFile dataFile(QString("数据保存/药品名单/%1.txt").arg(medicineName));
        if (!dataFile.exists()) {
            qDebug() << "药品数据文件不存在:" << dataFile.fileName();
            errorCount++;
            continue;
        }

        if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开药品数据文件:" << dataFile.fileName()
                << "错误:" << dataFile.errorString();
            errorCount++;
            continue;
        }

        QTextStream dataIn(&dataFile);
        QStringList lines;
        while (!dataIn.atEnd()) {
            lines << dataIn.readLine();
        }

        try {
            Medicine med = Medicine::fromFileData(medicineName, lines);
            if (this->addMedicine(med)) {
                loadedCount++;
                qDebug() << "成功加载药品:" << medicineName;
            } else {
                qDebug() << "药品已存在，跳过:" << medicineName;
                errorCount++;
            }
        } catch (const std::exception& e) {
            qDebug() << "加载药品" << medicineName << "时出错:" << e.what();
            errorCount++;
        }
    }

    qDebug() << "药品加载完成，成功:" << loadedCount << "失败:" << errorCount;

    if (errorCount > 0) {
        QMessageBox::warning(nullptr, "警告",
                             QString("成功加载%1个药品，%2个药品加载失败").arg(loadedCount).arg(errorCount));
    } else if (loadedCount == 0) {
        QMessageBox::information(nullptr, "提示", "没有找到任何药品数据");
    } else {
        QMessageBox::information(nullptr, "成功",
                                 QString("成功加载%1个药品").arg(loadedCount));
    }

    return true; // 只要有成功加载或文件不存在等情况都返回true
}

bool Warehouse::saveToFile() const {
    QDir dir("数据保存/药品名单");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "创建药品名单目录失败";
            QMessageBox::critical(nullptr, "错误", "无法创建药品数据目录");
            return false;
        }
    }

    // 保存药品名单
    QFile listFile("数据保存/药品名单.txt");
    if (!listFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开药品名单.txt:" << listFile.errorString();
        QMessageBox::critical(nullptr, "错误",
                              QString("无法创建药品名单文件:\n%1").arg(listFile.errorString()));
        return false;
    }

    int savedCount = 0;
    int errorCount = 0;
    QTextStream out(&listFile);
    for (const Medicine& med : medicines.values()) {
        out << med.getName() << "\n";

        // 保存单个药品数据
        QFile dataFile(QString("数据保存/药品名单/%1.txt").arg(med.getName()));
        if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "无法保存药品数据文件:" << dataFile.fileName()
                << "错误:" << dataFile.errorString();
            errorCount++;
            continue;
        }

        QTextStream dataOut(&dataFile);
        dataOut << med.toFileData();
        savedCount++;
        qDebug() << "成功保存药品:" << med.getName();
    }

    qDebug() << "药品保存完成，成功:" << savedCount << "失败:" << errorCount;

    if (errorCount > 0) {
        QMessageBox::warning(nullptr, "警告",
                             QString("成功保存%1个药品，%2个药品保存失败").arg(savedCount).arg(errorCount));
    } else if (savedCount == 0) {
        QMessageBox::information(nullptr, "提示", "没有药品数据需要保存");
    } else {
        QMessageBox::information(nullptr, "成功",
                                 QString("成功保存%1个药品").arg(savedCount));
    }

    return errorCount == 0; // 如果有任何保存失败则返回false
}
