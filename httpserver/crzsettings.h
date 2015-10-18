#pragma once
#include <QSettings>

class CRZSettings
{
public:
    CRZSettings(QSettings &settings, QString const &section);

    QString const &section() const { return m_section; }

    QVariant value(QString const &key);
    QVariant value(QString const &key, int d);
    QVariant value(QString const &key, QString const &d);

    bool boolValue(const QString &key, const bool &d);
private:
    QSettings &m_settings;
    QString const &m_section;
};

