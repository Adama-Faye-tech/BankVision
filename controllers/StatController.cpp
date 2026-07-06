#include "StatController.h"
#include <QDate>
#include <algorithm>
#include <cmath>

StatController::StatController(CompteBancaire* compte, QObject* parent)
    : QObject(parent), compte(compte)
{}

QVector<double> StatController::getSoldesMensuels(int mois) const {
    if (!compte) return QVector<double>(mois, 0.0);
    return compte->getSoldesMensuels(mois);
}

QMap<QString, double> StatController::getDepensesParCategorie(int mois) const {
    if (!compte) return {};
    return compte->getDepensesParCategorie(mois);
}

QVector<double> StatController::getRecettesMensuelles(int mois) const {
    if (!compte) return QVector<double>(mois, 0.0);
    QVector<double> result;
    QDateTime now = QDateTime::currentDateTime();
    for (int m = mois - 1; m >= 0; --m) {
        QDateTime debut = now.addMonths(-m);
        debut.setDate(QDate(debut.date().year(), debut.date().month(), 1));
        debut.setTime(QTime(0, 0));
        QDateTime fin = debut.addMonths(1).addSecs(-1);
        double total = 0.0;
        for (const auto& t : compte->getHistorique(1000)) {
            if (t.date >= debut && t.date <= fin &&
                (t.type == "depot" || t.type == "virement_reception"))
                total += t.montant;
        }
        result.append(total);
    }
    return result;
}

QVector<double> StatController::getDepensesMensuelles(int mois) const {
    if (!compte) return QVector<double>(mois, 0.0);
    QVector<double> result;
    QDateTime now = QDateTime::currentDateTime();
    for (int m = mois - 1; m >= 0; --m) {
        QDateTime debut = now.addMonths(-m);
        debut.setDate(QDate(debut.date().year(), debut.date().month(), 1));
        debut.setTime(QTime(0, 0));
        QDateTime fin = debut.addMonths(1).addSecs(-1);
        double total = 0.0;
        for (const auto& t : compte->getHistorique(1000)) {
            if (t.date >= debut && t.date <= fin &&
                (t.type == "retrait" || t.type == "virement_envoi"))
                total += t.montant;
        }
        result.append(total);
    }
    return result;
}

double StatController::getSoldeMoyen(int jours) const {
    if (!compte) return 0.0;
    return compte->getSoldeMoyen(jours);
}

double StatController::getDepenseMax(int mois) const {
    if (!compte) return 0.0;
    double maxDep = 0.0;
    QDateTime limite = QDateTime::currentDateTime().addMonths(-mois);
    for (const auto& t : compte->getHistorique(1000)) {
        if (t.date >= limite && (t.type == "retrait" || t.type == "virement_envoi"))
            maxDep = qMax(maxDep, t.montant);
    }
    return maxDep;
}

double StatController::getTauxEpargne(int mois) const {
    double recettes = getTotalRecettes(mois);
    double depenses = getTotalDepenses(mois);
    if (recettes <= 0.0) return 0.0;
    return qMax(0.0, (recettes - depenses) / recettes * 100.0);
}

double StatController::getTotalDepenses(int mois) const {
    if (!compte) return 0.0;
    return compte->getTotalDepenses(mois);
}

double StatController::getTotalRecettes(int mois) const {
    if (!compte) return 0.0;
    return compte->getTotalRecettes(mois);
}

QStringList StatController::getLabelsMois(int mois) const {
    QStringList labels;
    QDateTime now = QDateTime::currentDateTime();
    QStringList nomsMois = {"Jan", "Fév", "Mar", "Avr", "Mai", "Jun",
                             "Jul", "Aoû", "Sep", "Oct", "Nov", "Déc"};
    for (int m = mois - 1; m >= 0; --m) {
        QDate d = now.addMonths(-m).date();
        labels << nomsMois[d.month() - 1];
    }
    return labels;
}
