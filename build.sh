if [ -d build ]; then rm -Rf build; fi

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DKAHYPAR_USE_MINIMAL_BOOST=ON -DCMAKE_CXX_FLAGS="-fPIC"
make -j
