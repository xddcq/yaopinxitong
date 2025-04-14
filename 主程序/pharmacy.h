#ifndef PHARMACY_H
#define PHARMACY_H

#include "warehouse.h"
#include "date.h"

#include <QList>

class Pharmacy {
private:
    Warehouse& warehouse;
    QMap<Date, QMap<QString, int>> salesRecords; // 日期 -> (药品ID -> 销售数量)
    QMap<QString, double> categorySales; // 类别 -> 总销售额

public:
    QList<Medicine> getLowStockMedicines(const Date& /*currentDate*/) {
        return warehouse.checkLowStock();
    }

    QList<Medicine> getNearExpiryMedicines(const Date& currentDate) {
        return warehouse.checkNearExpiry(currentDate);
    }
    Pharmacy();
    Pharmacy(Warehouse& warehouse) : warehouse(warehouse) {}
    // 药品管理
    bool addMedicine(const Medicine& medicine);
    bool removeMedicine(const QString& id);
    bool updateMedicine(const Medicine& medicine);
    QList<Medicine> searchMedicine(const QString& keyword);
    //QList<Medicine> getLowStockMedicines(const Date& currentDate);
    //QList<Medicine> getNearExpiryMedicines(const Date& currentDate);

    // 库存管理
    bool stockIn(const QString& id, int quantity, const Date& currentDate);
    bool stockOut(const QString& id, int quantity, const Date& currentDate);

    // 销售管理
    bool sellMedicine(const QString& id, int quantity, const Date& saleDate);
    double getDailySales(const Date& date) const;
    double getMonthlySales(int year, int month) const;
    double getYearlySales(int year) const;
    QMap<QString, double> getCategorySales() const;
};

#endif // PHARMACY_H
