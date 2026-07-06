#include <QApplication>
#include <QFile>
#include "views/FenetreConnexion.h"
#include "data/DataManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    
    QFile styleFile(":styles/style.css");
    if (styleFile.open(QIODevice::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    DataManager::instance().initialiser("data/banque.db");

    FenetreConnexion connexion;
    connexion.show();

    return app.exec();
}
