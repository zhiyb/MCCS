import qbs

Project {
    minimumQbsVersion: "1.7.1"

    StaticLibrary {
        name: "fast-cpp-csv-parser"
        Export {
            Depends {name: "cpp"}
            cpp.includePaths: ["fast-cpp-csv-parser"]
        }

        files: [
            "fast-cpp-csv-parser/LICENSE",
            "fast-cpp-csv-parser/README.md",
            "fast-cpp-csv-parser/csv.h",
        ]
    }

    StaticLibrary {
        name: "rapidjson"
        Export {
            Depends {name: "cpp"}
            cpp.includePaths: ["rapidjson/include"]
        }

        files: []
    }

    StaticLibrary {
        name: "spdlog"
        Export {
            Depends {name: "cpp"}
            cpp.includePaths: ["spdlog/include"]
        }

        files: []
    }

    CppApplication {
        consoleApplication: true
        cpp.dynamicLibraries: ["ev", "crypto", "pthread", "m"]
        cpp.commonCompilerFlags: [
            "-Wno-unused-parameter"
        ]
        cpp.cxxLanguageVersion: "c++11"
        Depends {name: "fast-cpp-csv-parser"}
        Depends {name: "rapidjson"}
        Depends {name: "spdlog"}

        files: [
            "README.md",
            "chat.cpp",
            "chat.h",
            "client.cpp",
            "client.h",
            "client_play.cpp",
            "handler.cpp",
            "handler.h",
            "logging.h",
            "main.cpp",
            "network.cpp",
            "network.h",
            "packet.cpp",
            "packet.h",
            "player.cpp",
            "player.h",
            "protocols.cpp",
            "protocols.h",
            "server.cpp",
            "server.h",
            "status.cpp",
            "status.h",
            "tick.cpp",
            "tick.h",
            "types.h",
            "packets/*.h",
            "protocols/id.h",
        ]

        Group {
            name: "Protocols"
            qbs.install: true
            qbs.installDir: "protocols"
            files: ["protocols/*.csv"]
        }

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }
}
