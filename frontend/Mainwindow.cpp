#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("PhotonDB Client");
    resize(600, 400);

    // --- Build the UI ---
    // Qt Widgets are arranged with "layouts". A QVBoxLayout stacks widgets
    // top to bottom; a QHBoxLayout arranges them left to right. You nest
    // them to build more complex screens.

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    // Row 1: host / port / connect button
    auto *connectRow = new QHBoxLayout();
    hostInput = new QLineEdit("127.0.0.1");
    portInput = new QLineEdit("6379");
    connectButton = new QPushButton("Connect");
    statusLabel = new QLabel("Disconnected");

    connectRow->addWidget(new QLabel("Host:"));
    connectRow->addWidget(hostInput);
    connectRow->addWidget(new QLabel("Port:"));
    connectRow->addWidget(portInput);
    connectRow->addWidget(connectButton);
    connectRow->addWidget(statusLabel);

    // Row 2: the log of everything sent/received
    logView = new QTextEdit();
    logView->setReadOnly(true);

    // Row 3: command input + send button
    auto *sendRow = new QHBoxLayout();
    commandInput = new QLineEdit();
    commandInput->setPlaceholderText("e.g. SET foo bar");
    sendButton = new QPushButton("Send");
    sendRow->addWidget(commandInput);
    sendRow->addWidget(sendButton);

    mainLayout->addLayout(connectRow);
    mainLayout->addWidget(logView);
    mainLayout->addLayout(sendRow);

    setCentralWidget(central);

    // --- Networking ---
    socket = new QTcpSocket(this);

    // connect(sender, signal, receiver, slot)
    // This wires "button was clicked" to "run this function". This is the
    // whole trick behind Qt: everything reacts to signals instead of you
    // writing your own event loop.
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(commandInput, &QLineEdit::returnPressed, this, &MainWindow::onSendClicked);

    connect(socket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::onSocketError);

    setConnectedState(false);
}

void MainWindow::onConnectClicked() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
        return;
    }

    QString host = hostInput->text();
    quint16 port = static_cast<quint16>(portInput->text().toUInt());

    appendLog(QString("Connecting to %1:%2 ...").arg(host).arg(port));
    socket->connectToHost(host, port);
}

void MainWindow::onSocketConnected() {
    appendLog("Connected.");
    setConnectedState(true);
}

void MainWindow::onSocketDisconnected() {
    appendLog("Disconnected.");
    setConnectedState(false);
}

void MainWindow::onSocketError() {
    appendLog("Socket error: " + socket->errorString());
}

void MainWindow::onSendClicked() {
    QString line = commandInput->text().trimmed();
    if (line.isEmpty()) return;
    if (socket->state() != QAbstractSocket::ConnectedState) {
        appendLog("Not connected -- press Connect first.");
        return;
    }

    std::vector<std::string> args = splitCommand(line);
    if (args.empty()) return;

    QByteArray encoded = encodeRespArray(args);
    socket->write(encoded);

    appendLog("> " + line);
    commandInput->clear();
}

void MainWindow::onSocketReadyRead() {
    QByteArray data = socket->readAll();
    appendLog("< " + decodeRespReply(data));
}

void MainWindow::appendLog(const QString &text) {
    logView->append(text);
}

void MainWindow::setConnectedState(bool connected) {
    statusLabel->setText(connected ? "Connected" : "Disconnected");
    connectButton->setText(connected ? "Disconnect" : "Connect");
    sendButton->setEnabled(connected);
}

std::vector<std::string> MainWindow::splitCommand(const QString &line) {
    std::vector<std::string> result;
    std::istringstream stream(line.toStdString());
    std::string word;
    while (stream >> word) {
        result.push_back(word);
    }
    return result;
}

QByteArray MainWindow::encodeRespArray(const std::vector<std::string> &args) {
    // Mirrors what resp_to_text expects on the server side:
    // *<count>\r\n
    // $<len>\r\n<word>\r\n   (repeated for each argument)
    std::string resp = "*" + std::to_string(args.size()) + "\r\n";
    for (const auto &arg : args) {
        resp += "$" + std::to_string(arg.size()) + "\r\n";
        resp += arg + "\r\n";
    }
    return QByteArray::fromStdString(resp);
}

QString MainWindow::decodeRespReply(const QByteArray &data) {
    if (data.isEmpty()) return "(empty reply)";

    char type = data[0];
    QString body = QString::fromUtf8(data.mid(1)); // everything after the type byte
    // Strip trailing \r\n for display purposes
    body = body.trimmed();

    switch (type) {
        case '+': // simple string, e.g. +OK
            return "OK: " + body;
        case '-': // error, e.g. -ERR unknown command
            return "ERROR: " + body;
        case ':': // integer, e.g. :5
            return "(integer) " + body;
        case '$': // bulk string, e.g. $5\r\nhello  or  $-1 for null
            if (body.startsWith("-1")) return "(nil)";
            // body is "<len>\r\n<value>" -- drop the length line
            return body.section("\r\n", 1);
        case '*': // array, e.g. from EXEC or MULTI results -- shown raw for now
            return "(array) " + QString::fromUtf8(data);
        default:
            // Unrecognized/partial data -- just show the raw bytes so you can
            // see what actually came over the wire while debugging
            return QString::fromUtf8(data);
    }
}