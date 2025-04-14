#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDate>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),pharmacy(*new Warehouse())  {
    ui->setupUi(this);

    // 设置当前日期为系统日期
    QDate today = QDate::currentDate();
    currentDate = Date(today.year(), today.month(), today.day());
    ui->currentDateLabel->setText("当前日期: " + currentDate.toString());

    // 连接信号和槽
    connect(ui->addMedicineBtn, &QPushButton::clicked, this, &MainWindow::onAddMedicine);
    connect(ui->searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchMedicine);
    connect(ui->stockInBtn, &QPushButton::clicked, this, &MainWindow::onStockIn);
    connect(ui->stockOutBtn, &QPushButton::clicked, this, &MainWindow::onStockOut);
    connect(ui->sellBtn, &QPushButton::clicked, this, &MainWindow::onSellMedicine);
    connect(ui->lowStockBtn, &QPushButton::clicked, this, &MainWindow::onShowLowStock);
    connect(ui->nearExpiryBtn, &QPushButton::clicked, this, &MainWindow::onShowNearExpiry);
    connect(ui->salesReportBtn, &QPushButton::clicked, this, &MainWindow::onShowSalesReport);
    connect(ui->categoryReportBtn, &QPushButton::clicked, this, &MainWindow::onShowCategoryReport);

    // 初始化药品表格
    QStringList headers = {"ID", "名称", "类别", "生产日期", "过期日期", "最后入库", "数量", "价格"};
    ui->medicineTable->setColumnCount(headers.size());
    ui->medicineTable->setHorizontalHeaderLabels(headers);
    ui->medicineTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onAddMedicine() {
    QString id = ui->idEdit->text();
    QString name = ui->nameEdit->text();
    QString category = ui->categoryEdit->text();
    Date prodDate = getDateFromInput(ui->prodDateEdit->text());
    Date expiryDate = getDateFromInput(ui->expiryDateEdit->text());
    Date lastStockDate = currentDate;
    int quantity = ui->quantitySpin->value();
    double price = ui->priceSpin->value();
    int threshold = ui->thresholdSpin->value();

    if (id.isEmpty() || name.isEmpty() || !prodDate.isValid() || !expiryDate.isValid()) {
        showWarningDialog("请输入有效的药品信息");
        return;
    }

     med.ini(id, name, category, prodDate, expiryDate, lastStockDate, quantity, price, threshold);
    if (pharmacy.addMedicine(med)) {
        showInfoDialog("药品添加成功");
        onSearchMedicine(); // 刷新显示
    } else {
        showWarningDialog("药品添加失败，可能ID已存在");
    }
}

void MainWindow::onSearchMedicine() {
    QString keyword = ui->searchEdit->text();
    QList<Medicine> medicines = pharmacy.searchMedicine(keyword);
    updateMedicineTable(medicines);
}

void MainWindow::onStockIn() {
    QString id = ui->stockIdEdit->text();
    int quantity = ui->stockQuantitySpin->value();

    if (id.isEmpty() || quantity <= 0) {
        showWarningDialog("请输入有效的入库信息");
        return;
    }

    if (pharmacy.stockIn(id, quantity, currentDate)) {
        showInfoDialog("入库成功");
        onSearchMedicine(); // 刷新显示
    } else {
        showWarningDialog("入库失败，药品不存在或数量无效");
    }
}

void MainWindow::onStockOut() {
    QString id = ui->stockIdEdit->text();
    int quantity = ui->stockQuantitySpin->value();

    if (id.isEmpty() || quantity <= 0) {
        showWarningDialog("请输入有效的出库信息");
        return;
    }

    if (pharmacy.stockOut(id, quantity, currentDate)) {
        showInfoDialog("出库成功");
        onSearchMedicine(); // 刷新显示
    } else {
        showWarningDialog("出库失败，药品不存在或库存不足");
    }
}
void MainWindow::onShowLowStock() {
    QList<Medicine> medicines = pharmacy.getLowStockMedicines(currentDate);

    if (medicines.isEmpty()) {
        QMessageBox::information(this, "库存提示", "当前没有库存不足的药品");
    } else {
        // 显示详细信息的弹窗
        QString message = "以下药品库存不足：\n\n";
        for (const Medicine& med : medicines) {
            message += QString("%1 (ID: %2) - 当前库存: %3 (阈值: %4)\n")
                           .arg(med.getName())
                           .arg(med.getId())
                           .arg(med.getQuantity())
                           .arg(med.getWarningThreshold());
        }

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("库存不足药品");
        msgBox.setText(message);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        // 添加"查看详情"按钮
        QPushButton *detailButton = msgBox.addButton("查看详情", QMessageBox::ActionRole);

        if (msgBox.exec() == QMessageBox::Ok) {
            // 用户点击了确定
            updateMedicineTable(medicines);
        } else if (msgBox.clickedButton() == detailButton) {
            // 用户点击了查看详情
            updateMedicineTable(medicines);
            ui->tabWidget->setCurrentIndex(0); // 切换到药品列表标签页
        }
    }
}

