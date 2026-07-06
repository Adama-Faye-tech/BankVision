#pragma once
#include <QWidget>
#include <QLabel>
#include "controllers/StatController.h"
#include "views/widgets/GraphiqueSolde.h"
#include "views/widgets/CamembertDepenses.h"

/**
 * @brief Page des statistiques : courbe, camembert, histogramme recettes/dépenses, KPIs.
 */
class FenetreStatistiques : public QWidget {
    Q_OBJECT
public:
    explicit FenetreStatistiques(StatController* statCtrl, QWidget* parent = nullptr);
    void rafraichir();

private:
    StatController*    statCtrl   = nullptr;
    GraphiqueSolde*    graphSolde = nullptr;
    CamembertDepenses* camembert  = nullptr;
    GraphiqueSolde*    histogramme = nullptr;

    QLabel* lblSoldeMoyen  = nullptr;
    QLabel* lblDepMax      = nullptr;
    QLabel* lblTauxEpargne = nullptr;
    QLabel* lblTotalDep    = nullptr;

    QWidget* creerKPIs();
    QWidget* creerHistogramme();
};
