namespace DSign {
    int sign_file(const char* xml_file, const char* key_file, const char* outfile);

    int signXML(const char* filename, const char* outfile);

    int verify_file(const char* xml_file, const char* key_file);

    int verifyXML(const char* filename);
}
