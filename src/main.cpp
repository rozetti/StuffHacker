#include <QCoreApplication>
#include <QDir>
#include "httplistener.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "requestmapper.h"

HttpSessionStore* sessionStore;
StaticFileController* staticFileController;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto settings = new QSettings("StuffHacker.ini", QSettings::IniFormat);

    // todo crz: unlimited listeners
    auto section1 = QString("listener");
    CRZSettings s1(*settings, section1);

    // todo crz: make enabled response to inni changes
    if (s1.value("enabled", "yes").toString() == "yes")
    {
        new HttpListener(s1, new RequestMapper(&app, s1), &app);
    }

    app.exec();
}
