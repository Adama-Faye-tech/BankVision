#pragma once
#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>
#include "models/CompteBancaire.h"

/**
 * @brief Fournit les données statistiques pour les graphiques et les KPIs.
 */
class StatController : public QObject {
    Q_OBJECT
public:
    explicit StatController(CompteBancaire* compte, QObject* parent = nullptr);

    void setCompte(CompteBancaire* c) { compte = c; }

    // Données pour les graphiques
    QVector<double> getSoldesMensuels(int mois = 12) const;
    QMap<QString, double> getDepensesParCategorie(int mois = 3) const;
    QVector<double> getRecettesMensuelles(int mois = 6) const;
    QVector<double> getDepensesMensuelles(int mois = 6) const;

    // KPIs
    double getSoldeMoyen(int jours = 30) const;
    double getDepenseMax(int mois = 1) const;
    double getTauxEpargne(int mois = 1) const;
    double getTotalDepenses(int mois = 1) const;
    double getTotalRecettes(int mois = 1) const;

    // Labels pour les axes
    QStringList getLabelsMois(int mois = 12) const;

private:
    CompteBancaire* compte = nullptr;
};
