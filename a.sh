      wget ftp://ftp.gnu.org/gnu/gcc/gcc-4.8.5/gcc-4.8.5.tar.gz
      tar -xzf gcc-4.8.5.tar.gz
      cd gcc-4.8.5
      ./contrib/download_prerequisites
      cd ..
      mkdir gcc-4.8.5-build
      cd gcc-4.8.5-build
      ../gcc-4.8.5/configure --prefix=/usr/local/gcc --enable-languages=c,c++ --build=x86_64-linux --disable-multilib
      LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib"
      make -j8
      make install
