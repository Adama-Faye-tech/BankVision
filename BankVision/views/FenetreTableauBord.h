#pragma once

#include <QWidget>
#include <QPushButton>

class FenetreTableauBord : public QWidget {
    Q_OBJECT

public:
    explicit FenetreTableauBord(QWidget *parent = nullptr);

private slots:
    void ouvrirOperations();
    void ouvrirStats();

private:
    QPushButton *btnOperations;
    QPushButton *btnStats;
};