set -e

sudo apt -qq update

# Install the compiler used for cross compiling
sudo apt-get -qq install gcc-arm-linux-gnueabihf -y > /dev/null

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
sudo apt-get -qq install libsqlite3-dev:armhf -y > /dev/null

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
sudo apt-get -qq install qemu-user -y > /dev/null
sudo apt-get -qq install qemu-user-binfmt -y > /dev/null
sudo apt-get -qq install sqlite3 -y > /dev/null
./run_e2e_tests.sh

# Deploy
APP_NAME="worktrack"
HASH=$(git rev-parse --short HEAD)
IMAGE_NAME="$DOCKER_REGISTRY_URL/$APP_NAME"

sed --in-place '/server/d' .dockerignore
docker build --tag "$IMAGE_NAME:$HASH" --tag "$IMAGE_NAME:latest" -f Dockerfile_run .

echo "$DOCKER_REGISTRY_PW" | docker login -u "$DOCKER_REGISTRY_USER" --password-stdin "$DOCKER_REGISTRY_URL"
docker push "$IMAGE_NAME:$HASH"
docker push "$IMAGE_NAME:latest"
