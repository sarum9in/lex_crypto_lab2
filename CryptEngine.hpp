#pragma once

#include <QObject>

class CryptEngine: public QObject
{
    Q_OBJECT
public:
    explicit CryptEngine(QObject *parent = 0);

signals:
    void crypted(const QString &t);
    void decrypted(const QString &t);

public slots:
    void crypt(const QString &key, const QString &iv, const QString &text);
    void decrypt(const QString &key, const QString &iv, const QString &cypherText);
};
