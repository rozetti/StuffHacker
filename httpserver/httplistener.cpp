/**
  @file
  @author Stefan Frings
*/

#include "httplistener.h"
#include "httpconnectionhandler.h"
#include "httpconnectionhandlerpool.h"
#include <QCoreApplication>

#include "crzsettings.h"

#undef SUPERVERBOSE

HttpListener::HttpListener(CRZSettings &settings, HttpRequestHandler* requestHandler, QObject *parent)
    : QTcpServer(parent),
        m_settings(settings)
{
    //Q_ASSERT(m_settings!=0);
    Q_ASSERT(requestHandler!=0);
    pool=NULL;
    this->requestHandler=requestHandler;
    // Reqister type of socketDescriptor for signal/slot handling
    qRegisterMetaType<tSocketDescriptor>("tSocketDescriptor");
    // Start listening
    listen();
}


HttpListener::~HttpListener() {
    close();
    qDebug("HttpListener: destroyed");
}


void HttpListener::listen() {
    if (!pool)
    {
        pool=new HttpConnectionHandlerPool(m_settings, requestHandler);
    }

    auto port = m_settings.value("port").toInt();
    auto host = m_settings.value("host").toString();

    auto host_address = host.isEmpty() ? QHostAddress::Any : QHostAddress(host);
    QTcpServer::listen(host_address, port);
    if (!isListening())
    {
        qCritical("HttpListener: Cannot bind on port %i: %s", port, qPrintable(errorString()));
    }
    else
    {
        qDebug("HttpListener: Listening on port %i", port);
    }
}


void HttpListener::close() {
    QTcpServer::close();
    qDebug("HttpListener: closed");
    if (pool) {
        delete pool;
        pool=NULL;
    }
}

void HttpListener::incomingConnection(tSocketDescriptor socketDescriptor) {
#ifdef SUPERVERBOSE
    qDebug("HttpListener: New connection");
#endif

    HttpConnectionHandler* freeHandler=NULL;
    if (pool) {
        freeHandler=pool->getConnectionHandler();
    }

    // Let the handler process the new connection.
    if (freeHandler) {
        // The descriptor is passed via signal/slot because the handler lives in another
        // thread and cannot open the socket when directly called by another thread.
        connect(this,SIGNAL(handleConnection(tSocketDescriptor)),freeHandler,SLOT(handleConnection(tSocketDescriptor)));
        emit handleConnection(socketDescriptor);
        disconnect(this,SIGNAL(handleConnection(tSocketDescriptor)),freeHandler,SLOT(handleConnection(tSocketDescriptor)));
    }
    else {
        // Reject the connection
        qDebug("HttpListener: Too many incoming connections");
        QTcpSocket* socket=new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
        socket->write("HTTP/1.1 503 too many connections\r\nConnection: close\r\n\r\nToo many connections\r\n");
        socket->disconnectFromHost();
    }
}
