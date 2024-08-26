#ifndef SISO2_CONFIG_H
#define SISO2_CONFIG_H

#include <string>
#include <stdint.h>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <fstream>
#include <magic_enum.hpp>
#include "phy/phy.h"
#include "dev/iq-device.h"

namespace SISO2
{
    struct Config
    {
    #ifdef SISO2_ADRV
        std::string log_dir = "/var/log/siso2";
        std::string config_path = "/etc/siso2.yaml";
        IqDevice::Device device = IqDevice::Device::ADRV;
        uint32_t udp_port = 4001;
        std::string udp_remote = "localhost:4001";
        std::string tx_port = "A";
        double tx_rate = 50.0e6;
        double tx_bw = 50.0e6;
        double tx_freq = 907e6;
        double tx_gain = -18.0;
        uint32_t tx_interpolation = 1;
        double tx_duty_cycle = 0.5;
        Constellation tx_modulation = Constellation::_64APSK;
        std::string rx_port = "A_BALANCED";
        double rx_rate = 50.0e6;
        double rx_bw = 0.4e6;
        double rx_freq = 925e6;
        double rx_gain = 40.0;
        uint32_t rx_decimation = 125;
        std::string rx_agc = "fast_attack";
        uint32_t xo_correction = 39999254;
        Constellation rx_modulation = Constellation::_64APSK; 
        uint16_t server_port = 80;
        std::string server_root = "/srv";
        std::string mode = "fdd";
        std::string ip = "10.0.0.1";
        uint32_t user_id = 0xFF;
    #else
        std::string log_dir = "logs";
        std::string config_path = "config.yaml";
        IqDevice::Device device = IqDevice::Device::USRP;
        uint32_t udp_port = 4001;
        std::string udp_remote = "localhost:4001";
        std::string tx_port = "TX/RX";
        double tx_rate = 0.4e6;
        double tx_bw = 0.4e6;
        double tx_freq = 925e6;
        double tx_gain = 65.0;
        uint32_t tx_interpolation = 1;
        double tx_duty_cycle = 0.5;
        Constellation tx_modulation = Constellation::_64APSK;
        std::string rx_port = "RX2";
        double rx_rate = 50.0e6;
        double rx_bw = 50.0e6;
        double rx_freq = 907e6;
        double rx_gain = 40.0;
        uint32_t rx_decimation = 1;
        std::string rx_agc = "manual";
        Constellation rx_modulation = Constellation::_64APSK; 
        uint16_t server_port = 4999;
        std::string server_root = "./www";
        std::string mode = "fdd";
        std::string ip = "10.0.0.2";
        uint32_t user_id = 0x02;
    #endif

