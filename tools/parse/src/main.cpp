// SPDX-License-Identifier: MIT

#include <shigenoy/mocopi-receiver/Parser.hpp>

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

            ifs.seekg(0, std::ios::end);
            buf.resize(ifs.tellg());
            ifs.seekg(0, std::ios::beg);

            ifs.read(buf.data(), buf.size());
            if (ifs)
            {
                if (shigenoy::mocopi_receiver::isBoneDefinition(buf))
                {
                    const auto& parsed = shigenoy::mocopi_receiver::parseBoneDefinition(buf);
                    for (const auto& bndt : parsed.bndt_)
                    {
                        const auto& bnid = bndt.parseAs<shigenoy::mocopi_receiver::Bnid>(0);
                        const auto& pbid = bndt.parseAs<shigenoy::mocopi_receiver::Pbid>(
                            shigenoy::mocopi_receiver::Bnid::Size);
                        const auto& tran = bndt.parseAs<shigenoy::mocopi_receiver::Tran>(
                            shigenoy::mocopi_receiver::Bnid::Size +
                            shigenoy::mocopi_receiver::Pbid::Size);
                        std::cout << "Bone#" << bnid.parseAs<std::uint16_t>(0);
                        std::cout << "\tParent#" << pbid.parseAs<std::uint16_t>(0);
                        std::cout << "\t(quat{X, Y, Z, W}, tran{X, Y, Z})=({";
                        std::cout << tran.parseAs<float>(0) << ", ";
                        std::cout << tran.parseAs<float>(4) << ", ";
                        std::cout << tran.parseAs<float>(8) << ", ";
                        std::cout << tran.parseAs<float>(12) << "}, {";
                        std::cout << tran.parseAs<float>(16) << ", ";
                        std::cout << tran.parseAs<float>(20) << ", ";
                        std::cout << tran.parseAs<float>(24) << "})\n";
                    }
                }
                else if (shigenoy::mocopi_receiver::isFrameData(buf))
                {
                    const auto& parsed = shigenoy::mocopi_receiver::parseFrameData(buf);
                    for (const auto& btdt : parsed.btdt_)
                    {
                        const auto& bnid = btdt.parseAs<shigenoy::mocopi_receiver::Bnid>(0);
                        const auto& tran = btdt.parseAs<shigenoy::mocopi_receiver::Tran>(
                            shigenoy::mocopi_receiver::Bnid::Size);
                        std::cout << "Bone#" << bnid.parseAs<std::uint16_t>(0);
                        std::cout << "\t(quat{X, Y, Z, W}, tran{X, Y, Z})=({";
                        std::cout << tran.parseAs<float>(0) << ", ";
                        std::cout << tran.parseAs<float>(4) << ", ";
                        std::cout << tran.parseAs<float>(8) << ", ";
                        std::cout << tran.parseAs<float>(12) << "}, {";
                        std::cout << tran.parseAs<float>(16) << ", ";
                        std::cout << tran.parseAs<float>(20) << ", ";
                        std::cout << tran.parseAs<float>(24) << "})\n";
                    }
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
