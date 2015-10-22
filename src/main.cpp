#include <QCoreApplication>
#include <QDir>
#include "httplistener.h"
#include "httpsessionstore.h"
#include "staticfilecontroller.h"
#include "requestmapper.h"

#include <fstream>

HttpSessionStore* sessionStore;
StaticFileController* staticFileController;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto settings = new QSettings("StuffHacker.ini", QSettings::IniFormat);

    // todo crz: unlimited listeners
    auto section1 = QString("listener");
    CRZSettings s1(*settings, section1);

    // todo crz: make enabled response to ini changes
    if (s1.value("enabled", "yes").toString() == "yes")
    {
        auto clear_log = s1.boolValue("clearLog", false);
        if (clear_log)
        {
            std::ofstream f;
            auto d = section1 + ".log";
            auto filename = s1.value("logFile", d).toString();
            f.open(filename.toStdString(), std::ios::trunc);
        }

        new HttpListener(s1, new RequestMapper(&app, s1), &app);
    }

    app.exec();
}
