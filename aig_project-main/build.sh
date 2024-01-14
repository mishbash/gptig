cd "$(dirname "$0")/src"
[ -d build ] || mkdir build
cd build
cmake ..
make
cp result ../..