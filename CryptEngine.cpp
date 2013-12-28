#include "CryptEngine.hpp"

CryptEngine::CryptEngine(QObject *parent):
    QObject(parent)
{
}

void CryptEngine::crypt(const QString &keyDerivationFunction,
                        const QString &key,
                        const QString &modeOfOperation,
                        const QString &iv,
                        const QString &text)
{
    emit crypted("[" + text + "]");
}

void CryptEngine::decrypt(const QString &keyDerivationFunction,
                          const QString &key,
                          const QString &modeOfOperation,
                          const QString &iv,
                          const QString &cipherText)
{
    if (!cipherText.isEmpty() && cipherText.at(0) == '[' && cipherText.at(cipherText.size() - 1) == ']')
        emit decrypted(cipherText.mid(1, cipherText.size() - 2));
    else
        emit decrypted("ERROR");
}
