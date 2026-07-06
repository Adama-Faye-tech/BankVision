#include "CompteBancaire.h"

CompteBancaire::CompteBancaire()
    : id(-1), solde(0), type(TypeCompte::COURANT), statut(StatutCompte::ACTIF) {}

CompteBancaire::CompteBancaire(int id, QString iban, TypeCompte type, double solde)
    : id(id), iban(iban), solde(solde), type(type), statut(StatutCompte::ACTIF) {}

bool CompteBancaire::deposer(double montant) {
    if(montant <= 0) return false;
    solde += montant;
    return true;
}

bool CompteBancaire::retirer(double montant) {
    if(montant <= 0 || montant > solde) return false;
    solde -= montant;
    return true;
}

bool CompteBancaire::virer(CompteBancaire& dest, double montant) {
    if(retirer(montant)) {
        dest.deposer(montant);
        return true;
    }
    return false;
}

double CompteBancaire::getSolde() const { return solde; }
QString CompteBancaire::getIBAN() const { return iban; }
TypeCompte CompteBancaire::getType() const { return type; }
StatutCompte CompteBancaire::getStatut() const { return statut; }

void CompteBancaire::setStatut(StatutCompte s) { statut = s; }