#pragma once

#include <QWidget>

class IndicateurSolde : public QWidget {
    Q_OBJECT

public:
    explicit IndicateurSolde(QWidget *parent = nullptr);

    void setValeur(double v);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double valeur;
};