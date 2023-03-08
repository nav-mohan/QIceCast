## Next steps
### 1. __AAC Encoding__
### 2. __Network Statistics__
### 3. __Metadata __

----------------

## Iceacast HTTP messages

### 1. __OK: Icecast sends these two in quick succession__
```
"HTTP/1.1 100 Continue\r\nContent-Length: 0\r\n\r\n"
"HTTP/1.0 200 OK\r\n\r\n"
```

### 2. __Forbidden: Mountpoint in use__
```
"HTTP/1.0 403 Forbidden\r\nServer: Icecast 2.4.4\r\nConnection: Close\r\nDate: Tue, 24 Jan 2023 05:46:52 GMT\r\nContent-Type: text/plain; charset=utf-8\r\nCache-Control: no-cache, no-store\r\nExpires: Mon, 26 Jul 1997 05:00:00 GMT\r\nPragma: no-cache\r\nAccess-Control-Allow-Origin: *\r\n\r\nMountpoint in use"
```

### 3. __Forbidden: Too may sources__
```
"HTTP/1.0 403 Forbidden\r\nServer: Icecast 2.4.4\r\nConnection: Close\r\nDate: Tue, 24 Jan 2023 05:48:26 GMT\r\nContent-Type: text/plain; charset=utf-8\r\nCache-Control: no-cache, no-store\r\nExpires: Mon, 26 Jul 1997 05:00:00 GMT\r\nPragma: no-cache\r\nAccess-Control-Allow-Origin: *\r\n\r\ntoo many sources connected"
```


