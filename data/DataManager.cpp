#include "DataManager.h"
#include "models/Banque.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QRandomGenerator>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>

// ---------------------------------------------------------------------------
// Initialisation

bool DataManager::initialiser(const QString& cheminDB) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(cheminDB);

    if (!db.open()) {
        qWarning() << "Impossible d'ouvrir la base de données:" << db.lastError().text();
        return false;
    }

    initialise = creerTables();
    if (initialise) insererDonneesTest();
    return initialise;
}

bool DataManager::creerTables() {
    QSqlQuery q;

    // Table clients
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS clients (
            id       INTEGER PRIMARY KEY AUTOINCREMENT,
            login    TEXT UNIQUE NOT NULL,
            hash_mdp TEXT NOT NULL,
            prenom   TEXT,
            nom      TEXT,
            email    TEXT,
            admin    INTEGER DEFAULT 0
        )
    )")) {
        qWarning() << "Erreur table clients:" << q.lastError().text();
        return false;
    }

    // Table comptes
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS comptes (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            client_id  INTEGER NOT NULL,
            iban       TEXT UNIQUE NOT NULL,
            type       TEXT NOT NULL,
            solde      REAL DEFAULT 0.0,
            statut     TEXT DEFAULT 'ACTIF',
            FOREIGN KEY(client_id) REFERENCES clients(id)
        )
    )")) {
        qWarning() << "Erreur table comptes:" << q.lastError().text();
        return false;
    }

    // Table transactions
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS transactions (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            iban        TEXT NOT NULL,
            date        TEXT NOT NULL,
            type        TEXT NOT NULL,
            montant     REAL NOT NULL,
            solde_apres REAL NOT NULL,
            description TEXT,
            categorie   TEXT,
            FOREIGN KEY(iban) REFERENCES comptes(iban)
        )
    )")) {
        qWarning() << "Erreur table transactions:" << q.lastError().text();
        return false;
    }

    // Table prêts
    if (!q.exec(R"(
        CREATE TABLE IF NOT EXISTS prets (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            client_id   INTEGER NOT NULL,
            iban        TEXT NOT NULL,
            montant     REAL NOT NULL,
            taux        REAL NOT NULL,
            duree_mois  INTEGER NOT NULL,
            statut      TEXT DEFAULT 'EN_ATTENTE',
            date        TEXT NOT NULL,
            FOREIGN KEY(client_id) REFERENCES clients(id)
        )
    )")) {
        qWarning() << "Erreur table prets:" << q.lastError().text();
        return false;
    }

    return true;
}

