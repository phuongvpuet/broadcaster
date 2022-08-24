Fetch webrtc \
```bash
cd webrtc-src \
fetch --nohooks webrtc \
gclient sync \
cd src \
git checkout -b m94 refs/remotes/branch-heads/4606 \
gclient sync \
```
Environment variables:

* `SERVER_URL`: The URL of the mediasoup-demo HTTP API server (required).
* `ROOM_ID`: Room id (required).
* `USE_SIMULCAST`: If "false" no simulcast will be used (defaults to "true").
* `ENABLE_AUDIO`: If "false" no audio Producer is created (defaults to "true").
* `WEBRTC_DEBUG`: Enable libwebrtc logging. Can be "info", "warn" or "error" (optional).
* `VERIFY_SSL`: Verifies server side SSL certificate (defaults to "true") (optional).

## Dependencies

* [libmediasoupclient][libmediasoupclient] (already included in the repository)
* [cpr][cpr] (already included in the repository)
* OpenSSL (must be installed in the system including its headers)


## Installation

```bash
git clone https://github.com/versatica/mediasoup-broadcaster-demo.git

cmake . -Bbuild                                              \
  -DLIBWEBRTC_INCLUDE_PATH:PATH=${PATH_TO_LIBWEBRTC_SOURCES} \
  -DLIBWEBRTC_BINARY_PATH:PATH=${PATH_TO_LIBWEBRTC_BINARY}   \
  -DOPENSSL_INCLUDE_DIR:PATH=${PATH_TO_OPENSSL_HEADERS}      \
  -DCMAKE_USE_OPENSSL=ON

make -C build
```

#### Linkage Considerations (1)

```
[ 65%] Linking C shared library ../../../../lib/libcurl.dylib ld: cannot link directly with dylib/framework, your binary is not an allowed client of /usr/lib/libcrypto.dylib for architecture x86_64 clang: error: linker command failed with exit code 1 (use -v to see invocation)
make[2]: *** [lib/libcurl.dylib] Error 1 make[1]: *** [cpr/opt/curl/lib/CMakeFiles/libcurl.dir/all] Error 2
make: *** [all] Error 2
```

The following error may happen if the linker is not able to find the openssl crypto library. In order to avoid this error, specify the crypto library path along with the openssl root directory using the `OPENSSL_CRYPTO_LIBRARY` flag. Ie:

```
-DOPENSSL_ROOT_DIR=/usr/local/Cellar/openssl@1.1/1.1.1h \
-DOPENSSL_CRYPTO_LIBRARY=/usr/local/Cellar/openssl@1.1/1.1.1h/lib/libcrypto.1.1.dylib
```