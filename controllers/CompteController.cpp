#include "CompteController.h"
#include "data/DataManager.h"
#include "models/Banque.h"
#include "controllers/AuthController.h"
#include <QRandomGenerator>

CompteController::CompteController(Client* client, QObject* parent)
    : QObject(parent), client(client) {
    if (!client) return;

    // Si le client n'a pas de comptes en mémoire, les recharger depuis la BD
    if (client->getComptes().isEmpty()) {
        for (auto& c : DataManager::instance().chargerComptes(client->getId()))
            client->ajouterCompte(c);
    }

    // Sélectionner le premier compte par défaut
    if (!client->getComptes().isEmpty())
        compteActif = &client->getComptes().first();
}

void CompteController::setCompteActif(const QString& iban) {
    if (!client) return;
    compteActif = client->getCompteParIBAN(iban);
}

// ---------------------------------------------------------------------------

bool CompteController::deposer(double montant, const QString& desc, const QString& cat) {
    if (!compteActif) { emit operationEchouee("Aucun compte sélectionné."); return false; }
    if (montant <= 0) { emit operationEchouee("Montant invalide."); return false; }

    if (compteActif->deposer(montant, desc, cat)) {
        // Sauvegarder en base
        DataManager::instance().sauvegarderSolde(compteActif->getIBAN(), compteActif->getSolde());
        auto hist = compteActif->getHistorique(1);
        if (!hist.isEmpty())
            DataManager::instance().sauvegarderTransaction(compteActif->getIBAN(), hist.first());
        emit soldeModifie(compteActif->getSolde());
        emit operationReussie(QString("Dépôt de %1 € effectué.").arg(montant, 0, 'f', 2));
        return true;
    }
    emit operationEchouee("Opération impossible (compte bloqué ou montant invalide).");
    return false;
}

bool CompteController::retirer(double montant, const QString& desc, const QString& cat) {
    if (!compteActif) { emit operationEchouee("Aucun compte sélectionné."); return false; }
    if (montant <= 0) { emit operationEchouee("Montant invalide."); return false; }

    if (compteActif->retirer(montant, desc, cat)) {
        DataManager::instance().sauvegarderSolde(compteActif->getIBAN(), compteActif->getSolde());
        auto hist = compteActif->getHistorique(1);
        if (!hist.isEmpty())
            DataManager::instance().sauvegarderTransaction(compteActif->getIBAN(), hist.first());
        emit soldeModifie(compteActif->getSolde());
        emit operationReussie(QString("Retrait de %1 € effectué.").arg(montant, 0, 'f', 2));
        return true;
    }
    emit operationEchouee("Solde insuffisant ou compte bloqué.");
    return false;
}

bool CompteController::virerVers(const QString& ibanDest, double montant, const QString& desc) {
    if (!compteActif) { emit operationEchouee("Aucun compte sélectionné."); return false; }
    if (montant <= 0) { emit operationEchouee("Montant invalide."); return false; }

    // Chercher le compte destination
    CompteBancaire* dest = nullptr;

    // D'abord parmi les comptes du client
    dest = client->getCompteParIBAN(ibanDest);

    // Sinon dans la banque globale
    if (!dest) dest = Banque::instance().getCompteParIBAN(ibanDest);

    if (!dest) {
        // Virement externe : créer un compte fictif temporaire
        // (dans une vraie app, cela passerait par un réseau interbancaire)
        CompteBancaire compteExt(-1, ibanDest, TypeCompte::COURANT, -1, 0.0);
        if (compteActif->virer(compteExt, montant, desc)) {
            DataManager::instance().sauvegarderSolde(compteActif->getIBAN(), compteActif->getSolde());
            auto hist = compteActif->getHistorique(1);
            if (!hist.isEmpty())
                DataManager::instance().sauvegarderTransaction(compteActif->getIBAN(), hist.first());
            emit soldeModifie(compteActif->getSolde());
            emit operationReussie(QString("Virement externe de %1 € vers %2 effectué.").arg(montant, 0, 'f', 2).arg(ibanDest));
            return true;
        }
    } else {
        if (compteActif->virer(*dest, montant, desc)) {
            DataManager::instance().sauvegarderSolde(compteActif->getIBAN(), compteActif->getSolde());
            DataManager::instance().sauvegarderSolde(dest->getIBAN(), dest->getSolde());
            auto hist = compteActif->getHistorique(1);
            if (!hist.isEmpty())
                DataManager::instance().sauvegarderTransaction(compteActif->getIBAN(), hist.first());
            emit soldeModifie(compteActif->getSolde());
            emit operationReussie(QString("Virement de %1 € effectué.").arg(montant, 0, 'f', 2));
            return true;
        }
    }
    emit operationEchouee("Solde insuffisant ou compte non trouvé.");
    return false;
}

int CompteController::demanderPret(double montant, double taux, int dureeMois) {
    if (!client) return -1;
    QString iban = compteActif ? compteActif->getIBAN() : "";
    Pret p(0, client->getId(), iban, montant, taux, dureeMois);
    int id = DataManager::instance().creerPret(p);
    if (id > 0)
        emit operationReussie("Demande de prêt enregistrée avec succès.");
    else
        emit operationEchouee("Erreur lors de l'enregistrement de la demande.");
    return id;
}

QVector<Pret> CompteController::getPrets() const {
    if (!client) return {};
    return DataManager::instance().chargerPrets(client->getId());
}

bool CompteController::creerNouveauCompte(TypeCompte type, double soldeInitial) {
    if (!client) return false;
    QString iban = Banque::instance().genererIBAN();
    int id = DataManager::instance().creerCompte(client->getId(), iban, type, soldeInitial);
    if (id < 0) { emit operationEchouee("Erreur de création du compte."); return false; }

    CompteBancaire c(id, iban, type, client->getId(), soldeInitial);
    client->ajouterCompte(c);
    emit operationReussie("Nouveau compte créé : " + iban);
    return true;
}

bool CompteController::exporterCSV(const QString& cheminFichier) {
    if (!compteActif) return false;
    return DataManager::instance().exporterCSV(compteActif->getIBAN(), cheminFichier);
}
