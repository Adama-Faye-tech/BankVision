#pragma once
#include <QString>
#include "models/Client.h"

/**
 * @brief Gère l'authentification : hachage SHA-256, sessions, verrouillage.
 */
class AuthController : public QObject {
    Q_OBJECT
public:
    static AuthController& instance() {
        static AuthController ac;
        return ac;
    }

    bool connecter(const QString& login, const QString& motDePasse);
    void deconnecter();
    bool estConnecte() const    { return clientConnecte != nullptr; }
    bool estVerrouille() const  { return verrouille; }
    int  essaisRestants() const { return MAX_ESSAIS - nbEssais; }
    int  secondesVerrouillage() const;

    Client* getClientActif() { return clientConnecte; }

    static QString hacher(const QString& texte);
    bool inscrireClient(const QString& login, const QString& mdp,
                        const QString& prenom, const QString& nom,
                        const QString& email);

signals:
    void connexionReussie(Client* client);
    void connexionEchouee(const QString& raison);
    void deconnexion();

private:
    AuthController() = default;
    static constexpr int MAX_ESSAIS      = 3;
    static constexpr int DUREE_VERROU_S  = 30;

    Client* clientConnecte = nullptr;
    int     nbEssais       = 0;
    bool    verrouille     = false;
    QDateTime dateVerrouillage;

    // Stockage interne du client (ownership)
    Client clientData;
};
