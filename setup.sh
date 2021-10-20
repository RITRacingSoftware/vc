git submodule update --init --recursive
docker build . -tvc
touch vc/libs/ecu-sim/libs/vector_blf/LICENSE.GPL-3.0