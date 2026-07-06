#pragma once

class Pret {
private:
    int id;
    int compteId;
    double montant;
    double taux;
    int dureeMois;

public:
    Pret();
    Pret(int id, int compteId, double montant, double taux, int duree);

    double calculerMensualite() const;
    double getMontant() const;
};