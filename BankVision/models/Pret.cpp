#include "Pret.h"
#include <cmath>

Pret::Pret()
    : id(-1), compteId(-1), montant(0), taux(0), dureeMois(0) {}

Pret::Pret(int id, int compteId, double montant, double taux, int duree)
    : id(id), compteId(compteId), montant(montant), taux(taux), dureeMois(duree) {}

double Pret::calculerMensualite() const {
    double r = taux / 12 / 100;
    if(r == 0) return montant / dureeMois;

    return montant * r / (1 - pow(1 + r, -dureeMois));
}

double Pret::getMontant() const {
    return montant;
}