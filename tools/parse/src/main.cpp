// SPDX-License-Identifier: MIT

#include <shigenoy/mocopi-receiver/Data.hpp>

#include <bit>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

#include <CLI/CLI.hpp>

int
main(int argc, char* argv[])
{
    try
    {
        CLI::App app;

        std::filesystem::path packet_file_path{};
        app.add_option("packet", packet_file_path, "raw packet file path");
        app.allow_windows_style_options();

        try
        {
            app.parse(argc, argv);
        }
        catch (const CLI::ParseError& e)
        {
            return app.exit(e);
        }

        if (std::filesystem::exists(packet_file_path))
        {
            std::ifstream ifs{ packet_file_path, std::ios::in | std::ios::binary };
            std::vector<char> buf;
            buf.resize(shigenoy::mocopi_receiver::Head::Size);
            ifs.read(buf.data(), buf.size());
            if (ifs)
            {
                const auto& head = std::bit_cast<shigenoy::mocopi_receiver::Head>(
                    *(reinterpret_cast<shigenoy::mocopi_receiver::Head*>(buf.data())));
                std::cout << head.length << "\t" << std::string_view{ head.type.data(), 4 }
                          << "\n\t";
                for (const auto& c : head.payload)
                {
                    std::cout << std::setbase(16) << static_cast<unsigned int>(c) << " ";
                }
                std::cout << "\n";
                {
                    const auto& ftyp = head.parseAs<shigenoy::mocopi_receiver::Ftyp>(0);
                    std::cout << ftyp.length << "\t" << std::string_view{ ftyp.type.data(), 4 }
                              << "\n\t";
                    std::cout << std::string_view{ ftyp.payload.data(), ftyp.payload.size() }
                              << "\n";
                }
                {
                    const auto& vrsn = head.parseAs<shigenoy::mocopi_receiver::Vrsn>(
                        shigenoy::mocopi_receiver::Ftyp::Size);
                    std::cout << vrsn.length << "\t" << std::string_view{ vrsn.type.data(), 4 }
                              << "\n\t";
                    std::cout << static_cast<int>(vrsn.payload[0]) << "\n";
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
