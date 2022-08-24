cmake . -Bbuild -DLIBWEBRTC_INCLUDE_PATH:PATH="{$PROJECT_DIR}webrtc-src/src" -DLIBWEBRTC_BINARY_PATH:PATH="{$PROJECT_DIR}/webrtc-src/src/out/m94/obj" -DOPENSSL_ROOT_DIR="/usr/local/Cellar/openssl@3/3.0.5/" -DOPENSSL_CRYPTO_LIBRARY="/usr/local/Cellar/openssl@3/3.0.5/lib/libcrypto.3.dylib" -DOPENSSL_INCLUDE_DIR:PATH="/usr/local/opt/openssl/include" -DCMAKE_USE_OPENSSL=ON
make -C build
# -DOPENSSL_ROOT_DIR="/usr/local/Cellar/openssl@3/3.0.4/"
# -DOPENSSL_CRYPTO_LIBRARY="/usr/local/Cellar/openssl@3/3.0.4/lib/libcrypto.3.dylib"
# -DOPENSSL_INCLUDE_DIR:PATH="/usr/local/opt/openssl/include"
