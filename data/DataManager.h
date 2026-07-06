#pragma once
#include <QString>
#include <QSqlDatabase>
#include <QVector>
#include "models/Client.h"
#include "models/Pret.h"

/**
 * @brief Gestionnaire Singleton de la base de données SQLite.
 *        Initialise les tables et fournit les méthodes CRUD.
 */
class DataManager {
public:
    static DataManager& instance() {
        static DataManager dm;
        return dm;
    }

    bool initialiser(const QString& cheminDB);
    bool estInitialise() const { return initialise; }

    // --- Clients & Authentification ---
    bool   creerClient(const QString& login, const QString& motDePasseHash,
                       const QString& prenom, const QString& nom, const QString& email,
                       bool admin = false);
    Client chargerClient(const QString& login);
    bool   loginExiste(const QString& login) const;
    QString getHashMotDePasse(const QString& login) const;
    QVector<Client> chargerTousClients();

    // --- Comptes ---
    int    creerCompte(int clientId, const QString& iban,
                       TypeCompte type, double soldeInitial);
    bool   sauvegarderSolde(const QString& iban, double solde);
    QVector<CompteBancaire> chargerComptes(int clientId);

    // --- Transactions ---
    bool sauvegarderTransaction(const QString& iban, const Transaction& t);
    QVector<Transaction> chargerTransactions(const QString& iban, int limite = 100);

    // --- Prêts ---
    int  creerPret(const Pret& p);
    bool mettreAJourStatutPret(int pretId, StatutPret statut);
    QVector<Pret> chargerPrets(int clientId = -1);

    // --- Export ---
    bool exporterCSV(const QString& iban, const QString& cheminFichier);

private:
    DataManager() = default;
    bool   initialise = false;
    QSqlDatabase db;
    bool creerTables();
    void insererDonneesTest();
};