        Config(std::string config_path)
        {
            YAML::Node yaml;
            if (std::filesystem::exists(config_path)) {
                spdlog::info("Loading config from {}", config_path);
                yaml = YAML::LoadFile(config_path);
                if (yaml["Mode"]) mode = yaml["Mode"].as<std::string>();
                if (yaml["Log"]["dir"]) log_dir = yaml["Log"]["dir"].as<std::string>();
                if (yaml["Server"]["port"]) server_port = yaml["Server"]["port"].as<uint16_t>();
                if (yaml["Server"]["root"]) server_root = yaml["Server"]["root"].as<std::string>();
            #ifdef SISO2_ADRV
                if (yaml["RF"]["xo_correction"]) xo_correction = yaml["RF"]["xo_correction"].as<uint32_t>();
            #endif
                if (yaml["RF"]["device"]) {
                    auto val = magic_enum::enum_cast<IqDevice::Device>(yaml["RF"]["device"].as<std::string>());
                    if (val.has_value()) device = val.value();
                }
                if (yaml["RF"]["udp_port"]) udp_port = yaml["RF"]["udp_port"].as<uint32_t>();
                if (yaml["RF"]["udp_remote"]) udp_remote = yaml["RF"]["udp_remote"].as<std::string>();
                if (yaml["TX"]["port"]) tx_port = yaml["TX"]["port"].as<std::string>();
                if (yaml["TX"]["rate"]) tx_rate = yaml["TX"]["rate"].as<double>();
                if (yaml["TX"]["freq"]) tx_freq = yaml["TX"]["freq"].as<double>();
                if (yaml["TX"]["gain"]) tx_gain = yaml["TX"]["gain"].as<double>();
                if (yaml["TX"]["bw"]) tx_bw = yaml["TX"]["bw"].as<double>();
                if (yaml["RX"]["port"]) rx_port = yaml["RX"]["port"].as<std::string>();
                if (yaml["RX"]["rate"]) rx_rate = yaml["RX"]["rate"].as<double>();
                if (yaml["RX"]["freq"]) rx_freq = yaml["RX"]["freq"].as<double>();
                if (yaml["RX"]["gain"]) rx_gain = yaml["RX"]["gain"].as<double>();
                if (yaml["RX"]["bw"]) rx_bw = yaml["RX"]["bw"].as<double>();
                if (yaml["TX"]["interpolation"]) tx_interpolation = yaml["TX"]["interpolation"].as<uint32_t>();
                if (yaml["TX"]["duty_cycle"]) tx_duty_cycle = yaml["TX"]["duty_cycle"].as<double>();
                if (yaml["TX"]["modulation"]) {
                    auto val = magic_enum::enum_cast<Constellation>(yaml["TX"]["modulation"].as<std::string>());
                    if (val.has_value()) tx_modulation = val.value();
                }
                if (yaml["RX"]["decimation"]) rx_decimation = yaml["RX"]["decimation"].as<uint32_t>();
                if (yaml["RX"]["agc"]) rx_agc = yaml["RX"]["agc"].as<std::string>();
                if (yaml["RX"]["modulation"]) {
                    auto val = magic_enum::enum_cast<Constellation>(yaml["RX"]["modulation"].as<std::string>());
                    if (val.has_value()) rx_modulation = val.value();
                }
                if (yaml["Net"]["ip"]) ip = yaml["Net"]["ip"].as<std::string>();
                if (yaml["Net"]["user_id"]) user_id = yaml["Net"]["user_id"].as<uint32_t>();
            } else {
                spdlog::warn("Config not found. Creating {}", config_path);
                load_defaults();
                save(config_path);
            }
        }

        void load_defaults()
        {
        }

        void save(std::string config_path)
        {
            YAML::Node yaml;
            yaml["Mode"] = mode;
            yaml["Log"]["dir"] = log_dir;
            yaml["Server"]["port"] = server_port;
            yaml["Server"]["root"] = server_root;
        #ifdef SISO2_ADRV
            yaml["RF"]["xo_correction"] = xo_correction;
        #endif
            yaml["RF"]["device"] = std::string(magic_enum::enum_name<IqDevice::Device>(device));
            yaml["RF"]["udp_port"] = udp_port;
            yaml["RF"]["udp_remote"] = udp_remote;
            yaml["TX"]["port"] = tx_port;
            yaml["TX"]["rate"] = tx_rate;
            yaml["TX"]["freq"] = tx_freq;
            yaml["TX"]["gain"] = tx_gain;
            yaml["TX"]["bw"] = tx_bw;
            yaml["RX"]["port"] = rx_port;
            yaml["RX"]["rate"] = rx_rate;
            yaml["RX"]["freq"] = rx_freq;
            yaml["RX"]["gain"] = rx_gain;
            yaml["RX"]["bw"] = rx_bw;
            yaml["TX"]["interpolation"] = tx_interpolation;
            yaml["TX"]["duty_cycle"] = tx_duty_cycle;
            yaml["TX"]["modulation"] = std::string(magic_enum::enum_name<Constellation>(tx_modulation));
            yaml["RX"]["decimation"] = rx_decimation;
            yaml["RX"]["agc"] = rx_agc;
            yaml["RX"]["modulation"] = std::string(magic_enum::enum_name<Constellation>(rx_modulation));
            yaml["Net"]["ip"] = ip;
            yaml["Net"]["user_id"] = user_id;
            std::ofstream(config_path) << yaml;
        }
    };
}

#endif