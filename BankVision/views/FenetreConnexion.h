#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class FenetreConnexion : public QWidget {
    Q_OBJECT

public:
    explicit FenetreConnexion(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();

private:
    QLineEdit *username;
    QLineEdit *password;
    QPushButton *btnLogin;
};