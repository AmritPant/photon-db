#include "aof.hpp"
#include "store.h"  // <-- Connects directly to get_store() definition
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

AOFManager::AOFManager() 
    : m_appendOnlyEnabled(false),
      m_appendFilename("appendonly.aof"),
      m_appendDirname("appendonlydir"),
      m_noAppendSyncOnRewrite(false),
      m_aofFileDescriptor(-1) {}

AOFManager::~AOFManager() {
    stop();
}

void AOFManager::initialize(bool appendOnly, const std::string& filename, const std::string& dirname) {
    m_appendOnlyEnabled = appendOnly;
    if (!filename.empty()) m_appendFilename = filename;
    if (!dirname.empty()) m_appendDirname = dirname;
}

bool AOFManager::start() {
    if (!m_appendOnlyEnabled) return true; 

    if (!createDirectory()) return false;
    if (!createManifest()) return false;

    std::string fullPath = m_appendDirname + "/" + m_appendFilename;
    m_aofFileDescriptor = open(fullPath.c_str(), O_WRONLY | O_APPEND | O_CREAT, 0644);
    
    if (m_aofFileDescriptor == -1) {
        std::cerr << "[AOF Error] Could not open append-only path: " << fullPath << std::endl;
        return false;
    }

    std::cout << "[AOF] Persistence logging active on " << fullPath << std::endl;
    return true;
}

void AOFManager::stop() {
    if (m_aofFileDescriptor != -1) {
        flushBuffer();
        close(m_aofFileDescriptor);
        m_aofFileDescriptor = -1;
    }
}

bool AOFManager::createDirectory() {
    struct stat info;
    if (stat(m_appendDirname.c_str(), &info) == 0) {
        return (info.st_mode & S_IFDIR); 
    }
    return mkdir(m_appendDirname.c_str(), 0755) == 0;
}

bool AOFManager::createManifest() {
    std::string manifestPath = m_appendDirname + "/appendonly.aof.manifest";
    std::ifstream check(manifestPath);
    if (check.good()) return true;

    std::ofstream manifest(manifestPath);
    if (!manifest.is_open()) return false;

    manifest << "file " << m_appendFilename << " seq 1 type i\n";
    return true;
}

bool AOFManager::isMutatingCommand(const std::string& command) {
    std::string upperCmd = command;
    std::transform(upperCmd.begin(), upperCmd.end(), upperCmd.begin(), ::toupper);
    if (upperCmd == "GET" || upperCmd == "PING" || upperCmd == "ECHO" || upperCmd == "INFO") {
        return false;
    }
    return true; 
}

std::string AOFManager::serializeToRESP(const std::vector<std::string>& args) {
    std::ostringstream resp;
    resp << "*" << args.size() << "\r\n";
    for (const auto& arg : args) {
        resp << "$" << arg.length() << "\r\n" << arg << "\r\n";
    }
    return resp.str();
}

void AOFManager::appendCommand(const std::vector<std::string>& args) {
    if (!m_appendOnlyEnabled || args.empty() || m_aofFileDescriptor == -1) return;
    if (!isMutatingCommand(args[0])) return;

    m_writeBuffer += serializeToRESP(args);
    flushBuffer();
}

void AOFManager::flushBuffer() {
    if (m_writeBuffer.empty() || m_aofFileDescriptor == -1) return;

    ssize_t bytesWritten = write(m_aofFileDescriptor, m_writeBuffer.c_str(), m_writeBuffer.size());
    if (bytesWritten > 0) {
        m_writeBuffer.erase(0, bytesWritten);
    }
    if (!m_noAppendSyncOnRewrite) {
        fsync(m_aofFileDescriptor);
    }
}

bool AOFManager::replayLog() {
    if (!m_appendOnlyEnabled) return true;

    std::string fullPath = m_appendDirname + "/" + m_appendFilename;
    std::ifstream aofFile(fullPath, std::ios::binary);
    if (!aofFile.is_open()) return true;

    std::cout << "[AOF] Replaying transactions into memory..." << std::endl;

    // Fetch the reference to your active global map store
    auto &store = get_store();

    char token;
    while (aofFile >> token) {
        if (token != '*') continue;

        int arraySize;
        aofFile >> arraySize;
        aofFile.ignore(2); // Drop \r\n

        std::vector<std::string> cmdArgs;
        bool parseSuccess = true;

        for (int i = 0; i < arraySize; ++i) {
            char dollarSign;
            aofFile >> dollarSign;
            if (dollarSign != '$') { parseSuccess = false; break; }

            int stringLength;
            aofFile >> stringLength;
            aofFile.ignore(2); // Drop \r\n

            std::string argField(stringLength, ' ');
            aofFile.read(&argField[0], stringLength);
            aofFile.ignore(2); // Drop \r\n

            cmdArgs.push_back(argField);
        }

        if (parseSuccess && !cmdArgs.empty()) {
            std::string cmd = cmdArgs[0];
            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);

            if (cmd == "SET" && cmdArgs.size() >= 3) {
                // Instantiates a clean Entry structure and loads it back to your global map
                Entry newEntry;
                newEntry.value = cmdArgs[2];
                newEntry.expiry = std::nullopt; // Clear expiration on reboot or parse custom logs
                store[cmdArgs[1]] = newEntry;
            } 
            else if (cmd == "DEL" && cmdArgs.size() >= 2) {
                store.erase(cmdArgs[1]);
            }
        }
    }

    std::cout << "[AOF] Recovery sequence completed successfully." << std::endl;
    return true;
}