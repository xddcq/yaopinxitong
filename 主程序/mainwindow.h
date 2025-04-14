#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pharmacy.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
Medicine med;
private slots:
    void onAddMedicine();
    void onSearchMedicine();
    void onStockIn();
    void onStockOut();
    void onSellMedicine();
    void onShowLowStock();
    void onShowNearExpiry();
    void onShowSalesReport();
    void onShowCategoryReport();

private:
    Ui::MainWindow *ui;
    Pharmacy pharmacy;
    Date currentDate;

    void updateMedicineTable(const QList<Medicine>& medicines);
    void showWarningDialog(const QString& message);
    void showInfoDialog(const QString& message);
    Date getDateFromInput(const QString& dateStr);
};

#endif // MAINWINDOW_H





