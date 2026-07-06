#pragma once

#include <QString>

class AuthController {
public:
    AuthController();
    QString hashPassword(const QString& password);
    bool login(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password);
};
