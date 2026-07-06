#pragma once
#include <QString>
#include <QVector>
#include <QDateTime>
#include "Transaction.h"

enum class TypeCompte {
    COURANT,
    EPARGNE,
    PROFESSIONNEL
};

enum class StatutCompte {
    ACTIF,
    BLOQUE,
    FERME
};

/**
 * @brief Modèle d'un compte bancaire avec ses opérations.
 */
class CompteBancaire {
public:
    CompteBancaire() = default;
    CompteBancaire(int id, const QString& iban, TypeCompte type,
                   int clientId, double soldeInitial = 0.0);

    // --- Opérations ---
    bool deposer(double montant, const QString& desc = "", const QString& cat = "depot");
    bool retirer(double montant, const QString& desc = "", const QString& cat = "retrait");
    bool virer(CompteBancaire& dest, double montant, const QString& desc = "");

    // --- Accesseurs ---
    int          getId()     const { return id; }
    double       getSolde()  const { return solde; }
    QString      getIBAN()   const { return iban; }
    int          getClientId() const { return clientId; }
    TypeCompte   getType()   const { return type; }
    StatutCompte getStatut() const { return statut; }
    QString      getTypeStr() const;

    void setSolde(double s)     { solde = s; }
    void setStatut(StatutCompte s) { statut = s; }

    QVector<Transaction> getHistorique(int n = 30) const;
    void ajouterTransaction(const Transaction& t);

    // --- Statistiques ---
    double          getSoldeMoyen(int jours = 30) const;
    QVector<double> getSoldesMensuels(int mois = 12) const;
    double          getTotalDepenses(int mois = 1) const;
    double          getTotalRecettes(int mois = 1) const;
    QMap<QString, double> getDepensesParCategorie(int mois = 1) const;

private:
    int          id       = 0;
    int          clientId = 0;
    QString      iban;
    double       solde    = 0.0;
    TypeCompte   type     = TypeCompte::COURANT;
    StatutCompte statut   = StatutCompte::ACTIF;
    QVector<Transaction> historique;

    int nextTransactionId = 1;
    void enregistrerTransaction(const QString& t, double m,
                                const QString& d, const QString& cat);
};
