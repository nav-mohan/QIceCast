
# Iceacast HTTP messages

## All good - Icecast sends these two in quick succession
"HTTP/1.1 100 Continue\r\nContent-Length: 0\r\n\r\n"
"HTTP/1.0 200 OK\r\n\r\n"

## Mountpoint in use
```
"HTTP/1.0 403 Forbidden\r\nServer: Icecast 2.4.4\r\nConnection: Close\r\nDate: Tue, 24 Jan 2023 05:46:52 GMT\r\nContent-Type: text/plain; charset=utf-8\r\nCache-Control: no-cache, no-store\r\nExpires: Mon, 26 Jul 1997 05:00:00 GMT\r\nPragma: no-cache\r\nAccess-Control-Allow-Origin: *\r\n\r\nMountpoint in use"
```

## Too may sources
```
"HTTP/1.0 403 Forbidden\r\nServer: Icecast 2.4.4\r\nConnection: Close\r\nDate: Tue, 24 Jan 2023 05:48:26 GMT\r\nContent-Type: text/plain; charset=utf-8\r\nCache-Control: no-cache, no-store\r\nExpires: Mon, 26 Jul 1997 05:00:00 GMT\r\nPragma: no-cache\r\nAccess-Control-Allow-Origin: *\r\n\r\ntoo many sources connected"
```

# Next steps

## 1. Triple or Circular-Buffer for `m_middleMan`
I took a shot at making a circular_buffer that inherited `QIODevice` and held an internal `char buffer[]`. It used a `QMutex` to lock upon read/write. I used `signals/slots` in a pull-mode i.e the encoder requests data from the `CircularBuffer` rather than a push-mode where the buffer pushes data into the encoder. I tried recording a mp3 audio but it sounded like the buffers were mixed up between past and present. Check out `circular_buffer_artifact.mp3` inside the `ideas/` folder. Some small segments repeat continuously. I can hear my voice but just small fragments of it repeated over and over mixed in with noise that sounded like a triangle wave. Overall, it didn't work. I hope it's just a problem in my implementation of the circulr buffer's `getData()` method. I hope it's just that the buffer is mixed up because I didn't handle the `if` conditions for `circularity` and `overtaking` properly. I hope it's not an issue with the underlying threads of `signals/slots`. 
In order to degub this I could try the following:  

1. Build a simpler app. Instead of an encoder just have a WAV file writer. And use the `CircularBuffer` to relay data. See if you still the artificats described above. 
2. Increase the buffer size of the `QAudioInput` to > 8192. Currently the encoder uses a `PCM_BUFFERSIZE` of 8192. 
3. I noticed that the encoder was repeatedly locking and attempting to pull data even tough it had just failed. So provide an `early-exit` if `m_head - tail < bytesRequested` using
    * A simple `if` condition in the `getData()` or
    * By usnig `condition_variables` instead of `mutex`. This will stop the `getData()` from attempting to pull data if there is not enough data in the buffer. 

## 2. GUI Development


## 3. Error Handling
1. Socket connection errors
2. Encoder setup errors 
3. Encoder processing errors
4. 

I would need the `theMiddleMan` because that is the PCM buffer and I need to encode that into Mpeg and draw it's levels. 
I wouldn't need the `WavFile` or `MpegFile`