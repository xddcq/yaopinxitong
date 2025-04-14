#ifndef BASE_H
#define BASE_H
#include<windows.h>
static int timeyear() {
    SYSTEMTIME st;
    GetLocalTime(&st); // 获取本地时间年
    return st.wYear;
}
static int timemonth() {
    SYSTEMTIME st;
    GetLocalTime(&st); // 获取本地时间月
    return st.wMonth;
}
static int timeday() {
    SYSTEMTIME st;
    GetLocalTime(&st); // 获取本地时间日
    return st.wDay;
}
#endif // BASE_H
