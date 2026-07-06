#include "DataManager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QDebug>

DataManager::DataManager() {}

DataManager& DataManager::instance() {
    static DataManager instance;
    return instance;
}

bool DataManager::initialiser(const QString& cheminDB) {
    QFileInfo dbFile(cheminDB);
    QString dbPath = dbFile.isAbsolute()
        ? dbFile.absoluteFilePath()
        : QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(cheminDB);

    if (!preparerFichierDatabase(dbPath)) {
        return false;
    }

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Erreur DB:" << db.lastError().text();
        return false;
    }

    if (!creerTables() || !creerUtilisateurDefaut()) {
        return false;
    }

    qDebug() << "Base de donnees connectee";
    return true;
}

QSqlDatabase& DataManager::database() {
    return db;
}

bool DataManager::preparerFichierDatabase(const QString& dbPath) {
    QDir().mkpath(QFileInfo(dbPath).absolutePath());

    QFile file(dbPath);
    if (!file.exists() || file.size() == 0) {
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Erreur DB:" << "impossible de lire" << dbPath;
        return false;
    }

    const QByteArray header = file.read(16);
    file.close();

    if (header == QByteArray("SQLite format 3", 15) + '\0') {
        return true;
    }

    const QString backupPath = dbPath + ".invalid";
    QFile::remove(backupPath);
    if (!QFile::rename(dbPath, backupPath)) {
        qDebug() << "Erreur DB:" << "fichier invalide impossible a remplacer" << dbPath;
        return false;
    }

    qDebug() << "Ancien fichier DB invalide deplace vers" << backupPath;
    return true;
}

bool DataManager::creerTables() {
    QSqlQuery query(db);
    const QStringList statements = {
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE,"
        "password TEXT"
        ")",
        "CREATE TABLE IF NOT EXISTS comptes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "iban TEXT,"
        "solde REAL,"
        "type TEXT"
        ")",
        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "compte_id INTEGER,"
        "type TEXT,"
        "montant REAL,"
        "date TEXT,"
        "FOREIGN KEY(compte_id) REFERENCES comptes(id)"
        ")",
        "CREATE TABLE IF NOT EXISTS prets ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "compte_id INTEGER,"
        "montant REAL,"
        "taux REAL,"
        "date TEXT"
        ")"
    };

    for (const QString& statement : statements) {
        if (!query.exec(statement)) {
            qDebug() << "Erreur creation table:" << query.lastError().text();
            return false;
        }
    }

    return true;
}

bool DataManager::creerUtilisateurDefaut() {
    QSqlQuery query(db);
    const QString hash = QString(QCryptographicHash::hash("admin", QCryptographicHash::Sha256).toHex());

    query.prepare("INSERT OR IGNORE INTO users(username, password) VALUES(?, ?)");
    query.addBindValue("admin");
    query.addBindValue(hash);

    if (!query.exec()) {
        qDebug() << "Erreur creation utilisateur:" << query.lastError().text();
        return false;
    }

    return true;
}
