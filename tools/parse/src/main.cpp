// SPDX-License-Identifier: MIT

#include <shigenoy/mocopi-receiver/Container.hpp>

#include <cstdint>
#include <filesystem>
#include <iostream>
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
            std::vector<std::byte> buf;

            ifs.seekg(0, std::ios::end);
            buf.resize(ifs.tellg());
            ifs.seekg(0, std::ios::beg);

            ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
            if (ifs)
            {
                const shigenoy::mocopi_receiver::ParsedMocopiPacket p{ buf };

                if (p.parsed_.contains(shigenoy::mocopi_receiver::wellknown_code::BNDT))
                {
                    // Bone Definition
                    const auto& [begin, end] =
                        p.parsed_.equal_range(shigenoy::mocopi_receiver::wellknown_code::BNDT);
                    for (auto itr = begin; itr != end; ++itr)
                    {
                        const auto& bndt = itr->second;
                        const auto& bnid =
                            bndt.children_
                                .equal_range(shigenoy::mocopi_receiver::wellknown_code::BNID)
                                .first->second;
                        const auto& pbid =
                            bndt.children_
                                .equal_range(shigenoy::mocopi_receiver::wellknown_code::PBID)
                                .first->second;
                        const auto& tran =
                            bndt.children_
                                .equal_range(shigenoy::mocopi_receiver::wellknown_code::TRAN)
                                .first->second;
                        std::cout << "Bone#" << bnid.get().readAs<std::uint16_t>(0);
                        std::cout << "\tParent#" << pbid.get().readAs<std::uint16_t>(0);
                        std::cout << "\t(quat{X, Y, Z, W}, tran{X, Y, Z})=({";
                        std::cout << tran.get().readAs<float>(0) << ", ";
                        std::cout << tran.get().readAs<float>(4) << ", ";
                        std::cout << tran.get().readAs<float>(8) << ", ";
                        std::cout << tran.get().readAs<float>(12) << "}, {";
                        std::cout << tran.get().readAs<float>(16) << ", ";
                        std::cout << tran.get().readAs<float>(20) << ", ";
                        std::cout << tran.get().readAs<float>(24) << "})\n";
                    }
                }
                else if (p.parsed_.contains(shigenoy::mocopi_receiver::wellknown_code::BTDT))
                {
                    // Frame Data
                    const auto& [begin, end] =
                        p.parsed_.equal_range(shigenoy::mocopi_receiver::wellknown_code::BTDT);
                    for (auto itr = begin; itr != end; ++itr)
                    {
                        const auto& bndt = itr->second;
                        const auto& bnid =
                            bndt.children_
                                .equal_range(shigenoy::mocopi_receiver::wellknown_code::BNID)
                                .first->second;
                        const auto& tran =
                            bndt.children_
                                .equal_range(shigenoy::mocopi_receiver::wellknown_code::TRAN)
                                .first->second;
                        std::cout << "Bone#" << bnid.get().readAs<std::uint16_t>(0);
                        std::cout << "\t(quat{X, Y, Z, W}, tran{X, Y, Z})=({";
                        std::cout << tran.get().readAs<float>(0) << ", ";
                        std::cout << tran.get().readAs<float>(4) << ", ";
                        std::cout << tran.get().readAs<float>(8) << ", ";
                        std::cout << tran.get().readAs<float>(12) << "}, {";
                        std::cout << tran.get().readAs<float>(16) << ", ";
                        std::cout << tran.get().readAs<float>(20) << ", ";
                        std::cout << tran.get().readAs<float>(24) << "})\n";
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
