# Wake on Lan Gateway
A simple HTTP server that emits WoL messages.

# Compile

## Dependencies

Alpine:
```
apk add musl-dev cmake make gcc inih-dev libmicrohttpd-dev
```

Debian:
```
apt install cmake make gcc libinih-dev libmicrohttpd-dev
```

## Compile

```
cmake .
make
```

# Run
```
./wakeonlangateway config.ini
```
TODO

# Usage

```
curl -X POST http://localhost:8080/wake/aa:bb:cc:11:22:33
```

TODO
