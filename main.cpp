#include <QApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlApplicationEngine engine(QUrl("qrc:/qml/main.qml"));

    QList<QObject *> rootObjects = engine.rootObjects();
    Q_ASSERT(rootObjects.size() == 1);
    QObject *const root = rootObjects.front();

    return app.exec();
}
