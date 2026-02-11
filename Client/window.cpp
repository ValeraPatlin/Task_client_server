#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Window)
{
    ui->setupUi(this);

    client = std::make_unique<Client>(this);
    client->setSizePackageData(sizePackageData);

    connect(client.get(), &Client::connectedToServer, this, &Window::elemsEnabled);
    connect(client.get(), &Client::disconnectedFromServer, this, &Window::elemsEnabled);
    connect(client.get(), &Client::receivedDataFromServer, this, &Window::receivedDataFromServer);
    connect(client.get(), &Client::printInfo, this, &Window::print);

    readingConfig();

    ui->sb_port->setRange(1024, 65535);
    ui->sb_port->setValue(numPort);
    ui->sb_port->setWrapping(true);

    ui->sb_size->setRange(1, 99);
    ui->sb_size->setValue(sizePackageData);
    ui->sb_size->setWrapping(true);

    ui->le_ip->setText(ipAddress);
    ui->le_ip->setClearButtonEnabled(true);

    QRegularExpression rx{"^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"};
    validator = std::make_unique<QRegularExpressionValidator>(rx, ui->le_ip);
    ui->le_ip->setValidator(validator.get());

    elemsEnabled(false);

    ui->te_data->setReadOnly(true);
    ui->le_sendData->setClearButtonEnabled(true);
    ui->le_sendData->setPlaceholderText("Enter the data");
}

Window::~Window()
{
    delete ui;
}


void Window::receivedDataFromServer(const QByteArray &data)
{
    qDebug() << "Received from server:" << data.toHex(' ');

    QVector<quint8> numbers;

    for (int i = 0; i < data.size(); ++i)
    {
        quint8 byteValue = static_cast<quint8>(data.at(i));
        numbers.append(byteValue);
    }

    QString outputString;

    for (int i = 0; i < numbers.size(); ++i)
    {
        outputString += QString::number(numbers.at(i));

        if (i < numbers.size() - 1)
        {
            outputString += " ";
        }
    }
    print("Decoded numbers:  " + outputString + "\n", QColor("#000000"));
}

void Window::on_pb_connect_clicked()
{
    print("Connect to server................................", QColor("#000000"));
    client->connectToServer(ui->le_ip->text(), ui->sb_port->value());
}

void Window::on_pb_disconnect_clicked()
{
    print("Disconnect from the server..............", QColor("#000000"));
    client->disconnectFromServer();
}


void Window::on_sb_size_valueChanged(int arg1)
{
    client->setSizePackageData(arg1);
}

void Window::elemsEnabled(const bool flag)
{
    ui->pb_push->setEnabled(flag);
    ui->pb_disconnect->setEnabled(flag);
    ui->le_sendData->setEnabled(flag);
    ui->sb_size->setEnabled(flag);

    ui->le_ip->setEnabled(!flag);
    ui->sb_port->setEnabled(!flag);
}

void Window::readingConfig(const QString &path)
{
    qDebug() << Q_FUNC_INFO;

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Settings");

    numPort = settings.value("port", 2345).toInt();
    ipAddress = settings.value("ip", "127.0.0.1").toString();
    sizePackageData = settings.value("size", 4).toInt();

    settings.endGroup();
    settings.sync();
}

void Window::writeConfig(const QString &path)
{
    qDebug() << Q_FUNC_INFO;

    QSettings settings(path, QSettings::IniFormat);
    settings.beginGroup("Settings");

    settings.setValue("port", numPort);
    settings.setValue("ip", ipAddress);
    settings.setValue("size", sizePackageData);

    settings.endGroup();
    settings.sync();
}

void Window::print(const QString &text, const QColor &color)
{
    QTextCursor cursor = ui->te_data->textCursor();
    QTextCharFormat format;

    format.setForeground(color);
    cursor.setCharFormat(format);
    cursor.insertText(text);
}

void Window::closeEvent(QCloseEvent *e)
{
    numPort = ui->sb_port->value();
    ipAddress = ui->le_ip->text();
    sizePackageData = ui->sb_size->value();
    writeConfig();

    client->disconnectFromServer();
    e->accept();
    QWidget::closeEvent(e);
}


void Window::on_pb_push_clicked()
{
    QByteArray byteData;

    for (char ch : ui->le_sendData->text().toStdString())
    {
        byteData.append(static_cast<quint8>(ch));
    }

    client->sendData(byteData);
    ui->le_sendData->clear();
}
