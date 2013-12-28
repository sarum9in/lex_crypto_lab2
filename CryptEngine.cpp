#include "CryptEngine.hpp"

CryptEngine::CryptEngine(QObject *parent):
    QObject(parent)
{
}

void CryptEngine::crypt(const QString &key, const QString &iv, const QString &text)
{
    emit crypted("[" + text + "]");
}

void CryptEngine::decrypt(const QString &key, const QString &iv, const QString &cypherText)
{
    if (!cypherText.isEmpty() && cypherText.at(0) == '[' && cypherText.at(cypherText.size() - 1) == ']')
        emit decrypted(cypherText.mid(1, cypherText.size() - 2));
    else
        emit decrypted("ERROR");
}
