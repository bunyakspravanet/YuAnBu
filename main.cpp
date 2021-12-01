#include <QCoreApplication>
#include <QObject>
#include "cppAMQ.h"
#include <iostream>
using namespace std;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    activemq::library::ActiveMQCPP::initializeLibrary();
    {
    std::cout << "=====================================================\\n";
    std::cout << "Starting the example:" << std::endl;
    std::cout << "-----------------------------------------------------\\n";


    // Set the URI to point to the IP Address of your broker.
    // add any optional params to the url to enable things like
    // tightMarshalling or tcp logging etc.  See the CMS web site for
    // a full list of configuration options.
    //
    //  http://activemq.apache.org/cms/
    //
    // Wire Format Options:
    // =========================
    // Use either stomp or openwire, the default ports are different for each
    //
    // Examples:
    //    tcp://127.0.0.1:61616                      default to openwire
    //    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
    //
    // SSL:
    // =========================
    // To use SSL you need to specify the location of the trusted Root CA or the
    // certificate for the broker you want to connect to.  Using the Root CA allows
    // you to use failover with multiple servers all using certificates signed by
    // the trusted root.  If using client authentication you also need to specify
    // the location of the client Certificate.
    //
    //     System::setProperty( "decaf.net.ssl.keyStore", "<path>/client.pem" );
    //     System::setProperty( "decaf.net.ssl.keyStorePassword", "password" );
    //     System::setProperty( "decaf.net.ssl.trustStore", "<path>/rootCA.pem" );
    //
    // The you just specify the ssl transport in the URI, for example:
    //
    //     ssl://localhost:61617
    //
    std::string brokerURI = "failover:(tcp://192.168.2.195:61616)";
//        "failover:(tcp://127.0.0.1:61616)";
    std::string destURI = "queue";
    //============================================================
    // set to true to use topics instead of queues
    // Note in the code above that this causes createTopic or
    // createQueue to be used in both consumer an producer.
    //============================================================
    bool useTopics = false;
    bool clientAck = false;
    //bool sessionTransacted = false;

    long long startTime = System::currentTimeMillis();

    cAMQProducer cProducer(brokerURI, destURI, useTopics, clientAck);
    cAMQConsumer cConsumer(brokerURI, destURI, useTopics, clientAck);

    // Start the consumer thread.
    Thread cConsumerThread(&cConsumer);
    cConsumerThread.start();

    // Wait for the consumer to indicate that its ready to go.
    while (cConsumer.ConsumerReady == 0) cout << "*";
    cout << endl;

    // Start the producer thread.
    Thread cProducerThread(&cProducer);
    cProducerThread.start();

    while (cProducer.ProducerReady == 0) cout << "-";
    cout << endl;

 //   QBson recmsq;
 //   QBson sendmsq;

    for (int i=0; i<10; i++)
    {
        cout << "try to send\n";
        cProducer.SendMessage = "Test Message " + to_string(i) + " in thread ";
        emit cProducer.MessToSendSig(cProducer.SendMessage, cProducer.threadIdStr);

        while (cConsumer.ReceiveText.size() == 0); // cout << "@"; cout << endl;
        cout << "Is received: " << cConsumer.ReceiveText.c_str() << endl;
        cConsumer.ReceiveText = "";
    }

    while(true);

    // Wait for the threads to complete.
    cProducerThread.join();
    cConsumerThread.join();

    long long endTime = System::currentTimeMillis();
    double totalTime = (double)(endTime - startTime) / 1000.0;



    cConsumer.close();
    cProducer.close();

    std::cout << "Time to completion = " << totalTime << " seconds." << std::endl;
    std::cout << "-----------------------------------------------------\\n";
    std::cout << "Finished with the example." << std::endl;
    std::cout << "=====================================================\\n";

    }
    activemq::library::ActiveMQCPP::shutdownLibrary();



    return a.exec();
}
