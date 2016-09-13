# A simple network simulator. Simulates delays and lost packets,
# because otherwise what's the point?

from collections import defaultdict
from collections import namedtuple
import heapq
import random
import numpy as np

class Comm(object):
    Message = namedtuple('Message', ['recvtime', 'sender', 'data'])

    def __init__(self, count):
        # Counter to simulate transport times
        self.recvtime = np.zeros(count, 'int64')
        # Receive queue for different clients
        self.recvq = defaultdict(list)

        # These can be modified later
        self.delays = np.ones((count, count))
        self.passprobs = np.ones((count, count))

    def send(self, sender, receiver, messages):
        if not isinstance(messages, list):
            messages = [messages];
        recvtime = self.recvtime[receiver]
        for msg in messages:
            # TODO: Variable delay... exponential or beta distribution?
            delay = self.delays[sender][receiver]
            if random.random() < self.passprobs[sender][receiver]:
                print("Send {}->{}: {}".format(sender, receiver, msg))
                heapq.heappush(self.recvq[receiver], Comm.Message(recvtime + delay, sender, msg))
            else:
                print("Drop {}->{}: {}".format(sender, receiver, msg))

    def receive(self, receiver):
        recvtime = self.recvtime[receiver]
        msgs = []
        recvq = self.recvq[receiver]

        while len(recvq) > 0 and recvq[0].recvtime == recvtime:
            msg = heapq.heappop(recvq)
            print("Recv {}->{}: {}".format(msg.sender, receiver, msg.data))
            msgs.append((msg.sender, msg.data))

        self.recvtime[receiver] = recvtime + 1
        return msgs;
