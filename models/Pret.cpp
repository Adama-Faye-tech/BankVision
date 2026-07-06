#include "Pret.h"
#include <cmath>

Pret::Pret(int id, int clientId, const QString& iban, double montant,
           double tauxAnnuel, int dureeMois)
    : id(id), clientId(clientId), iban(iban), montant(montant),
      tauxAnnuel(tauxAnnuel), dureeMois(dureeMois),
      statut(StatutPret::EN_ATTENTE),
      dateDemande(QDateTime::currentDateTime())
{}

/**
 * @brief Calcule la mensualité via la formule d'amortissement.
 *        m = P * (r * (1+r)^n) / ((1+r)^n - 1)
 */
double Pret::getMensualite() const {
    if (montant <= 0 || dureeMois <= 0) return 0.0;
    double r = (tauxAnnuel / 100.0) / 12.0; // taux mensuel
    if (r == 0.0) return montant / dureeMois;
    double facteur = std::pow(1.0 + r, dureeMois);
    return montant * (r * facteur) / (facteur - 1.0);
}

double Pret::getCoutTotal() const {
    return getMensualite() * dureeMois;
}

double Pret::getCoutInterets() const {
    return getCoutTotal() - montant;
}

QString Pret::getStatutStr() const {
    switch (statut) {
        case StatutPret::EN_ATTENTE: return "En attente";
        case StatutPret::APPROUVE:   return "Approuvé";
        case StatutPret::REFUSE:     return "Refusé";
        case StatutPret::REMBOURSE:  return "Remboursé";
    }
    return "Inconnu";
}
