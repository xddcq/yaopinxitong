#include "medicine.h"
#include <QStringList>
#include <stdexcept>

void Medicine::ini(const QString& i, const QString& nam, const QString& categor,
                   const Date& prodDat, const Date& expiryDat,
                   const Date& lastStockDat, int quantit, double pric, int threshol) {
    this->id = i;
    this->name = nam;
    this->category = categor;
    this->productionDate = prodDat;
    this->expiryDate = expiryDat;
    this->lastStockDate = lastStockDat;
    this->quantity = quantit;
    this->price = pric;
    this->warningThreshold = threshol;
}
int Medicine::getExpiryDays() const {
    // 使用 Date 类的减法运算符重载来计算天数差
    return expiryDate - productionDate;
}

void Medicine::setExpiryDays(int days) {
    // 使用 Date 类的加法运算符重载来添加天数
    expiryDate = productionDate + days;
}
bool Medicine::isLowStock() const {
    return quantity < warningThreshold;
}

bool Medicine::isNearExpiry(const Date& currentDate) const {
    return currentDate.isNearExpiry(expiryDate);
}

bool Medicine::isExpired(const Date& currentDate) const {
    return currentDate.isExpired(expiryDate);
}

QString Medicine::toString() const {
    return QString("ID: %1, Name: %2, Category: %3, Quantity: %4, Price: %5")
    .arg(id).arg(name).arg(category).arg(quantity).arg(price);
}


void Medicine::setIsPrescription(bool isPrescription) {
    this->isPrescription = isPrescription;
}

Medicine Medicine::fromFileData(const QString& name, const QStringList& lines) {
    if (lines.size() < 6) {
        throw std::runtime_error("Invalid file format: insufficient data lines");
    }

    Medicine medicine;
    bool ok;

    // 解析药品ID (第2行)
    QString idStr = lines[1].trimmed();

    // 解析价格 (第3行)
    QString priceStr = lines[2].trimmed();
    double price = priceStr.toDouble(&ok);
    if (!ok) {
        throw std::runtime_error("Invalid price format: " + priceStr.toStdString());
    }

    // 解析保质期 (第4行)
    QString expiryStr = lines[3].trimmed();
    int expiryDays = expiryStr.toInt(&ok);
    if (!ok || expiryDays <= 0) {
        throw std::runtime_error("Invalid expiry days format: " + expiryStr.toStdString());
    }

    // 解析药品类型 (第5行)
    QString category = lines[4].trimmed();
    if (category.isEmpty()) {
        throw std::runtime_error("Category cannot be empty");
    }

    // 解析处方药标志 (第6行)
    QString prescriptionStr = lines[5].trimmed();
    bool isPrescription = (prescriptionStr == "1");

    // 设置药品属性
    medicine.setId(idStr);
    medicine.setName(name);
    medicine.setPrice(price);
    medicine.setExpiryDays(expiryDays);
    medicine.setCategory(category);
    medicine.setIsPrescription(isPrescription);

    // 如果有库存信息（可选）
    if (lines.size() > 6) {
        QString quantityStr = lines[6].trimmed();
        int quantity = quantityStr.toInt(&ok);
        if (ok) {
            medicine.setQuantity(quantity);
        }
    }

    // 如果有最后入库日期（可选）
    if (lines.size() > 7) {
        Date lastStockDate;
        QString dateStr = lines[7].trimmed();
        if (!dateStr.isEmpty()) {
            lastStockDate = Date::fromString(dateStr);
            medicine.setLastStockDate(lastStockDate);
        }
    }

    return medicine;
}

QString Medicine::toFileData() const {
    QStringList data;
    data << this->getName();
    data << this->getId();
    data << QString::number(this->getPrice(), 'f', 2);
    data << QString::number(this->getExpiryDays());
    data << this->getCategory();
    data << (this->getIsPrescription() ? "1" : "0");
    data << QString::number(this->getQuantity());
    data << this->getLastStockDate().toString();

    return data.join("\n");
}
void Medicine::setId(const QString& id) {
    this->id = id;
}

void Medicine::setName(const QString& name) {
    this->name = name;
}

void Medicine::setCategory(const QString& category) {
    this->category = category;
}
