#pragma once
#include <QWidget>
#include <QMap>
#include <QString>
#include <QVector>
#include <QColor>

/**
 * @brief Widget QPainter affichant un camembert des dépenses par catégorie.
 */
class CamembertDepenses : public QWidget {
    Q_OBJECT
public:
    explicit CamembertDepenses(QWidget* parent = nullptr);
    void setDonnees(const QMap<QString, double>& categories);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    QMap<QString, double>  donnees;
    QVector<QColor>        couleurs;
};
