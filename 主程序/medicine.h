#ifndef MEDICINE_H
#define MEDICINE_H

#include "date.h"
#include <QString>

class Medicine {
private:
    QString id;
    QString name;
    QString category;
    Date productionDate;
    Date expiryDate;
    Date lastStockDate;
    int quantity;
    double price;
    int warningThreshold;
    bool isPrescription;
public:
    void ini(const QString& i, const QString& nam, const QString& categor,
             const Date& prodDat, const Date& expiryDat,
             const Date& lastStockDat, int quantity, double pric, int threshol = 10);
    void setId(const QString& id);
    void setName(const QString& name);
    void setExpiryDays(int days);
    void setCategory(const QString& category);
    void setIsPrescription(bool isPrescription);

    // Getters
    QString getId() const { return id; }
    QString getName() const { return name; }
    QString getCategory() const { return category; }
    Date getProductionDate() const { return productionDate; }
    Date getExpiryDate() const { return expiryDate; }
    Date getLastStockDate() const { return lastStockDate; }
    int getQuantity() const { return quantity; }
    double getPrice() const { return price; }
    int getWarningThreshold() const { return warningThreshold; }
    int getExpiryDays() const;  // 新增方法
    bool getIsPrescription() const { return isPrescription; }

    // Setters
    void setQuantity(int q) { quantity = q; }
    void setPrice(double p) { price = p; }
    void setLastStockDate(const Date& d) { lastStockDate = d; }

    // Functions
    bool isLowStock() const;
    bool isNearExpiry(const Date& currentDate) const;
    bool isExpired(const Date& currentDate) const;
    QString toString() const;

    // 新增方法
    static Medicine fromFileData(const QString& name, const QStringList& lines);
    QString toFileData() const;
};
#endif // MEDICINE_H
