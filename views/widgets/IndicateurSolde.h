#pragma once
#include <QWidget>

/**
 * @brief Widget indicateur visuel du solde (barre colorée + valeur).
 */
class IndicateurSolde : public QWidget {
    Q_OBJECT
public:
    explicit IndicateurSolde(QWidget* parent = nullptr);

    void setSolde(double solde, double objectif = 5000.0);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    double solde    = 0.0;
    double objectif = 5000.0;
};
