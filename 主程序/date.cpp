#include "date.h"
#include <QStringList>

//Date::Date(int y, int m, int d) : year(y), month(m), day(d) {}

bool Date::isValid() const {
    if (year < 1900 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        return false;
    if (month == 2) {
        bool leap = (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);
        if (day > (leap ? 29 : 28)) return false;
    }
    return true;
}

bool Date::isExpired(const Date& expiryDate) const {
    return *this > expiryDate;
}
bool Date::isNearExpiry(const Date& expiryDate, int days) const {
    Date currentPlusDays = *this;
    currentPlusDays.day += days;
    return expiryDate < currentPlusDays && !isExpired(expiryDate);
}

QString Date::toString() const {
    return QString("%1-%2-%3").arg(year, 4, 10, QChar('0'))
                              .arg(month, 2, 10, QChar('0'))
                              .arg(day, 2, 10, QChar('0'));
}

Date Date::fromString(const QString& dateStr) {
    QStringList parts = dateStr.split('-');
    if (parts.size() != 3) return Date();
    return Date(parts[0].toInt(), parts[1].toInt(), parts[2].toInt());
}

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator==(const Date& other) const {
    return year == other.year && month == other.month && day == other.day;
}
