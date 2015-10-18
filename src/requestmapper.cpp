#include "requestmapper.h"
#include "genericcontroller.h"

RequestMapper::RequestMapper(QObject* parent, CRZSettings &settings) :
    HttpRequestHandler(parent),
    m_settings(settings)
{
    auto endpoint = m_settings.value("endpoint").toString();

    qDebug() << "endpoint: " << endpoint;
}

void RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
    (new GenericController(m_settings, 0))->service(request, response);
}
