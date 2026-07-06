#pragma once
#include <QString>
#include <QDateTime>

enum class StatutPret {
    EN_ATTENTE,
    APPROUVE,
    REFUSE,
    REMBOURSE
};

/**
 * @brief Représente une demande de prêt bancaire.
 */
class Pret {
public:
    Pret() = default;
    Pret(int id, int clientId, const QString& iban, double montant,
         double tauxAnnuel, int dureeMois);

    // Accesseurs
    int    getId()        const { return id; }
    int    getClientId()  const { return clientId; }
    QString getIban()     const { return iban; }
    double getMontant()   const { return montant; }
    double getTaux()      const { return tauxAnnuel; }
    int    getDuree()     const { return dureeMois; }
    StatutPret getStatut() const { return statut; }
    QDateTime getDate()   const { return dateDemande; }

    // Calculs financiers
    double getMensualite() const;
    double getCoutTotal()  const;
    double getCoutInterets() const;

    void setStatut(StatutPret s) { statut = s; }

    QString getStatutStr() const;

private:
    int        id         = 0;
    int        clientId   = 0;
    QString    iban;
    double     montant    = 0.0;
    double     tauxAnnuel = 0.0; // en % ex: 3.5 pour 3.5%
    int        dureeMois  = 12;
    StatutPret statut     = StatutPret::EN_ATTENTE;
    QDateTime  dateDemande;
};
