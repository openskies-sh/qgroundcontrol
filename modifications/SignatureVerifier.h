#ifndef SIGNATUREVERIFIER_H
#define SIGNATUREVERIFIER_H

#include <QObject>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/md5.h>
#include <openssl/des.h>
#include <openssl/pkcs12.h>
#include <jwt-cpp/jwt-cpp/jwt.h>
#include "cppcodec/base64_url_unpadded.hpp"
#include <GlobalDictionary.h>

namespace SignatureVerifier {
    bool convertJwkToPem(const std::string &strnn, const std::string& stree, const std::string& kid, std::string& strPubKey);
    int verifyJWT(const std::string &strJwtToken, const std::string& strPubKey);
}
#endif // SIGNATUREVERIFIER_H
