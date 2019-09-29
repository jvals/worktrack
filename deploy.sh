sudo apt -qq update

# Install the compiler used for cross compiling
sudo apt-get -qq install gcc-arm-linux-gnueabihf -y

# Update sources with armhf ports
cat << EOF | sudo tee /etc/apt/sources.list
deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ xenial main multiverse restricted universe
deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports/ xenial main multiverse restricted universe
deb [arch=armhf] http://ports.ubuntu.com/ubuntu-ports/ xenial-updates main multiverse restricted universe
deb [arch=amd64] http://archive.ubuntu.com/ubuntu/ xenial-updates main multiverse restricted universe
deb [arch=amd64] http://security.ubuntu.com/ubuntu/ xenial-security main multiverse restricted universe
EOF
sudo dpkg --add-architecture armhf
sudo apt-get -qq update

# We need the arm version of libsqlite3-dev
sudo apt-get -qq install libsqlite3-dev:armhf -y

# Make clogger
cd clogger
CC=arm-linux-gnueabihf-gcc CFLAGS='-O3 -Wall -Wextra -Wpedantic -march=armv7 -D LOG_ALL' make 
cd -

# Make project
CC=arm-linux-gnueabihf-gcc \
  LD=arm-linux-gnueabihf-gcc\
  LDFLAGS='-Lclogger -static -llogger -lsqlite3 -ldl -lpthread'\
  CFLAGS='-O3 -Wall -Wextra -Wpedantic -march=armv7 -std=gnu11 -D LOG_INFO -Iclogger'\
  make

# Test the arm binary
sudo apt-get -qq install qemu-user -y
sudo apt-get -qq install qemu-user-binfmt -y
sudo apt-get -qq install sqlite3 -y
./run_e2e_tests.sh

