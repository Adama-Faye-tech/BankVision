#pragma once
#include <QString>
#include <QVector>
#include <QMap>
#include "Client.h"
#include "Pret.h"

/**
 * @brief Agrégateur principal : contient les clients et les prêts.
 */
class Banque {
public:
    static Banque& instance() {
        static Banque b;
        return b;
    }

    void ajouterClient(const Client& c) { clients.append(c); }
    void ajouterPret(const Pret& p)     { prets.append(p); }

    Client*          getClientParId(int id);
    Client*          getClientParLogin(const QString& login);
    CompteBancaire*  getCompteParIBAN(const QString& iban);
    QVector<Client>& getClients()        { return clients; }
    QVector<Pret>&   getPrets()          { return prets; }

    // Génère un IBAN unique
    QString genererIBAN(const QString& prefix = "FR");

private:
    Banque() = default;
    QVector<Client>  clients;
    QVector<Pret>    prets;
    int nextIbanNum = 1000;
};
