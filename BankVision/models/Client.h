#pragma once

#include <QString>

class Client {
private:
    int id;
    QString nom;
    QString prenom;
    QString email;

public:
    Client();
    Client(int id, QString nom, QString prenom, QString email);

    int getId() const;
    QString getNom() const;
    QString getPrenom() const;
    QString getEmail() const;

    void setNom(const QString& n);
    void setPrenom(const QString& p);
    void setEmail(const QString& e);
};