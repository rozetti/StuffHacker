#pragma once

#include "httprequesthandler.h"

class CRZSettings;

class RequestMapper : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)

public:
    RequestMapper(QObject* parent, CRZSettings &settings);

    void service(HttpRequest& request, HttpResponse& response);

private:
    CRZSettings &m_settings;
};
