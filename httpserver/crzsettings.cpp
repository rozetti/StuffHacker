#include "crzsettings.h"

#include <QSettings>

CRZSettings::CRZSettings(QSettings &settings, QString const &section) :
    m_settings(settings),
    m_section(section)
{

}

QVariant CRZSettings::value(const QString &key)
{
    m_settings.beginGroup(m_section);
    auto v = m_settings.value(key);
    m_settings.endGroup();

    return v;
}

QVariant CRZSettings::value(const QString &key, int d)
{
    m_settings.beginGroup(m_section);
    auto v = m_settings.value(key, d);
    m_settings.endGroup();

    return v;
}

QVariant CRZSettings::value(const QString &key, QString const &d)
{
    m_settings.sync();
    m_settings.beginGroup(m_section);
    auto v = m_settings.value(key, d);
    m_settings.endGroup();

    return v;
}

bool CRZSettings::boolValue(const QString &key, bool const &d)
{
    m_settings.sync();
    m_settings.beginGroup(m_section);
    auto v = m_settings.value(key, d ? "yes" : "no").toString().toLower();
    m_settings.endGroup();

    if ("no" == v) return false;
    if ("false" == v) return false;

    return true;
}
