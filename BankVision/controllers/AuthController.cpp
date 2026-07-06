#include "AuthController.h"
#include "data/DataManager.h"
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QVariant>

AuthController::AuthController() {}

QString AuthController::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool AuthController::login(const QString& username, const QString& password) {
    QSqlQuery query(DataManager::instance().database());

    query.prepare("SELECT password FROM users WHERE username = ?");
    query.addBindValue(username);

    if(query.exec() && query.next()) {
        QString stored = query.value(0).toString();
        return stored == hashPassword(password);
    }

    return false;
}

bool AuthController::registerUser(const QString& username, const QString& password) {
    QSqlQuery query(DataManager::instance().database());

    query.prepare("INSERT INTO users(username, password) VALUES(?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));

    return query.exec();
}