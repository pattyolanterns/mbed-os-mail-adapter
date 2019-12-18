# mbed-os-mail-adapter

The mail adapter is used to allow messages to be passed between producer threads and a consumer thread that is both thread safe and ISR safe.

One use case is to ensure serial coms are thread safe (so different threads do not intertwine printf statements), but it also supports printf statements from an ISR.

I have also found this adapter very useful in event based systems, were events are generated from various ISR's.
