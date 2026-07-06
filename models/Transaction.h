#pragma once
#include <QString>
#include <QDateTime>

/**
 * @brief Représente une transaction bancaire unique.
 */
struct Transaction {
    int       id;
    QDateTime date;
    QString   type;        // "depot", "retrait", "virement_envoi", "virement_reception"
    double    montant;
    double    soldeApres;
    QString   description;
    QString   categorie;   // "alimentation", "loisirs", "transport", etc.
};
