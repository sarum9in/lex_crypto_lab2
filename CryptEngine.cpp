#include "CryptEngine.hpp"

#include <QCryptographicHash>

CryptEngine::CryptEngine(QObject *parent):
    QObject(parent)
{
}

static QByteArray derive(const QString &derivationFunction, const QString &text, const int maxSize=0)
{
    QCryptographicHash::Algorithm algo;
    if (derivationFunction == "MD5")
        algo = QCryptographicHash::Md5;
    else if (derivationFunction == "SHA-256")
        algo = QCryptographicHash::Sha256;
    else if (derivationFunction == "SHA-512")
        algo = QCryptographicHash::Sha512;
    else
        algo = QCryptographicHash::Sha512;
    QByteArray derived = QCryptographicHash::hash(text.toUtf8(), algo);

    if (maxSize)
    {
        QByteArray result(maxSize, '\0');
        for (int i = 0; i < derived.size(); ++i)
        {
            // ^= is not supported
            const int targetPos = i % result.size();
            result[targetPos] = result[targetPos] ^ derived[i];
        }
        return result;
    }
    else
    {
        return derived;
    }
}

static QByteArray crypt(const QByteArray &key,
                        const QString &modeOfOperation,
                        const QByteArray &iv,
                        const QByteArray &text)
{
    return text;
}

static QByteArray decrypt(const QByteArray &key,
                          const QString &modeOfOperation,
                          const QByteArray &iv,
                          const QByteArray &cipherText)
{
    return cipherText;
}

void CryptEngine::crypt(const QString &keyDerivationFunction,
                        const QString &key,
                        const QString &modeOfOperation,
                        const QString &iv,
                        const QString &text)
{
    const QByteArray cipherText = ::crypt(
        derive(keyDerivationFunction, key),
        modeOfOperation,
        derive(keyDerivationFunction, iv),
        text.toUtf8()
    );
    emit crypted(QString(cipherText.toBase64()));
}

void CryptEngine::decrypt(const QString &keyDerivationFunction,
                          const QString &key,
                          const QString &modeOfOperation,
                          const QString &iv,
                          const QString &cipherText)
{
    const QByteArray text = ::decrypt(
        derive(keyDerivationFunction, key),
        modeOfOperation,
        derive(keyDerivationFunction, iv),
        QByteArray::fromBase64(cipherText.toUtf8())
    );
    emit decrypted(QString::fromUtf8(text));
}
