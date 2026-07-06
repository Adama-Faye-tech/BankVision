#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QSpinBox>
#include "controllers/CompteController.h"

/**
 * @brief Page des opérations bancaires : dépôt, retrait, virement, prêt.
 */
class FenetreOperations : public QWidget {
    Q_OBJECT
public:
    explicit FenetreOperations(CompteController* ctrl, QWidget* parent = nullptr);

private slots:
    void effectuerDepot();
    void effectuerRetrait();
    void effectuerVirement();
    void demanderPret();
    void simulerPret();
    void afficherResultat(const QString& msg, bool succes);
    void onCompteChange(int index);
    void rafraichirSolde(double solde);

private:
    CompteController* ctrl = nullptr;

    // Sélecteur de compte en haut
    QComboBox*  cmbComptes   = nullptr;
    QLabel*     lblSoldeActif = nullptr;

    // Dépôt
    QDoubleSpinBox* sbDepot      = nullptr;
    QComboBox*      cmbCatDepot  = nullptr;
    QLineEdit*      leDescDepot  = nullptr;

    // Retrait
    QDoubleSpinBox* sbRetrait    = nullptr;
    QComboBox*      cmbCatRetrait = nullptr;
    QLineEdit*      leDescRetrait = nullptr;

    // Virement
    QLineEdit*      leIbanVirement = nullptr;
    QDoubleSpinBox* sbVirement     = nullptr;
    QLineEdit*      leDescVirement = nullptr;

    // Prêt
    QDoubleSpinBox* sbMontantPret  = nullptr;
    QDoubleSpinBox* sbTauxPret     = nullptr;
    QSpinBox*       sbDureePret    = nullptr;
    QLabel*         lblSimul       = nullptr;

    // Résultat
    QLabel* lblResultat = nullptr;

    QWidget* creerOngletDepot();
    QWidget* creerOngletRetrait();
    QWidget* creerOngletVirement();
    QWidget* creerOngletPret();
    QWidget* creerEnTeteCompte();
};

