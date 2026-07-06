#pragma once

#include <QSqlDatabase>
#include <QString>

class DataManager {
private:
    QSqlDatabase db;

    DataManager();
    bool preparerFichierDatabase(const QString& dbPath);
    bool creerTables();
    bool creerUtilisateurDefaut();

public:
    static DataManager& instance();

    bool initialiser(const QString& cheminDB);
    QSqlDatabase& database();
};