void MainWindow::onShowNearExpiry() {
    QList<Medicine> medicines = pharmacy.getNearExpiryMedicines(currentDate);

    if (medicines.isEmpty()) {
        QMessageBox::information(this, "临期提示", "当前没有临期药品");
    } else {
        // 显示详细信息的弹窗
        QString message = "以下药品即将过期：\n\n";
        for (const Medicine& med : medicines) {
            int daysLeft = med.getExpiryDate() - currentDate;
            message += QString("%1 (ID: %2) - 过期日期: %3 (剩余%4天)\n")
                           .arg(med.getName())
                           .arg(med.getId())
                           .arg(med.getExpiryDate().toString())
                           .arg(daysLeft);
        }

        QMessageBox msgBox(this);
        msgBox.setWindowTitle("临期药品警告");
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);

        // 添加"查看详情"按钮
        QPushButton *detailButton = msgBox.addButton("查看详情", QMessageBox::ActionRole);

        if (msgBox.exec() == QMessageBox::Ok) {
            // 用户点击了确定
            updateMedicineTable(medicines);
        } else if (msgBox.clickedButton() == detailButton) {
            // 用户点击了查看详情
            updateMedicineTable(medicines);
            ui->tabWidget->setCurrentIndex(0); // 切换到药品列表标签页
        }
    }
}
void MainWindow::onSellMedicine() {
    QString id = ui->sellIdEdit->text();
    int quantity = ui->sellQuantitySpin->value();

    if (id.isEmpty() || quantity <= 0) {
        showWarningDialog("请输入有效的销售信息");
        return;
    }

    if (pharmacy.sellMedicine(id, quantity, currentDate)) {
        showInfoDialog("销售成功");
        onSearchMedicine(); // 刷新显示
    } else {
        showWarningDialog("销售失败，药品不存在或库存不足");
    }
}

void MainWindow::onShowSalesReport() {
    double daily = pharmacy.getDailySales(currentDate);
    double monthly = pharmacy.getMonthlySales(currentDate.getYear(), currentDate.getMonth());
    double yearly = pharmacy.getYearlySales(currentDate.getYear());

    QString report = QString("销售报表:\n"
                           "今日销售额: %1 元\n"
                           "本月销售额: %2 元\n"
                           "本年销售额: %3 元")
                   .arg(daily).arg(monthly).arg(yearly);

    showInfoDialog(report);
}

void MainWindow::onShowCategoryReport() {
    QMap<QString, double> categorySales = pharmacy.getCategorySales();
    if (categorySales.isEmpty()) {
        showInfoDialog("暂无销售类别数据");
        return;
    }

    QString report = "按类别销售统计:\n";
    for (auto it = categorySales.begin(); it != categorySales.end(); ++it) {
        report += QString("%1: %2 元\n").arg(it.key()).arg(it.value());
    }

    showInfoDialog(report);
}

void MainWindow::updateMedicineTable(const QList<Medicine>& medicines) {
    ui->medicineTable->setRowCount(medicines.size());

    for (int i = 0; i < medicines.size(); ++i) {
        const Medicine& med = medicines[i];

        // 创建物品并设置文本
        auto createItem = [](const QString& text) {
            QTableWidgetItem* item = new QTableWidgetItem(text);
            return item;
        };

        ui->medicineTable->setItem(i, 0, createItem(med.getId()));
        ui->medicineTable->setItem(i, 1, createItem(med.getName()));
        ui->medicineTable->setItem(i, 2, createItem(med.getCategory()));
        ui->medicineTable->setItem(i, 3, createItem(med.getProductionDate().toString()));
        ui->medicineTable->setItem(i, 4, createItem(med.getExpiryDate().toString()));
        ui->medicineTable->setItem(i, 5, createItem(med.getLastStockDate().toString()));
        ui->medicineTable->setItem(i, 6, createItem(QString::number(med.getQuantity())));
        ui->medicineTable->setItem(i, 7, createItem(QString::number(med.getPrice(), 'f', 2)));

        // 标记低库存药品
        if (med.isLowStock()) {
            for (int col = 0; col < ui->medicineTable->columnCount(); ++col) {
                ui->medicineTable->item(i, col)->setForeground(Qt::red);
            }
        }

        // 标记临期药品
        if (med.isNearExpiry(currentDate)) {
            for (int col = 0; col < ui->medicineTable->columnCount(); ++col) {
                ui->medicineTable->item(i, col)->setBackground(QColor(255, 255, 0)); // 黄色背景
            }
        }
    }
}

void MainWindow::showWarningDialog(const QString& message) {
    QMessageBox::warning(this, "警告", message);
}

void MainWindow::showInfoDialog(const QString& message) {
    QMessageBox::information(this, "信息", message);
}

Date MainWindow::getDateFromInput(const QString& dateStr) {
    QStringList parts = dateStr.split('-');
    if (parts.size() != 3) return Date();
    return Date(parts[0].toInt(), parts[1].toInt(), parts[2].toInt());
}