void DataManager::insererDonneesTest() {
    // Vérifier si des données existent déjà
    QSqlQuery check;
    check.exec("SELECT COUNT(*) FROM clients");
    if (check.next() && check.value(0).toInt() > 0) return;

    // Comptes de démonstration
    // Mot de passe = "admin123" (SHA-256)
    QString hashAdmin = "240be518fabd2724ddb6f04eeb1da5967448d7e831c08c8fa822809f74c720a9";
    // Mot de passe = "password" (SHA-256)
    QString hashUser  = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8";

    QSqlQuery q;
    // Admin
    q.prepare("INSERT OR IGNORE INTO clients (login, hash_mdp, prenom, nom, email, admin) VALUES (?,?,?,?,?,?)");
    q.addBindValue("admin"); q.addBindValue(hashAdmin);
    q.addBindValue("Pierre"); q.addBindValue("Martin"); q.addBindValue("admin@bankvision.fr"); q.addBindValue(1);
    q.exec();

    int adminId = q.lastInsertId().toInt();

    // Utilisateur normal
    q.prepare("INSERT OR IGNORE INTO clients (login, hash_mdp, prenom, nom, email, admin) VALUES (?,?,?,?,?,?)");
    q.addBindValue("alice"); q.addBindValue(hashUser);
    q.addBindValue("Alice"); q.addBindValue("Dupont"); q.addBindValue("alice@example.com"); q.addBindValue(0);
    q.exec();
    int aliceId = q.lastInsertId().toInt();

    // Comptes de Alice
    q.prepare("INSERT OR IGNORE INTO comptes (client_id, iban, type, solde) VALUES (?,?,?,?)");
    q.addBindValue(aliceId); q.addBindValue("FR7610001000010000000000001"); q.addBindValue("COURANT"); q.addBindValue(4250000.0);
    q.exec();
    q.prepare("INSERT OR IGNORE INTO comptes (client_id, iban, type, solde) VALUES (?,?,?,?)");
    q.addBindValue(aliceId); q.addBindValue("FR7610001000010000000000002"); q.addBindValue("EPARGNE"); q.addBindValue(12500000.0);
    q.exec();

    // Transactions de démonstration pour les stats (12 derniers mois)
    QDateTime now = QDateTime::currentDateTime();
    QStringList catDepenses = {"alimentation", "loisirs", "transport", "logement", "sante"};
    QStringList descDepenses = {"Supermarché", "Cinéma", "Essence", "Loyer", "Pharmacie"};
    double solde = 4250000.0;

    for (int i = 11; i >= 0; --i) {
        // Salaire en F CFA
        double salaire = 500000.0 + (QRandomGenerator::global()->bounded(20000) - 10000);
        QDateTime d = now.addMonths(-i).addDays(-20);
        q.prepare("INSERT INTO transactions (iban, date, type, montant, solde_apres, description, categorie) VALUES (?,?,?,?,?,?,?)");
        q.addBindValue("FR7610001000010000000000001"); q.addBindValue(d.toString(Qt::ISODate));
        q.addBindValue("depot"); q.addBindValue(salaire); q.addBindValue(solde + salaire);
        q.addBindValue("Salaire mensuel"); q.addBindValue("salaire");
        q.exec(); solde += salaire;

        // Quelques dépenses en F CFA
        for (int j = 0; j < 4; ++j) {
            double dep = 15000.0 + QRandomGenerator::global()->bounded(80000);
            int    cat = j % catDepenses.size();
            QDateTime dd = now.addMonths(-i).addDays(-j * 5);
            q.prepare("INSERT INTO transactions (iban, date, type, montant, solde_apres, description, categorie) VALUES (?,?,?,?,?,?,?)");
            q.addBindValue("FR7610001000010000000000001"); q.addBindValue(dd.toString(Qt::ISODate));
            q.addBindValue("retrait"); q.addBindValue(dep); q.addBindValue(solde - dep);
            q.addBindValue(descDepenses[cat]); q.addBindValue(catDepenses[cat]);
            q.exec(); solde -= dep;
        }
    }

    // Mettre à jour le solde réel
    q.prepare("UPDATE comptes SET solde = ? WHERE iban = ?");
    q.addBindValue(solde); q.addBindValue("FR7610001000010000000000001");
    q.exec();
}

// ---------------------------------------------------------------------------
// Clients

bool DataManager::creerClient(const QString& login, const QString& motDePasseHash,
                               const QString& prenom, const QString& nom,
                               const QString& email, bool admin) {
    QSqlQuery q;
    q.prepare("INSERT INTO clients (login, hash_mdp, prenom, nom, email, admin) VALUES (?,?,?,?,?,?)");
    q.addBindValue(login); q.addBindValue(motDePasseHash);
    q.addBindValue(prenom); q.addBindValue(nom); q.addBindValue(email); q.addBindValue(admin ? 1 : 0);
    if (!q.exec()) { qWarning() << q.lastError(); return false; }
    return true;
}

Client DataManager::chargerClient(const QString& login) {
    QSqlQuery q;
    q.prepare("SELECT id, login, prenom, nom, email, admin FROM clients WHERE login = ?");
    q.addBindValue(login);
    q.exec();
    if (!q.next()) return Client();
    Client c(q.value(0).toInt(), q.value(1).toString(),
             q.value(2).toString(), q.value(3).toString(), q.value(4).toString());
    c.setAdmin(q.value(5).toBool());

    // Charger les comptes
    for (auto& compte : chargerComptes(c.getId()))
        c.ajouterCompte(compte);

    return c;
}

