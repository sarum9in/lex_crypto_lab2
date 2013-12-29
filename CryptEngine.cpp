#include "CryptEngine.hpp"

#include <QCryptographicHash>
#include <QtEndian>

CryptEngine::CryptEngine(QObject *parent):
    QObject(parent)
{
}

template<class T>
T rotr(T x, unsigned int moves)
{
    return (x >> moves) | (x << sizeof(T) * 8 - moves);
}
template<class T>
T rotl(T x, unsigned int moves)
{
    return (x << moves) | (x >> sizeof(T) * 8 - moves);
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

inline void initialPerm(quint32 &left, quint32 &right)
{
    quint32 work;

    work = ((left >> 4) ^ right) & 0x0f0f0f0f;
    right ^= work;
    left ^= work << 4;
    work = ((left >> 16) ^ right) & 0xffff;
    right ^= work;
    left ^= work << 16;
    work = ((right >> 2) ^ left) & 0x33333333;
    left ^= work;
    right ^= (work << 2);
    work = ((right >> 8) ^ left) & 0xff00ff;
    left ^= work;
    right ^= (work << 8);
    right = rotl(right, 1);
    work = (left ^ right) & 0xaaaaaaaa;
    left ^= work;
    right ^= work;
    left = rotl(left, 1);
}

inline void finalPerm(quint32 &left, quint32 &right)
{
    quint32 work;

    right = rotr(right, 1);
    work = (left ^ right) & 0xaaaaaaaa;
    left ^= work;
    right ^= work;
    left = rotr(left, 1);
    work = ((left >> 8) ^ right) & 0xff00ff;
    right ^= work;
    left ^= work << 8;
    work = ((left >> 2) ^ right) & 0x33333333;
    right ^= work;
    left ^= work << 2;
    work = ((right >> 16) ^ left) & 0xffff;
    left ^= work;
    right ^= work << 16;
    work = ((right >> 4) ^ left) & 0x0f0f0f0f;
    left ^= work;
    right ^= work << 4;
}

static void cryptBlock(const char *const key, const char *const src, char *const dst)
{
    quint32 left = *reinterpret_cast<const quint8 *>(src);
    quint32 right = *reinterpret_cast<const quint8 *>(src + 4);
    initialPerm(left, right);
}

static QByteArray crypt(const QByteArray &key,
                        const CryptEngine::ModeOfOperation &modeOfOperation,
                        QByteArray iv,
                        const QByteArray &text)
{
    Q_ASSERT(text.size() % 8 == 0);
    const int fullBlocks = text.size() / 8;
    QByteArray result(fullBlocks * 8, '\0');
    QByteArray input(8, '\0');
    QByteArray output(8, '\0');
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
        cryptBlock(key.data(), input.data(), output.data());
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

static void decryptBlock(const char *const key, const char *const src, char *const dst)
{
    for (int i = 0; i < 8; ++i)
        dst[i] = src[i] ^ key[i % 7];
}

static QByteArray decrypt(const QByteArray &key,
                          const CryptEngine::ModeOfOperation &modeOfOperation,
                          QByteArray iv,
                          const QByteArray &cipherText)
{
    Q_ASSERT(cipherText.size() % 8 == 0);
    const int fullBlocks = cipherText.size() / 8;
    QByteArray result(fullBlocks * 8, '\0');
    QByteArray input(8, '\0');
    QByteArray output(8, '\0');
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
        decryptBlock(key.data(), input.data(), output.data());
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
