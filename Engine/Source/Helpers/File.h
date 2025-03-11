//
// Created by lepag on 3/10/25.
//

#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <iostream>
#include <sstream>

namespace Trin::Helpers {
    class FileObject {
    public:
        FileObject(std::ifstream &fileif, std::ofstream &fileof):
        fileif(fileif), fileof(fileof)
        {}

        ~FileObject() {
            fileof.close();
            fileif.close();
        }

        [[nodiscard]] std::string text() const {
            std::stringstream stream;
            stream << fileif.rdbuf();
            return stream.str();
        }

        [[nodiscard]] int size() const {
            // Save current position
            const auto currentPos = fileif.tellg();

            // Go to end of file
            fileif.seekg(0, std::ios::end);

            // Get size at end position
            const size_t size = fileif.tellg();

            // Restore original position
            fileif.seekg(currentPos);

            return static_cast<int>(size);
        }

        void close() const {
            delete this;
        }

    private:
        std::ifstream &fileif;
        std::ofstream &fileof;
    };

    enum class FileType {
        Read,
        Write,
        Both
    };

    class File {
    public:
        static FileObject* file(const char *path, FileType type) {
            std::ifstream fileStream;
            std::ofstream fileOf;
            switch (type) {
                case FileType::Read:
                    fileStream = std::ifstream(path, std::ios::in);
                    break;
                case FileType::Write:
                    fileOf = std::ofstream(path, std::ios::out);
                    break;
                case FileType::Both:
                    fileStream = std::ifstream(path, std::ios::in | std::ios::out);
                    fileOf = std::ofstream(path, std::ios::in | std::ios::out);
                    break;
            }
            if (!fileStream || !fileStream.is_open() || (!fileOf || !fileOf.is_open())) {
                std::cerr << "Failed to open file: " << path << std::endl;
                return nullptr;
            }

            return new FileObject(fileStream, fileOf);
        }
    };
}

#endif //FILE_H
