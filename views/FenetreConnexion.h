#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>

/**
 * @brief Écran de connexion avec champs login/mot de passe, animation et verrouillage.
 */
class FenetreConnexion : public QWidget {
    Q_OBJECT
public:
    explicit FenetreConnexion(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private slots:
    void tenterConnexion();
    void onConnexionReussie();
    void onConnexionEchouee(const QString& raison);
    void mettreAJourVerrouillage();

private:
    // UI
    QLineEdit*   champLogin    = nullptr;
    QLineEdit*   champMdp      = nullptr;
    QPushButton* btnConnecter  = nullptr;
    QLabel*      lblMessage    = nullptr;
    QLabel*      lblLogo       = nullptr;
    QLabel*      lblTitre      = nullptr;
    QLabel*      lblSousTitre  = nullptr;
    QLabel*      lblVerrou     = nullptr;
    QWidget*     carte         = nullptr;

    QTimer*              timerVerrou = nullptr;
    QPropertyAnimation*  animCarte   = nullptr;

    void construireUI();
    void animerEntree();
    void animerEchec();
    void ouvrirTableauBord();
};
