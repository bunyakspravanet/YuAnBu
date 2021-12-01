#pragma once
#include <QObject>
#include <QDebug>
//#include <QBson.h>
//#include <bson.h>
//#include <encoding.h>
//#include <BsonProtocolEngine.h>

#include <../Rosco/BuildDependencies/amq/activemq/transport/tcp/SslTransport.h>
#include <../Rosco/BuildDependencies/amq/activemq/library/ActiveMQCPP.h>
#include <../Rosco/BuildDependencies/amq/decaf/lang/Thread.h>
#include <../Rosco/BuildDependencies/amq/decaf/lang/Runnable.h>
#include <../Rosco/BuildDependencies/amq/decaf/util/concurrent/CountDownLatch.h>
#include <../Rosco/BuildDependencies/amq/decaf/lang/Integer.h>
#include <../Rosco/BuildDependencies/amq/decaf/lang/Long.h>
#include <../Rosco/BuildDependencies/amq/decaf/lang/System.h>
#include <../Rosco/BuildDependencies/amq/activemq/core/ActiveMQConnectionFactory.h>
#include <../Rosco/BuildDependencies/amq/activemq/util/Config.h>
#include <../Rosco/BuildDependencies/amq/cms/Connection.h>
#include <../Rosco/BuildDependencies/amq/cms/Session.h>
#include <../Rosco/BuildDependencies/amq/cms/TextMessage.h>
#include <../Rosco/BuildDependencies/amq/cms/BytesMessage.h>
#include <../Rosco/BuildDependencies/amq/cms/MapMessage.h>
#include <../Rosco/BuildDependencies/amq/cms/ExceptionListener.h>
#include <../Rosco/BuildDependencies/amq/cms/MessageListener.h>
#include <../Rosco/BuildDependencies/amq/activemq/core/ActiveMQConnection.h>
#include <../Rosco/BuildDependencies/amq/activemq/transport/DefaultTransportListener.h>
#include <activemq/transport/TransportListener.h>
#include <../Rosco/BuildDependencies/amq/decaf/util/Date.h>

#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

////////////////////////////////////////////////////////////////////////////////
class cAMQProducer : public QObject, public Runnable
{
    Q_OBJECT

private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageProducer* producer;
    bool useTopic;
    bool clientAck;
    std::string brokerURI;
    std::string destURI;

private:
    cAMQProducer( const cAMQProducer& );
    cAMQProducer& operator= ( const cAMQProducer& );

public:
    cAMQProducer( const std::string& brokerURI, const std::string& destURI, bool useTopic = false, bool clientAck = false ) :
        connection(NULL),
        session(NULL),
        destination(NULL),
        producer(NULL),
        useTopic(useTopic),
        clientAck(clientAck),
        brokerURI(brokerURI),
        destURI(destURI)
    { }

    virtual ~cAMQProducer()
    {
        cleanup();
    }

    bool sessionTransacted = false;
    std::string SendMessage = "";
    int ProducerReady = 0;
    string threadIdStr;

    void close()
    {
        this->cleanup();
    }

public slots:
    void onMessage(string SendMessage, string threadIdStr);

signals:
    void MessToSendSig(string SendMessage, string threadIdStr);
    void SendErrorSig();

private:
    virtual void run();
    void cleanup();

};

////////////////////////////////////////////////////////////////////////////////
class cAMQConsumer : public QObject,
                     public ExceptionListener,
                     public MessageListener,
                     public Runnable

{
    Q_OBJECT
    //Q_PROPERTY(Priority priority READ priority WRITE setPriority)
    //Q_ENUMS(Priority)

private:
    Connection* connection;
    Session* session;
    Destination* destination;
    MessageConsumer* consumer;
    bool useTopic;
    std::string brokerURI;
    std::string destURI;
    bool clientAck;

private:
    cAMQConsumer(const cAMQConsumer&);
    cAMQConsumer& operator=(const cAMQConsumer&);

public:
    cAMQConsumer(const std::string& brokerURI,
                 const std::string& destURI,
                 bool useTopic = false,
                 bool clientAck = false) :
        connection(NULL),
        session(NULL),
        destination(NULL),
        consumer(NULL),
        useTopic(useTopic),
        brokerURI(brokerURI),
        destURI(destURI),
        clientAck(clientAck)
    { }

    virtual ~cAMQConsumer()
    {
        this->cleanup();
    }

    void close()
    {
        this->cleanup();
    }

    int ConsumerReady = 0;
    bool sessionTransacted = false;

    string ReceiveText = "";

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.

    virtual void onException(const CMSException& ex AMQCPP_UNUSED);

    virtual void onException(const decaf::lang::Exception& ex);

    virtual void transportInterrupted();

    virtual void transportResumed();

    // Called from the consumer since this class is a registered MessageListener.

    void onMessage(const Message* message);


signals:
    void ReceivErrorSig();

private:
    virtual void run();
    void cleanup();

};



