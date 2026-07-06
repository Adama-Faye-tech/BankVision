#include "AuthController.h"
#include "data/DataManager.h"
#include <QCryptographicHash>
#include <QDateTime>

// ---------------------------------------------------------------------------

QString AuthController::hacher(const QString& texte) {
    return QCryptographicHash::hash(texte.toUtf8(), QCryptographicHash::Sha256).toHex();
}

bool AuthController::connecter(const QString& login, const QString& motDePasse) {
    // Vérifier si le compte est verrouillé
    if (verrouille) {
        int secondes = secondesVerrouillage();
        if (secondes > 0) {
            emit connexionEchouee(QString("Compte verrouillé. Réessayez dans %1 secondes.").arg(secondes));
            return false;
        }
        // Débloquer après le délai
        verrouille = false;
        nbEssais   = 0;
    }

    // Vérifier l'existence du login
    if (!DataManager::instance().loginExiste(login)) {
        ++nbEssais;
        if (nbEssais >= MAX_ESSAIS) {
            verrouille       = true;
            dateVerrouillage = QDateTime::currentDateTime();
            emit connexionEchouee("Trop de tentatives. Compte verrouillé 30 secondes.");
        } else {
            emit connexionEchouee(QString("Login ou mot de passe incorrect. %1 essai(s) restant(s).").arg(essaisRestants()));
        }
        return false;
    }

    // Vérifier le mot de passe
    QString hash     = hacher(motDePasse);
    QString hashSave = DataManager::instance().getHashMotDePasse(login);

    if (hash != hashSave) {
        ++nbEssais;
        if (nbEssais >= MAX_ESSAIS) {
            verrouille       = true;
            dateVerrouillage = QDateTime::currentDateTime();
            emit connexionEchouee("Trop de tentatives. Compte verrouillé 30 secondes.");
        } else {
            emit connexionEchouee(QString("Mot de passe incorrect. %1 essai(s) restant(s).").arg(essaisRestants()));
        }
        return false;
    }

    // Connexion réussie
    nbEssais       = 0;
    verrouille     = false;
    clientData     = DataManager::instance().chargerClient(login);
    clientConnecte = &clientData;
    emit connexionReussie(clientConnecte);
    return true;
}

void AuthController::deconnecter() {
    clientConnecte = nullptr;
    emit deconnexion();
}

int AuthController::secondesVerrouillage() const {
    if (!verrouille) return 0;
    int elapsed = static_cast<int>(dateVerrouillage.secsTo(QDateTime::currentDateTime()));
    return qMax(0, DUREE_VERROU_S - elapsed);
}

bool AuthController::inscrireClient(const QString& login, const QString& mdp,
                                     const QString& prenom, const QString& nom,
                                     const QString& email) {
    if (DataManager::instance().loginExiste(login)) return false;
    return DataManager::instance().creerClient(login, hacher(mdp), prenom, nom, email, false);
}
