#include "genericcontroller.h"
#include <QVariant>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>
#include <QThread>
#include <QEventLoop>
#include <QMapIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QRegularExpression>

#include <sstream>
#include <iostream>
#include <fstream>

void GenericController::log(std::string const &message) const
{
    auto to_screen = m_settings.boolValue("outputToScreen", "yes");
    auto to_log = m_settings.boolValue("outputToLog", "yes");

    if (to_screen)
    {
        std::cout << message;
    }

    if (to_log)
    {
        auto d = m_settings.section() + ".log";
        auto filename = m_settings.value("logFile", d).toString();

        std::ofstream f;
        f.open(filename.toStdString(), std::ios::app);
        f << message;
    }
}

GenericController::GenericController(CRZSettings &settings, QObject *parent) :
    HttpRequestHandler(parent),
    m_settings(settings)
{
}

QByteArray GenericController::process(QString const &url, QByteArray &bytes)
{
    auto replace = m_settings.value("responseReplace").toString();
    if (!replace.isEmpty())
    {
        auto ss = replace.split(':');
        if (ss.count() == 2)
        {
            auto s = QString(bytes);

            QRegularExpression re(ss[0]);
            s.replace(re, ss[1]);

            bytes = s.toUtf8();
        }
    }

    processRawResponse(url, bytes);

    auto json = QJsonDocument::fromJson(bytes);

    processJsonResponse(url, json);

    return json.toJson();
}

QString GenericController::fetch(QString const &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest replyRequest(url);

    QEventLoop el;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));

    auto reply = manager.get(replyRequest);
    el.exec(QEventLoop::ExcludeUserInputEvents);

    auto bytes = reply->readAll();
    reply->deleteLater();

    return QString(bytes);
}

QString GenericController::buildRequestedUrl(HttpRequest &request)
{
    auto parms = request.getParameterMap();
    auto path = request.getPath();

    auto s = path;

    auto first = true;

    QMap<QString, QString> parm_map;

    QMapIterator<QByteArray, QByteArray> it(parms);
    while (it.hasNext())
    {
        it.next();

        parm_map[QString(it.key())] = QString(it.value());

        s += first ? (first = false, "?") : "%";
        s += it.key() + "=" + it.value();
    }

    return s;
}

void GenericController::relay(QString const &url, HttpRequest& /*request*/, HttpResponse& response)
{
    auto endpoint = m_settings.value("endpoint").toString();

    QString s = endpoint;
    if (!url.isEmpty())
    {
        s = endpoint + url;
    }

    std::stringstream ss;
    ss << "\nrelaying request: " << QString(s).toStdString();
    log(ss.str());

    auto replace = m_settings.value("urlReplace").toString();
    if (!replace.isEmpty())
    {
        auto ss = replace.split(':');
        if (ss.count() == 2)
        {
            QRegularExpression re(ss[0]);
            s.replace(re, ss[1]);

            std::stringstream ss;
            ss << "\nmodified request: " << QString(s).toStdString();
            log(ss.str());
        }
    }

    QNetworkAccessManager manager;

    auto qurl = QUrl(s);
    QNetworkRequest replyRequest(qurl);

    QEventLoop el;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));

    auto reply = manager.get(replyRequest);
    el.exec(QEventLoop::ExcludeUserInputEvents);

    if (reply->error() != QNetworkReply::NoError)
    {
        // hack crz
        std::stringstream ss;
        ss << std::endl << "error: " << reply->errorString().toStdString()
                  << "(" << reply->error() << ")" << std::endl;
        log(ss.str());

        response.setStatus(500, reply->errorString().toUtf8());
        response.write("", true);
    }
    else
    {
        auto bytes = reply->readAll();
        reply->deleteLater();

        bytes = process(s, bytes);

        std::stringstream ss;
        ss << std::endl << QString(bytes).toStdString() << std::endl;
        log(ss.str());

        std::stringstream ss2;
        ss2 << std::endl << QString::number(bytes.length()).toStdString() << " bytes received" << std::endl;
        log(ss2.str());

        response.write(bytes, true);
    }
}

void GenericController::shipCannedResponse(QString const &filename, HttpResponse& response)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly))
    {
        std::stringstream ss;
        ss << std::endl << "could not open canned response file: " << filename.toStdString() << std::endl;
        log(ss.str());

        return;
    }

    auto bytes = f.readAll();

    response.write(bytes);

    std::stringstream ss;
    ss << std::endl << QString(bytes).toStdString() << std::endl;
    log(ss.str());

    std::stringstream ss2;
    ss2 << std::endl << QString::number(bytes.length()).toStdString() << " bytes shipped" << std::endl;
    log(ss2.str());

}

void GenericController::service(HttpRequest& request, HttpResponse& response)
{
    auto s = buildRequestedUrl(request);
    auto canned = m_settings.value("cannedResponse").toString();
    auto cc = canned.split(':');
    if (cc[0] == s)
    {
        auto response_filename = cc[1];

        std::stringstream ss;
        ss << std::endl << "canned response " << response_filename.toStdString() << std::endl;
        log(ss.str());

        // todo crz: process the canned response

        shipCannedResponse(response_filename, response);
    }
    else
    {
        relay(s, request, response);
    }
}
