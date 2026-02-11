#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QString>
#include <QSettings>
#include <QCloseEvent>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QColor>
#include <QVector>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QLoggingCategory>
#include <QDebug>
#include <memory>

#include "client.h"

namespace Ui {
class Window;
}

class Window : public QWidget
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);
    ~Window();

private slots:
    void on_pb_connect_clicked();
    void on_pb_push_clicked();
    void on_pb_disconnect_clicked();
    void elemsEnabled(const bool flag);
    void on_sb_size_valueChanged(int arg1);
    void receivedDataFromServer(const QByteArray& data);
    void print(const QString& text, const QColor& color);

private:
    Ui::Window *ui;

    int numPort{};
    QString ipAddress;
    int sizePackageData{};

    std::unique_ptr<Client> client;
    std::unique_ptr<QRegularExpressionValidator> validator;

    void readingConfig(const QString& path = "./client.conf");
    void writeConfig(const QString& path = "./client.conf");

    void closeEvent(QCloseEvent *e) override;
};

#endif // WINDOW_H
