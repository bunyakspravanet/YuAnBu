#include "cppAMQ.h"

void cAMQProducer::onMessage(string SendMessage, string threadIdStr)
{
     if (SendMessage.size() > 0 && ProducerReady > 0)
     {
        SendMessage += threadIdStr;

        // message send
        TextMessage* message = session->createTextMessage( SendMessage );

//            message->setIntProperty( "Integer", i );

        // Tell the producer to send the message
        try
        {
            producer->send(message);

            cout << "\nSent message- " << SendMessage.c_str() << endl;
            SendMessage = "";
            delete message;
        }
        catch ( CMSException& e )
        {
            e.printStackTrace();
        }
     }
}

void cAMQProducer::run()
{
    try
    {
        connect(this, &cAMQProducer::MessToSendSig, this, &cAMQProducer::onMessage, Qt::DirectConnection);

        // Create a ConnectionFactory
        auto_ptr<ActiveMQConnectionFactory> connectionFactory(
            new ActiveMQConnectionFactory( brokerURI ) );

        // Create a Connection
        try
        {
            connection = connectionFactory->createConnection();
            connection->start();
        }
        catch( CMSException& e )
        {
            e.printStackTrace();
            throw e;
        }

        // Create a Session
/*
        if( clientAck )
        {
            session = connection->createSession( Session::CLIENT_ACKNOWLEDGE );
        }
        else
        {
            session = connection->createSession( Session::AUTO_ACKNOWLEDGE );
        }
*/
        if (this->sessionTransacted)
        {
            session = connection->createSession(Session::SESSION_TRANSACTED);
        }
        else
        {
            session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        // Create the destination (Topic or Queue)
        if( useTopic )
        {
            destination = session->createTopic( destURI );
        }
        else
        {
            destination = session->createQueue( destURI );
        }

        // Create a MessageProducer from the Session to the Topic or Queue
        producer = session->createProducer( destination );
        producer->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

        ProducerReady = 1;

// Create the Thread Id String
        threadIdStr = Long::toString( Thread::currentThread()->getId() );
     }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
}

void cAMQProducer::cleanup()
{
    // Destroy resources.
    try
    {
        if( destination != NULL ) delete destination;
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
    destination = NULL;

    try
    {
        if( producer != NULL ) delete producer;
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
    producer = NULL;

    // Close open resources.
    try
    {
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }

    try
    {
        if( session != NULL ) delete session;
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
    session = NULL;

    try
    {
        if( connection != NULL ) delete connection;
    }
    catch ( CMSException& e )
    {
        e.printStackTrace();
    }
    connection = NULL;
}
///////////////////////////////////////////////////////////////// Consumer ////////////////////////////////////////////
void cAMQConsumer::run()
{
    try
    {
        // Create a ConnectionFactory
/*        ActiveMQConnectionFactory* connectionFactory = new ActiveMQConnectionFactory(brokerURI);

        // Create a Connection
        connection = connectionFactory->createConnection();
        delete connectionFactory;

        ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection);

        if (amqConnection != NULL)
        {
            amqConnection->addTransportListener(this);
//void addTransportListener(transport::TransportListener* transportListener);
        }

        connection->start();

        connection->setExceptionListener(this);
*/
        auto_ptr<ConnectionFactory> connectionFactory(
            ConnectionFactory::createCMSConnectionFactory(brokerURI));

        // Create a Connection
        connection = connectionFactory->createConnection();
        connection->start();
        connection->setExceptionListener(this);

        // Create a Session
/*
        if (clientAck)
        {
            session = connection->createSession(Session::CLIENT_ACKNOWLEDGE);
        }
        else
        {
            session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }
*/
        if (this->sessionTransacted == true)
        {
            session = connection->createSession(Session::SESSION_TRANSACTED);
        }
        else
        {
            session = connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        // Create the destination (Topic or Queue)
        if (useTopic)
        {
            destination = session->createTopic(destURI);
        }
        else
        {
            destination = session->createQueue(destURI);
        }

        // Create a MessageConsumer from the Session to the Topic or Queue
        consumer = session->createConsumer(destination);
        consumer->setMessageListener(this);

        TextMessage* message = session->createTextMessage( ReceiveText );
        ConsumerReady = 1;
        cout << "\nConsumer is ready to receive\n";

        onMessage(message);
        if (message->getText().size() > 0)
        {
            cout << "is received the message " << message->getText().c_str() << endl;
        }
        delete message;
    }
    catch (CMSException& e)
    {
        e.printStackTrace();
    }
}

void cAMQConsumer::onMessage(const Message* message)
{
    try
    {
        const TextMessage* textMessage = dynamic_cast<const TextMessage*>(message);
//        string text = "";

        if (textMessage != NULL && ReceiveText.size() == 0)
        {
            ReceiveText = textMessage->getText();
        }
        else
        {
            ReceiveText = "NOT A TEXTMESSAGE!";
        }

        if (clientAck)
        {
            message->acknowledge();
        }

//        if (text.size() > 0) cout << "Message " << count << " Received: " << text.c_str() << endl;
    }
    catch (CMSException& e)
    {
        e.printStackTrace();
    }
    // Commit all messages.
    if (this->sessionTransacted)
    {
        session->commit();
    }
}

void cAMQConsumer::cleanup()
{
    //*************************************************
    // Always close destination, consumers and producers before
    // you destroy their sessions and connection.
    //*************************************************

    // Destroy resources.
    try
    {
        if( destination != NULL ) delete destination;
    }
    catch (CMSException& e) {}
    destination = NULL;

    try
    {
        if( consumer != NULL ) delete consumer;
    }
    catch (CMSException& e) {}
    consumer = NULL;

    // Close open resources.
    try
    {
        if( session != NULL ) session->close();
        if( connection != NULL ) connection->close();
    }
    catch (CMSException& e) {}

    // Now Destroy them
    try
    {
        if( session != NULL ) delete session;
    }
    catch (CMSException& e) {}
    session = NULL;

    try
    {
        if( connection != NULL ) delete connection;
    }
    catch (CMSException& e) {}
    connection = NULL;
}

void cAMQConsumer::onException(const decaf::lang::Exception& ex)
{
    printf("Transport Exception occurred: %s \n", ex.getMessage().c_str());
}

void cAMQConsumer::transportInterrupted()
{
    std::cout << "The Connection's Transport has been Interrupted." << std::endl;
}

void cAMQConsumer::transportResumed()
{
    std::cout << "The Connection's Transport has been Restored." << std::endl;
}

void cAMQConsumer::onException(const CMSException& ex AMQCPP_UNUSED)
{
    printf("CMS Exception occurred.  Shutting down client.\n");
    exit(1);
}

