#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include "views/FenetreConnexion.h"
#include "data/DataManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Charger la feuille de style
    QFile styleFile(":/styles/style.qss");
    if (styleFile.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    // Initialiser la base de données
    DataManager::instance().initialiser("banque.db");

    // Définir l'icône globale de l'application
    app.setWindowIcon(QIcon(":/images/icon.jpg"));

    // Afficher la fenêtre de connexion
    FenetreConnexion connexion;
    connexion.show();

    return app.exec();
}
