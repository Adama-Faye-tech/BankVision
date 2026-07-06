#pragma once

#include <QString>

enum class TypeCompte { COURANT, EPARGNE, PROFESSIONNEL };
enum class StatutCompte { ACTIF, BLOQUE, FERME };

class CompteBancaire {
public:
    CompteBancaire();
    CompteBancaire(int id, QString iban, TypeCompte type, double solde);

    bool deposer(double montant);
    bool retirer(double montant);
    bool virer(CompteBancaire& dest, double montant);

    double getSolde() const;
    QString getIBAN() const;
    TypeCompte getType() const;
    StatutCompte getStatut() const;
    void setStatut(StatutCompte s);

private:
    int id;
    QString iban;
    double solde;
    TypeCompte type;
    StatutCompte statut;
};
