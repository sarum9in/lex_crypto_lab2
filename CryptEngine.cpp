#include "CryptEngine.hpp"

#include "RawDes.hpp"

#include <QCryptographicHash>
#include <QtEndian>
#include <QVector>

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

static CryptEngine::ModeOfOperation parseModeOfOperation(const QString &modeOfOperation)
{
    if (modeOfOperation == "ECB")
        return CryptEngine::ECB;
    else if (modeOfOperation == "CBC")
        return CryptEngine::CBC;
    else
        return CryptEngine::CBC;
}

static void process(
        RawDes &des,
        const char *const input,
        char *const output)
{
    quint32 left = *reinterpret_cast<const quint32 *>(input);
    quint32 right = *reinterpret_cast<const quint32 *>(input + 4);
    des.ProcessBlock(left, right);
    *reinterpret_cast<quint32 *>(output) = left;
    *reinterpret_cast<quint32 *>(output + 4) = right;
}

static QByteArray crypt(const QByteArray &key,
                        const CryptEngine::ModeOfOperation &modeOfOperation,
                        QByteArray iv,
                        const QByteArray &text)
{
    Q_ASSERT(text.size() % 8 == 0);
    Q_ASSERT(key.size() == 7);
    const int fullBlocks = text.size() / 8;
    QByteArray result(fullBlocks * 8, '\0');
    QByteArray input(8, '\0');
    QByteArray output(8, '\0');
    RawDes des;
    des.RawSetKey(ENCRYPTION, reinterpret_cast<const quint8 *>(key.data()));
    for (int i = 0; i < text.size(); i += 8)
    {
        switch (modeOfOperation)
        {
        case CryptEngine::ECB:
            for (int k = 0; k < 8; ++k)
                input[k] = text[i + k];
            break;
        case CryptEngine::CBC:
            for (int k = 0; k < 8; ++k)
                input[k] = iv[k] ^ text[i + k];
            break;
        }
        process(des, input.data(), output.data());
        switch (modeOfOperation) {
        case CryptEngine::ECB:
        case CryptEngine::CBC:
            for (int k = 0; k < 8; ++k)
                iv[k] = result[i + k] = output[k];
            break;
        }
    }
    return result;
}


static QByteArray decrypt(const QByteArray &key,
                          const CryptEngine::ModeOfOperation &modeOfOperation,
                          QByteArray iv,
                          const QByteArray &cipherText)
{
    Q_ASSERT(cipherText.size() % 8 == 0);
    Q_ASSERT(key.size() == 7);
    const int fullBlocks = cipherText.size() / 8;
    QByteArray result(fullBlocks * 8, '\0');
    QByteArray input(8, '\0');
    QByteArray output(8, '\0');
    RawDes des;
    des.RawSetKey(DECRYPTION, reinterpret_cast<const quint8 *>(key.data()));
    for (int i = 0; i < cipherText.size(); i += 8)
    {
        switch (modeOfOperation)
        {
        case CryptEngine::ECB:
        case CryptEngine::CBC:
            for (int k = 0; k < 8; ++k)
                input[k] = cipherText[i + k];
            break;
        }
        process(des, input.data(), output.data());
        switch (modeOfOperation)
        {
        case CryptEngine::ECB:
            for (int k = 0; k < 8; ++k)
                result[i + k] = output[k];
            break;
        case CryptEngine::CBC:
            for (int k = 0; k < 8; ++k)
            {
                result[i + k] = iv[k] ^ output[k];
                iv[k] = cipherText[i + k];
            }
            break;
        }
    }
    return result;
}

void CryptEngine::crypt(const QString &keyDerivationFunction,
                        const QString &key,
                        const QString &modeOfOperation,
                        const QString &iv,
                        const QString &text)
{
    QByteArray t = text.toUtf8();
    const quint32 size = qToBigEndian(t.size());
    t.prepend(reinterpret_cast<const char *>(&size), 4);
    while (t.size() % 8)
        t.push_back('\0');
    const QByteArray cipherText = ::crypt(
        derive(keyDerivationFunction, key, 7),
        parseModeOfOperation(modeOfOperation),
        derive(keyDerivationFunction, iv, 8),
        t
    );
    emit crypted(QString(cipherText.toBase64()));
}

void CryptEngine::decrypt(const QString &keyDerivationFunction,
                          const QString &key,
                          const QString &modeOfOperation,
                          const QString &iv,
                          const QString &cipherText)
{
    if (cipherText.size() < 4)
        emit decrypted("ERROR: too small");
    const QByteArray text = ::decrypt(
        derive(keyDerivationFunction, key, 7),
        parseModeOfOperation(modeOfOperation),
        derive(keyDerivationFunction, iv, 8),
        QByteArray::fromBase64(cipherText.toUtf8())
    );
    const quint32 size = qFromBigEndian(*reinterpret_cast<const quint32 *>(text.data()));
    emit decrypted(QString::fromUtf8(text.mid(4, size)));
}
