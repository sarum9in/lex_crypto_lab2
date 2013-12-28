#include <QApplication>
#include <QQmlApplicationEngine>

#include "CryptEngine.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine(QUrl("qrc:/qml/main.qml"));

    CryptEngine cryptEngine;

    QList<QObject *> rootObjects = engine.rootObjects();
    Q_ASSERT(rootObjects.size() == 1);
    QObject *const root = rootObjects.front();

    QObject *const crypt = root->findChild<QObject *>("crypt");
    QObject *const decrypt = root->findChild<QObject *>("decrypt");

    QObject::connect(crypt, SIGNAL(crypt(QString,QString,QString)), &cryptEngine, SLOT(crypt(QString,QString,QString)));
    QObject::connect(&cryptEngine, SIGNAL(crypted(QString)), crypt, SIGNAL(crypted(QString)));

    QObject::connect(decrypt, SIGNAL(decrypt(QString,QString,QString)), &cryptEngine, SLOT(decrypt(QString,QString,QString)));
    QObject::connect(&cryptEngine, SIGNAL(decrypted(QString)), decrypt, SIGNAL(decrypted(QString)));

    return app.exec();
}
