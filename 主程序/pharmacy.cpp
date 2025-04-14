#include "pharmacy.h"
#include "warehouse.h"
Warehouse warehouse;
//Pharmacy::Pharmacy() {}

bool Pharmacy::addMedicine(const Medicine& medicine) {
    return warehouse.addMedicine(medicine);
}

bool Pharmacy::removeMedicine(const QString& id) {
    return warehouse.removeMedicine(id);
}

bool Pharmacy::updateMedicine(const Medicine& medicine) {
    return warehouse.updateMedicine(medicine);
}

QList<Medicine> Pharmacy::searchMedicine(const QString& keyword) {
    QList<Medicine> result = warehouse.findMedicineByName(keyword);
    Medicine* byId = warehouse.findMedicineById(keyword);
    if (byId) {
        result.append(*byId);
    }
    return result;
}

bool Pharmacy::stockOut(const QString& id, int quantity, const Date& /*currentDate*/) {
    return warehouse.stockOut(id, quantity);
}
bool Pharmacy::stockIn(const QString& id, int quantity, const Date& currentDate) {
    return warehouse.stockIn(id, quantity, currentDate);
}

bool Pharmacy::sellMedicine(const QString& id, int quantity, const Date& saleDate) {
    Medicine* med = warehouse.findMedicineById(id);
    if (!med || quantity <= 0 || med->getQuantity() < quantity) return false;

    if (!warehouse.stockOut(id, quantity)) return false;

    // 记录销售
    salesRecords[saleDate][id] += quantity;
    categorySales[med->getCategory()] += quantity * med->getPrice();

    return true;
}

double Pharmacy::getDailySales(const Date& date) const {
    if (!salesRecords.contains(date)) return 0.0;

    double total = 0.0;
    const QMap<QString, int>& dailySales = salesRecords.value(date);
    for (auto it = dailySales.begin(); it != dailySales.end(); ++it) {
        const Medicine* med = warehouse.findMedicineById(it.key());
        if (med) {
            total += it.value() * med->getPrice();
        }
    }
    return total;
}

double Pharmacy::getMonthlySales(int year, int month) const {
    double total = 0.0;
    for (auto it = salesRecords.begin(); it != salesRecords.end(); ++it) {
        const Date& date = it.key();
        if (date.getYear() == year && date.getMonth() == month) {
            const QMap<QString, int>& dailySales = it.value();
            for (auto sit = dailySales.begin(); sit != dailySales.end(); ++sit) {
                const Medicine* med = warehouse.findMedicineById(sit.key());
                if (med) {
                    total += sit.value() * med->getPrice();
                }
            }
        }
    }
    return total;
}

double Pharmacy::getYearlySales(int year) const {
    double total = 0.0;
    for (auto it = salesRecords.begin(); it != salesRecords.end(); ++it) {
        const Date& date = it.key();
        if (date.getYear() == year) {
            const QMap<QString, int>& dailySales = it.value();
            for (auto sit = dailySales.begin(); sit != dailySales.end(); ++sit) {
                const Medicine* med = warehouse.findMedicineById(sit.key());
                if (med) {
                    total += sit.value() * med->getPrice();
                }
            }
        }
    }
    return total;
}

QMap<QString, double> Pharmacy::getCategorySales() const {
    return categorySales;
}
// pharmacy.cpp
