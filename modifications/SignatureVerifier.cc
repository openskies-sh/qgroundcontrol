#include "SignatureVerifier.h"

bool SignatureVerifier::convertJwkToPem(const std::string &strnn, const std::string& stree, const std::string& kid, std::string& strPubKey)
{
    auto nn = cppcodec::base64_url_unpadded::decode(strnn);
    auto ee = cppcodec::base64_url_unpadded::decode(stree);
    auto dd = cppcodec::base64_url_unpadded::decode(kid);

    BIGNUM * modul = BN_bin2bn(nn.data(),nn.size(), NULL);
    BIGNUM * expon = BN_bin2bn(ee.data(),ee.size(), NULL);
    BIGNUM * kids   = BN_bin2bn(dd.data(),dd.size(), NULL);
    RSA * rr = RSA_new();
    EVP_PKEY * pRsaKey = EVP_PKEY_new();
    RSA_set0_key(rr, modul, expon , kids);
    EVP_PKEY_assign_RSA(pRsaKey, rr);
    unsigned char * ss = new unsigned char[1024];
    unsigned char *desc = new unsigned char[1024];
    memset(desc, 0, 1024);

    BIO* bio = BIO_new(BIO_s_mem());
    PEM_write_bio_RSA_PUBKEY(bio, rr);
    BIO_read(bio, desc, 1024);
    strPubKey = (char*)desc;
    BIO_free(bio);
    RSA_free(rr);
        delete[] ss;
        delete[] desc;
    if (strPubKey.empty()) {
        return false;
    }
    return true;
}

int SignatureVerifier::verifyJWT(const std::string &strJwtToken, const std::string &strPubKey)
{         
    std::error_code c;
    try {
        auto verify = jwt::verify().allow_algorithm(jwt::algorithm::rs256(strPubKey)).with_issuer(AerobridgeGlobals::jwtTokenIssuerUrl.toStdString());
        auto decoded = jwt::decode(strJwtToken);
        verify.verify(decoded, c);
    }
    catch (jwt::signature_verification_exception *e){

    }
    catch (...)
    {
     return -1;
    }
    return c.value();
}
