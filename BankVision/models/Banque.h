#pragma once

#include <QVector>
#include "Client.h"
#include "CompteBancaire.h"

class Banque {
private:
    QVector<Client> clients;
    QVector<CompteBancaire> comptes;

public:
    Banque();

    void ajouterClient(const Client& c);
    void ajouterCompte(const CompteBancaire& c);

    int nombreClients() const;
    int nombreComptes() const;
};