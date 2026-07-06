#include "CompteBancaire.h"
#include <QMap>
#include <algorithm>
#include <cmath>

CompteBancaire::CompteBancaire(int id, const QString& iban, TypeCompte type,
                               int clientId, double soldeInitial)
    : id(id), iban(iban), type(type), clientId(clientId), solde(soldeInitial),
      statut(StatutCompte::ACTIF)
{}

// ---------------------------------------------------------------------------
// Opérations

bool CompteBancaire::deposer(double montant, const QString& desc, const QString& cat) {
    if (statut != StatutCompte::ACTIF || montant <= 0.0) return false;
    solde += montant;
    enregistrerTransaction("depot", montant, desc.isEmpty() ? "Dépôt" : desc, cat);
    return true;
}

bool CompteBancaire::retirer(double montant, const QString& desc, const QString& cat) {
    if (statut != StatutCompte::ACTIF || montant <= 0.0) return false;
    if (type != TypeCompte::PROFESSIONNEL && solde < montant) return false;
    solde -= montant;
    enregistrerTransaction("retrait", montant, desc.isEmpty() ? "Retrait" : desc, cat);
    return true;
}

bool CompteBancaire::virer(CompteBancaire& dest, double montant, const QString& desc) {
    if (statut != StatutCompte::ACTIF || dest.statut != StatutCompte::ACTIF) return false;
    if (montant <= 0.0 || solde < montant) return false;

    solde -= montant;
    enregistrerTransaction("virement_envoi", montant,
        desc.isEmpty() ? "Virement vers " + dest.getIBAN() : desc, "virement");

    dest.solde += montant;
    dest.enregistrerTransaction("virement_reception", montant,
        "Virement de " + iban, "virement");

    return true;
}

void CompteBancaire::enregistrerTransaction(const QString& type, double montant,
                                             const QString& desc, const QString& cat) {
    Transaction t;
    t.id          = nextTransactionId++;
    t.date        = QDateTime::currentDateTime();
    t.type        = type;
    t.montant     = montant;
    t.soldeApres  = solde;
    t.description = desc;
    t.categorie   = cat;
    historique.prepend(t); // Plus récent en premier
}

void CompteBancaire::ajouterTransaction(const Transaction& t) {
    historique.append(t);
    if (t.id >= nextTransactionId) nextTransactionId = t.id + 1;
}

// ---------------------------------------------------------------------------
// Accesseurs

QString CompteBancaire::getTypeStr() const {
    switch (type) {
        case TypeCompte::COURANT:       return "Courant";
        case TypeCompte::EPARGNE:       return "Épargne";
        case TypeCompte::PROFESSIONNEL: return "Professionnel";
    }
    return "Inconnu";
}

QVector<Transaction> CompteBancaire::getHistorique(int n) const {
    if (n <= 0 || historique.isEmpty()) return historique;
    return historique.mid(0, qMin(n, historique.size()));
}

// ---------------------------------------------------------------------------
// Statistiques

double CompteBancaire::getSoldeMoyen(int jours) const {
    if (historique.isEmpty()) return solde;
    QDateTime limite = QDateTime::currentDateTime().addDays(-jours);
    double sum = 0.0;
    int    cnt = 0;
    for (const auto& t : historique) {
        if (t.date >= limite) { sum += t.soldeApres; ++cnt; }
    }
    return cnt > 0 ? sum / cnt : solde;
}

QVector<double> CompteBancaire::getSoldesMensuels(int mois) const {
    QVector<double> result(mois, 0.0);
    QDateTime now = QDateTime::currentDateTime();
    for (int m = 0; m < mois; ++m) {
        QDateTime debut = now.addMonths(-(mois - 1 - m));
        debut.setDate(QDate(debut.date().year(), debut.date().month(), 1));
        debut.setTime(QTime(0, 0));
        QDateTime fin = debut.addMonths(1).addSecs(-1);
        double lastSolde = (m == mois - 1) ? solde : 0.0;
        for (const auto& t : historique) {
            if (t.date >= debut && t.date <= fin) {
                lastSolde = t.soldeApres;
                break;
            }
        }
        result[m] = lastSolde;
    }
    return result;
}

double CompteBancaire::getTotalDepenses(int mois) const {
    double total = 0.0;
    QDateTime limite = QDateTime::currentDateTime().addMonths(-mois);
    for (const auto& t : historique) {
        if (t.date >= limite && (t.type == "retrait" || t.type == "virement_envoi"))
            total += t.montant;
    }
    return total;
}

double CompteBancaire::getTotalRecettes(int mois) const {
    double total = 0.0;
    QDateTime limite = QDateTime::currentDateTime().addMonths(-mois);
    for (const auto& t : historique) {
        if (t.date >= limite && (t.type == "depot" || t.type == "virement_reception"))
            total += t.montant;
    }
    return total;
}

QMap<QString, double> CompteBancaire::getDepensesParCategorie(int mois) const {
    QMap<QString, double> cats;
    QDateTime limite = QDateTime::currentDateTime().addMonths(-mois);
    for (const auto& t : historique) {
        if (t.date >= limite && (t.type == "retrait" || t.type == "virement_envoi")) {
            QString c = t.categorie.isEmpty() ? "Autre" : t.categorie;
            cats[c] += t.montant;
        }
    }
    return cats;
}