bool DataManager::loginExiste(const QString& login) const {
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM clients WHERE login = ?");
    q.addBindValue(login);
    q.exec();
    return q.next() && q.value(0).toInt() > 0;
}

QString DataManager::getHashMotDePasse(const QString& login) const {
    QSqlQuery q;
    q.prepare("SELECT hash_mdp FROM clients WHERE login = ?");
    q.addBindValue(login);
    q.exec();
    if (q.next()) return q.value(0).toString();
    return {};
}

QVector<Client> DataManager::chargerTousClients() {
    QVector<Client> liste;
    QSqlQuery q("SELECT id, login, prenom, nom, email, admin FROM clients");
    while (q.next()) {
        Client c(q.value(0).toInt(), q.value(1).toString(),
                 q.value(2).toString(), q.value(3).toString(), q.value(4).toString());
        c.setAdmin(q.value(5).toBool());
        for (auto& compte : chargerComptes(c.getId()))
            c.ajouterCompte(compte);
        liste.append(c);
    }
    return liste;
}

// ---------------------------------------------------------------------------
// Comptes

int DataManager::creerCompte(int clientId, const QString& iban,
                              TypeCompte type, double soldeInitial) {
    QString typeStr;
    switch (type) {
        case TypeCompte::COURANT:       typeStr = "COURANT"; break;
        case TypeCompte::EPARGNE:       typeStr = "EPARGNE"; break;
        case TypeCompte::PROFESSIONNEL: typeStr = "PROFESSIONNEL"; break;
    }
    QSqlQuery q;
    q.prepare("INSERT INTO comptes (client_id, iban, type, solde) VALUES (?,?,?,?)");
    q.addBindValue(clientId); q.addBindValue(iban);
    q.addBindValue(typeStr); q.addBindValue(soldeInitial);
    if (!q.exec()) { qWarning() << q.lastError(); return -1; }
    return q.lastInsertId().toInt();
}

bool DataManager::sauvegarderSolde(const QString& iban, double solde) {
    QSqlQuery q;
    q.prepare("UPDATE comptes SET solde = ? WHERE iban = ?");
    q.addBindValue(solde); q.addBindValue(iban);
    return q.exec();
}

QVector<CompteBancaire> DataManager::chargerComptes(int clientId) {
    QVector<CompteBancaire> liste;
    QSqlQuery q;
    q.prepare("SELECT id, iban, type, solde, statut FROM comptes WHERE client_id = ?");
    q.addBindValue(clientId);
    q.exec();
    while (q.next()) {
        int    id    = q.value(0).toInt();
        QString iban  = q.value(1).toString();
        QString typeS = q.value(2).toString();
        double solde  = q.value(3).toDouble();

        TypeCompte type = TypeCompte::COURANT;
        if (typeS == "EPARGNE")        type = TypeCompte::EPARGNE;
        if (typeS == "PROFESSIONNEL")  type = TypeCompte::PROFESSIONNEL;

        CompteBancaire c(id, iban, type, clientId, solde);

        // Charger les transactions
        for (auto& t : chargerTransactions(iban, 100))
            c.ajouterTransaction(t);

        liste.append(c);
    }
    return liste;
}

// ---------------------------------------------------------------------------
// Transactions

bool DataManager::sauvegarderTransaction(const QString& iban, const Transaction& t) {
    QSqlQuery q;
    q.prepare("INSERT INTO transactions (iban, date, type, montant, solde_apres, description, categorie) VALUES (?,?,?,?,?,?,?)");
    q.addBindValue(iban);
    q.addBindValue(t.date.toString(Qt::ISODate));
    q.addBindValue(t.type);
    q.addBindValue(t.montant);
    q.addBindValue(t.soldeApres);
    q.addBindValue(t.description);
    q.addBindValue(t.categorie);
    if (!q.exec()) { qWarning() << q.lastError(); return false; }
    return true;
}

