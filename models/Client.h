#pragma once
#include <QString>
#include <QVector>
#include "CompteBancaire.h"

/**
 * @brief Représente un client de la banque.
 */
class Client {
public:
    Client() = default;
    Client(int id, const QString& login, const QString& prenom,
           const QString& nom, const QString& email);

    // Accesseurs
    int     getId()      const { return id; }
    QString getLogin()   const { return login; }
    QString getPrenom()  const { return prenom; }
    QString getNom()     const { return nom; }
    QString getNomComplet() const { return prenom + " " + nom; }
    QString getEmail()   const { return email; }
    bool    isAdmin()    const { return admin; }

    // Mutateurs
    void setAdmin(bool a) { admin = a; }
    void setEmail(const QString& e) { email = e; }

    // Comptes
    void ajouterCompte(const CompteBancaire& c);
    QVector<CompteBancaire>& getComptes() { return comptes; }
    const QVector<CompteBancaire>& getComptes() const { return comptes; }
    CompteBancaire* getCompteParIBAN(const QString& iban);

private:
    int     id    = 0;
    QString login;
    QString prenom;
    QString nom;
    QString email;
    bool    admin = false;
    QVector<CompteBancaire> comptes;
};
