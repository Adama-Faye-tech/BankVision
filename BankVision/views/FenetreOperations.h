#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

class FenetreOperations : public QWidget {
    Q_OBJECT

public:
    explicit FenetreOperations(QWidget *parent = nullptr);

private slots:
    void depot();
    void retrait();

private:
    QLineEdit *montant;
    QPushButton *btnDepot;
    QPushButton *btnRetrait;
};