QVector<Transaction> DataManager::chargerTransactions(const QString& iban, int limite) {
    QVector<Transaction> liste;
    QSqlQuery q;
    q.prepare("SELECT id, date, type, montant, solde_apres, description, categorie "
              "FROM transactions WHERE iban = ? ORDER BY date DESC LIMIT ?");
    q.addBindValue(iban); q.addBindValue(limite);
    q.exec();
    while (q.next()) {
        Transaction t;
        t.id          = q.value(0).toInt();
        t.date        = QDateTime::fromString(q.value(1).toString(), Qt::ISODate);
        t.type        = q.value(2).toString();
        t.montant     = q.value(3).toDouble();
        t.soldeApres  = q.value(4).toDouble();
        t.description = q.value(5).toString();
        t.categorie   = q.value(6).toString();
        liste.append(t);
    }
    return liste;
}

// ---------------------------------------------------------------------------
// Prêts

int DataManager::creerPret(const Pret& p) {
    QSqlQuery q;
    q.prepare("INSERT INTO prets (client_id, iban, montant, taux, duree_mois, statut, date) VALUES (?,?,?,?,?,?,?)");
    q.addBindValue(p.getClientId()); q.addBindValue(p.getIban());
    q.addBindValue(p.getMontant()); q.addBindValue(p.getTaux());
    q.addBindValue(p.getDuree()); q.addBindValue("EN_ATTENTE");
    q.addBindValue(p.getDate().toString(Qt::ISODate));
    if (!q.exec()) { qWarning() << q.lastError(); return -1; }
    return q.lastInsertId().toInt();
}

bool DataManager::mettreAJourStatutPret(int pretId, StatutPret statut) {
    QString s;
    switch (statut) {
        case StatutPret::EN_ATTENTE: s = "EN_ATTENTE"; break;
        case StatutPret::APPROUVE:   s = "APPROUVE"; break;
        case StatutPret::REFUSE:     s = "REFUSE"; break;
        case StatutPret::REMBOURSE:  s = "REMBOURSE"; break;
    }
    QSqlQuery q;
    q.prepare("UPDATE prets SET statut = ? WHERE id = ?");
    q.addBindValue(s); q.addBindValue(pretId);
    return q.exec();
}

QVector<Pret> DataManager::chargerPrets(int clientId) {
    QVector<Pret> liste;
    QSqlQuery q;
    if (clientId >= 0) {
        q.prepare("SELECT id, client_id, iban, montant, taux, duree_mois, statut, date FROM prets WHERE client_id = ?");
        q.addBindValue(clientId);
    } else {
        q.prepare("SELECT id, client_id, iban, montant, taux, duree_mois, statut, date FROM prets");
    }
    q.exec();
    while (q.next()) {
        Pret p(q.value(0).toInt(), q.value(1).toInt(), q.value(2).toString(),
               q.value(3).toDouble(), q.value(4).toDouble(), q.value(5).toInt());
        QString s = q.value(6).toString();
        if      (s == "APPROUVE")  p.setStatut(StatutPret::APPROUVE);
        else if (s == "REFUSE")    p.setStatut(StatutPret::REFUSE);
        else if (s == "REMBOURSE") p.setStatut(StatutPret::REMBOURSE);
        liste.append(p);
    }
    return liste;
}

// ---------------------------------------------------------------------------
// Export CSV

bool DataManager::exporterCSV(const QString& iban, const QString& cheminFichier) {
    QFile f(cheminFichier);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

    QTextStream ts(&f);
    ts << "Date;Type;Montant;Solde Après;Description;Catégorie\n";

    auto transactions = chargerTransactions(iban, 10000);
    for (const auto& t : transactions) {
        ts << t.date.toString("dd/MM/yyyy HH:mm") << ";"
           << t.type << ";"
           << QString::number(t.montant, 'f', 2) << ";"
           << QString::number(t.soldeApres, 'f', 2) << ";"
           << t.description << ";"
           << t.categorie << "\n";
    }
    return true;
}
