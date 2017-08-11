import qbs

Project {
    minimumQbsVersion: "1.7.1"

    Product {
        name: "fast-cpp-csv-parser"
        Export {
            Depends {name: "cpp"}
            cpp.includePaths: ["fast-cpp-csv-parser"]
        }
        files: [
            "fast-cpp-csv-parser/**",
            "protocols/338.csv",
        ]
    }

    Product {
        name: "rapidjson"
        Export {
            Depends {name: "cpp"}
            cpp.includePaths: ["rapidjson/include"]
        }
        files: ["rapidjson/**"]
    }

    Product {
        name: "spdlog"
        Export {
            Depends {name: "cpp"}
            cpp.includePaths: ["spdlog/include"]
        }
        files: ["spdlog/**"]
    }

    CppApplication {
        consoleApplication: true
        cpp.dynamicLibraries: ["ev", "crypto", "pthread", "m"]
        cpp.commonCompilerFlags: [
            "-Wno-unused-parameter",
        ]
        cpp.cxxLanguageVersion: "c++11"
        Depends {name: "fast-cpp-csv-parser"}
        Depends {name: "rapidjson"}
        Depends {name: "spdlog"}

        files: [
            "README.md",
            "logging.h",
            "main.cpp",
        ]

        Group {
            name: "Player"
            files: [
                "player.cpp",
                "player.h",
            ]
        }

        Group {
            name: "Packets"
            files: [
                "packet.cpp",
                "packet.h",
                "packets/*.h",
            ]
        }

        Group {
            name: "Network"
            files: [
                "network.cpp",
                "network.h",
                "handler.cpp",
                "handler.h",
                "client.cpp",
                "client.h",
                "client_play.cpp",
            ]
        }

        Group {
            name: "Server"
            files: [
                "server.cpp",
                "server.h",
                "status.cpp",
                "status.h",
                "tick.cpp",
                "tick.h",
            ]
        }

        Group {
            name: "Protocols"
            files: [
                "types.h",
                "chat.cpp",
                "chat.h",
                "protocols.cpp",
                "protocols.h",
                "protocols/id.h",
            ]
        }

        Group {
            name: "Protocol versions"
            qbs.install: true
            qbs.installDir: "protocols"
            files: [
                "protocols/*.csv",
            ]
        }

        Group {     // Properties for the produced executable
            fileTagsFilter: product.type
            qbs.install: true
        }
    }
}
