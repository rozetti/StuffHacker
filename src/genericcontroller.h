#pragma once

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"

#include "crzsettings.h"

class QNetworkReply;
class QJsonDocument;

class GenericController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(GenericController)

public:
    GenericController(CRZSettings &settings, QObject *parent);

    void service(HttpRequest& request, HttpResponse& response);

protected:
    virtual void processRawResponse(const QString &/*url*/, QByteArray &/*bytes*/) {}
    virtual void processJsonResponse(const QString &/*url*/, QJsonDocument &/*json*/) {}

private:
    void relay(QString const &url, HttpRequest &request, HttpResponse &response);

    void log(const std::string &message) const;
    QString buildRequestedUrl(HttpRequest &request);
    void shipCannedResponse(const QString &filename, HttpResponse &response);

private:
    QNetworkReply *m_reply;
    QString fetch(const QString &url);

    QByteArray process(QString const &url, QByteArray &bytes);

    CRZSettings &m_settings;
};
