#ifndef DATE_H
#define DATE_H
#include<string>
#include <QString>
#include"base.h"
class Date {
private:
    int year;
    int month;
    int day;
public:
    int daysTo(const Date& other) const {
        return other - *this;
    }

    Date addDays(int days) const {
        return *this + days;
    }
    bool isValid() const;
    bool isExpired(const Date& expiryDate) const;
    bool isNearExpiry(const Date& expiryDate, int days = 30) const;
    QString toString() const;
    static Date fromString(const QString& dateStr);
    bool operator<(const Date& other) const;

    bool operator==(const Date& other) const;
    Date(int y = timeyear(), int m = timemonth(), int d = timeday())//构造函数
    {
        if (!isValidDate(y, m, d)) {
            throw std::invalid_argument("Invalid date");
        }
        year = y;
        month = m;
        day = d;
    }
    int getYear() const { return year; }//获取年
    int getMonth() const { return month; }//获取月
    int getDay() const { return day; }//获取日
    bool operator!=(const Date& other) const {//重载!=
        return !(*this == other);
    }
    bool operator<=(const Date& other) const {//重载<=
        return *this < other || *this == other;
    }

    bool operator>(const Date& other) const {//重载>
        return !(*this <= other);
    }

    bool operator>=(const Date& other) const {//重载>=
        return !(*this < other);
    }
    friend std::ostream& operator<<(std::ostream& os, const Date& date);//重载输出
    bool isLeapYear(int y) const{
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    int daysInMonth(int y, int m) const{
        if (m == 2) {
            return isLeapYear(y) ? 29 : 28;
        }
        if (m == 4 || m == 6 || m == 9 || m == 11) {
            return 30;
        }
        return 31;
    }

    bool isValidDate(int y, int m, int d) {
        if (y < 1 || m < 1 || m > 12 || d < 1) {
            return false;
        }
        return d <= daysInMonth(y, m);
    }

    void normalize() {
        while (day > daysInMonth(year, month)) {
            day -= daysInMonth(year, month);
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }

        while (day < 1) {
            month--;
            if (month < 1) {
                month = 12;
                year--;
            }
            day += daysInMonth(year, month);
        }
    }

    void setYear(int y) {
        if (!isValidDate(y, month, day)) {
            throw std::invalid_argument("Invalid year for current month/day");
        }
        year = y;
    }

    void setMonth(int m) {
        if (m < 1 || m > 12) {
            throw std::invalid_argument("Month must be between 1 and 12");
        }
        if (!isValidDate(year, m, day)) {
            throw std::invalid_argument("Invalid month for current day/year");
        }
        month = m;
    }

    void setDay(int d) {
        if (!isValidDate(year, month, d)) {
            throw std::invalid_argument("Invalid day for current month/year");
        }
        day = d;
    }

    Date operator+(int days) const {
        Date result(*this);
        result.day += days;
        result.normalize();
        return result;
    }

    Date operator-(int days) const {
        Date result(*this);
        result.day -= days;
        result.normalize();
        return result;
    }

    Date& operator++() {
        ++day;
        normalize();
        return *this;
    }

    Date operator++(int) {
        Date temp(*this);
        ++(*this);
        return temp;
    }

    Date& operator--() {
        --day;
        normalize();
        return *this;
    }

    Date operator--(int) {
        Date temp(*this);
        --(*this);
        return temp;
    }

    int operator-(const Date& other) const {
        return daysSinceEpoch() - other.daysSinceEpoch();
    }

    int daysSinceEpoch() const {
        int totalDays = 0;

        for (int y = 1; y < year; ++y) {
            totalDays += isLeapYear(y) ? 366 : 365;
        }

        for (int m = 1; m < month; ++m) {
            totalDays += daysInMonth(year, m);
        }

        totalDays += day;

        return totalDays;
    }
};


#endif // DATE_H
