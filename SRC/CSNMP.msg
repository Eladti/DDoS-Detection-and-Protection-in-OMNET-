message SNMPRequest {
    int requestId;
    string oid;      // object identifier
    int type;        // 0 = get, 1 = set
    string value;    // used for SET
}

message SNMPResponse {
    int requestId;
    string oid;
    string value;    // returned value
    int errorStatus; // 0 = ok, 1 = noSuchName, etc.
}
