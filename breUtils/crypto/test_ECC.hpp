#pragma once

#include "ECC.hpp"
#include <iostream>
#include <cassert>

#include "ECC.hpp"
#include <iostream>
#include <cassert>

void testECC() {
    ECC ecc1, ecc2;

    // Generate key pairs for both ECC instances
    assert(ecc1.generateKeyPair());
    assert(ecc2.generateKeyPair());

    // Get public keys
    std::string pubKey1 = ecc1.getPublicKey();
    std::string pubKey2 = ecc2.getPublicKey();

    // Ensure public keys are not empty
    assert(!pubKey1.empty());
    assert(!pubKey2.empty());

    // Generate shared secrets
    std::vector<uint8_t> sharedSecret1 = ecc1.GenerateSharedSecret(pubKey2);
    std::vector<uint8_t> sharedSecret2 = ecc2.GenerateSharedSecret(pubKey1);

    // Ensure shared secrets are not empty
    assert(!sharedSecret1.empty());
    assert(!sharedSecret2.empty());

    // Ensure shared secrets are equal
    assert(sharedSecret1 == sharedSecret2);

    std::cout << "ECC key generation and shared secret exchange test passed." << std::endl;
}

