#pragma once

#include <QtCore>

#include <array>

enum CipherDir { ENCRYPTION, DECRYPTION };

class RawDes
{
public:
    RawDes();

    void RawSetKey(CipherDir direction, const quint8 *userKey);
    void RawProcessBlock(quint32 &l, quint32 &r) const;

protected:
    std::array<quint32, 32> k;

    static const quint32 Spbox[8][64];
};
