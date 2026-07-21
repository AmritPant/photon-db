#pragma once

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <vector>
#include <string>

class MainWindow : public QMainWindow {
    // Q_OBJECT is required on any class that uses Qt's signal/slot system
    // (i.e. "when this button is clicked, run this function")
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    // "slots" are just normal member functions that get connected to signals
    // (e.g. connect(button, clicked, this, &MainWindow::onSendClicked))
    void onConnectClicked();
    void onSendClicked();
    void onSocketConnected();
    void onSocketDisconnected();
    void onSocketReadyRead();
    void onSocketError();

private:
    // --- UI widgets ---
    QLineEdit *hostInput;
    QLineEdit *portInput;
    QPushButton *connectButton;
    QLabel *statusLabel;

    QTextEdit *logView;      // shows the conversation history
    QLineEdit *commandInput; // where the user types e.g. "SET foo bar"
    QPushButton *sendButton;

    // --- networking ---
    QTcpSocket *socket;

    // --- helpers ---
    void appendLog(const QString &text);
    void setConnectedState(bool connected);

    // Encodes a command like {"SET", "foo", "bar"} into RESP array format:
    // *3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n
    static QByteArray encodeRespArray(const std::vector<std::string> &args);

    // Splits "SET foo bar" into {"SET", "foo", "bar"}.
    // NOTE: this is a naive split on spaces -- it does not handle quoted
    // strings with spaces inside them (e.g. SET name "John Doe"). That's a
    // good improvement to make later.
    static std::vector<std::string> splitCommand(const QString &line);

    // Turns raw RESP reply bytes into a human-readable string for the log.
    // Handles the reply types your server currently sends:
    //   +OK\r\n            (simple string)
    //   -ERR ...\r\n        (error)
    //   :123\r\n            (integer)
    //   $5\r\nhello\r\n     (bulk string)
    //   $-1\r\n             (null bulk string)
    //   *N\r\n...           (array, e.g. from EXEC)
    static QString decodeRespReply(const QByteArray &data);
};