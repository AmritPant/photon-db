#ifndef AOF_HPP
#define AOF_HPP

#include <string>
#include <vector>

class AOFManager {
public:
    static AOFManager& getInstance() {
        static AOFManager instance;
        return instance;
    }

    void initialize(bool appendOnly, const std::string& filename = "", const std::string& dirname = "");
    bool start();
    void stop();
    void appendCommand(const std::vector<std::string>& args);
    void flushBuffer();
    
    bool replayLog(); 

private:
    AOFManager();
    ~AOFManager();
    AOFManager(const AOFManager&) = delete;
    AOFManager& operator=(const AOFManager&) = delete;

    bool m_appendOnlyEnabled;
    std::string m_appendFilename;
    std::string m_appendDirname;
    bool m_noAppendSyncOnRewrite;
    int m_aofFileDescriptor;
    std::string m_writeBuffer;

    bool createDirectory();
    bool createManifest();
    bool isMutatingCommand(const std::string& command);
    std::string serializeToRESP(const std::vector<std::string>& args);
};

#endif