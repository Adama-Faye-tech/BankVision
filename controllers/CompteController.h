#pragma once
#include <QObject>
#include <QString>
#include "models/Client.h"
#include "models/CompteBancaire.h"
#include "models/Pret.h"

/**
 * @brief Contrôleur des opérations bancaires : dépôt, retrait, virement, prêt.
 */
class CompteController : public QObject {
    Q_OBJECT
public:
    explicit CompteController(Client* client, QObject* parent = nullptr);

    // Sélection du compte actif
    void setCompteActif(const QString& iban);
    CompteBancaire* getCompteActif() { return compteActif; }
    Client*         getClient()      { return client; }

    // Opérations
    bool deposer(double montant, const QString& description = "", const QString& cat = "depot");
    bool retirer(double montant, const QString& description = "", const QString& cat = "retrait");
    bool virerVers(const QString& ibanDest, double montant, const QString& description = "");

    // Prêts
    int  demanderPret(double montant, double taux, int dureeMois);
    QVector<Pret> getPrets() const;

    // Compte courant
    bool creerNouveauCompte(TypeCompte type, double soldeInitial = 0.0);

    // Export
    bool exporterCSV(const QString& cheminFichier);

signals:
    void operationReussie(const QString& message);
    void operationEchouee(const QString& raison);
    void soldeModifie(double nouveauSolde);

private:
    Client*         client      = nullptr;
    CompteBancaire* compteActif = nullptr;
};
