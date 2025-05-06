asn1SccSint entitylen_encode(asn1SccSint val) {
    return val - 1;
}

asn1SccSint entitylen_decode(asn1SccSint val) {
   return val + 1;
}