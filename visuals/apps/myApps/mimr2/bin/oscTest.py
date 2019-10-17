# Import needed modules from osc4py3
from osc4py3.as_eventloop import *
from osc4py3 import oscbuildparse

# Start the system.
osc_startup()

# Make client channels to send packets.
osc_udp_client("127.0.0.1", 9020, "mimr")

# Build a simple message and send it.
msg = oscbuildparse.OSCMessage("/chaos", "ibff", [1,True,0.3,0.41])
osc_send(msg, "mimr")


# Periodically call osc4py3 processing method in your event loop.
finished = False
while not finished:
    # You can send OSC messages from your event loop too…
    # …
    osc_process()
    # …

# Properly close the system.
osc_terminate()