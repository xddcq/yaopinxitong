#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "medicine.h"
#include <QMap>
#include <QList>
#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
class Warehouse {
private:
    QMap<QString, Medicine> medicines; // 以ID为键存储药品
    QMap<QString, QList<QString>> categoryIndex; // 类别索引
    QString getDataDir() const {
        QDir dir;
        QString path = dir.absoluteFilePath("数据保存/药品名单");
        if (!dir.exists(path)) {
            dir.mkpath(path);
        }
        return path;
    }

    QString getListFilePath() const {
        return QDir(getDataDir()).absoluteFilePath("../药品名单.txt");
    }

    QString getMedicineFilePath(const QString& name) const {
        return QDir(getDataDir()).absoluteFilePath(name + ".txt");
    }
public:
    QList<Medicine> checkLowStock() const {
        QList<Medicine> result;
        for (const Medicine& med : medicines.values()) {
            if (med.isLowStock()) {
                result.append(med);
            }
        }
        return result;
    }

    QList<Medicine> checkNearExpiry(const Date& currentDate) const {
        QList<Medicine> result;
        for (const Medicine& med : medicines.values()) {
            if (med.isNearExpiry(currentDate)) {
                result.append(med);
            }
        }
        return result;
    }
    bool loadFromFile();
    bool saveToFile() const;
    bool addMedicine(const Medicine& medicine);
    bool removeMedicine(const QString& id);
    bool updateMedicine(const Medicine& medicine);
    Medicine* findMedicineById(const QString& id);
    QList<Medicine> findMedicineByName(const QString& name);
    QList<Medicine> findMedicineByCategory(const QString& category);
    QList<Medicine> getAllMedicines() const;
    bool stockIn(const QString& id, int quantity, const Date& currentDate);
    bool stockOut(const QString& id, int quantity);
};

#endif // WAREHOUSE_H

