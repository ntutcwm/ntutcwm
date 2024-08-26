## Hi there 👋
# SISO2

## 安裝必要套件

### Ubuntu 22.04

1. 安裝基本必要套件
```bash
sudo apt install libnss3-tools
sudo dpkg -i libncurses5_6.4-2_amd64.deb
sudo dpkg -i libncurses5_6.4-2_amd64.deb
sudo add-apt-repository universe

sudo apt install git gcc g++ make cmake ccache pkg-config \
  libfmt-dev libspdlog-dev libyaml-cpp-dev libuhd-dev uhd-host \
  libabsl-dev libprotobuf-dev libprotoc-dev protobuf-compiler \
  libqt5webengine5 libqt5webenginecore5 libqt5webenginewidgets5 qtwebengine5-dev \
  qml-module-qtwebengine qml-module-qtquick2 qml-module-qtquick-controls \
  libzmq3-dev libpng-dev libfftw3-dev libboost-all-dev \
  libncurses5 libcxxopts-dev libssl-dev libmodbus-dev \
  wget tar nano vim libcap2-bin
```

2. 安裝 nodejs >= 18

如果曾經裝過 nlohmann-json3-dev，必須移除，以免跟 SISO2 裡面附的套件版本衝突
```bash
sudo apt remove nlohmann-json3-dev
sudo snap install curl  # version 8.1.2


```
參考 https://github.com/nodesource/distributions 安裝 nodejs
```bash
sudo apt-get install -y ca-certificates curl gnupg
sudo mkdir -p /etc/apt/keyrings
curl -fsSL https://deb.nodesource.com/gpgkey/nodesource-repo.gpg.key | sudo gpg --dearmor -o /etc/apt/keyrings/nodesource.gpg
echo "deb [signed-by=/etc/apt/keyrings/nodesource.gpg] https://deb.nodesource.com/node_20.x nodistro main" | sudo tee /etc/apt/sources.list.d/nodesource.list
sudo apt-get update
sudo apt install -y nodejs
```

3. 下載 USRP 的韌體
```bash
sudo uhd_images_downloader
```

4. 設定 USRP 的權限，讓一般使用者（不用 sudo）就可以存取：
```bash
sudo cp /usr/lib/uhd/utils/uhd-usrp.rules /etc/udev/rules.d/10-uhd-usrp.rules
```

<!--
**ntutcwm/ntutcwm** is a ✨ _special_ ✨ repository because its `README.md` (this file) appears on your GitHub profile.

Here are some ideas to get you started:

- 🔭 I’m currently working on ...
- 🌱 I’m currently learning ...
- 👯 I’m looking to collaborate on ...
- 🤔 I’m looking for help with ...
- 💬 Ask me about ...
- 📫 How to reach me: ...
- 😄 Pronouns: ...
- ⚡ Fun fact: ...
-->
