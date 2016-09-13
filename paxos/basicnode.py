# Basic Paxos node. Proposer, acceptor and learner in the same
# package. Because I don't want to think about communication failures
# between acceptors and learners.

from collections import namedtuple
import requests as req
import responses as resp

class PaxosNode(object):
    Proposal = namedtuple('Proposal', ['number', 'value'])

    def __init__(self, id, count):
        self.id = id
        self.count = count
        self.minquorum = int((count + 1) / 2)
        self.activeproposals = {}
        self.cache = {}
        self.outgoing = {} # Resource -> accept count, proposal is stored in activeproposals

    # Nothing interesting in communication failing between client and
    # proposer.
    def get(self, resource):
        if resource in self.cache:
            return self.cache[resource].value
        else:
            return None

    # Nothing interesting in communication failing between client and
    # proposer. But, there could be error and timeout callbacks if
    # setting fails.
    def trySet(self, comm, resource, value):
        if resource in self.activeproposals:
            number = self.activeproposals[resource].number + 1
        elif resource in self.cache:
            number = self.cache[resource].number + 1
        else:
            number = 0

        self.activeproposals[resource] = PaxosNode.Proposal(number, value)
        msg = req.Propose(number, resource)

        # Proposer counts itself as having accepted
        self.outgoing[resource] = 1
        for i in range(self.count):
            if i != self.id:
                comm.send(self.id, i, msg)

    def _handle_propose(self, msg, sender, comm):
        if (msg.resource in self.activeproposals):
            prevnum = self.activeproposals[msg.resource]
            if msg.number > prevnum:
                self.activeproposals[msg.resource] = msg.number
                comm.send(self.id, sender, resp.Promise(msg.number, msg.resource))
            else:
                comm.send(self.id, sender, resp.Reject(msg.number, msg.resource))
        else:
            if msg.resource in self.cache:
                if msg.number > self.cache[msg.resource].number:
                    self.activeproposals[msg.resource] = msg.number
                    comm.send(self.id, sender, resp.Promise(msg.number, msg.resource))
                else:
                    comm.send(self.id, sender, resp.Reject(msg.number, msg.resource))
            else:
                self.activeproposals[msg.resource] = msg.number
                comm.send(self.id, sender, resp.Promise(msg.number, msg.resource))

    def _handle_accept(self, msg, sender, comm):
        if (msg.resource in self.activeproposals):
            proposal = self.activeproposals[msg.resource]
            if (msg.number >= proposal):
                self.cache[msg.resource] = PaxosNode.Proposal(msg.number, msg.value)
                del self.activeproposals[msg.resource]
        else:
            if msg.resource not in self.cache:
                self.cache[msg.resource] = PaxosNode.Proposal(msg.number, msg.value)
            else:
                if msg.number >= self.cache[msg.resource].number:
                    self.cache[msg.resource] = PaxosNode.Proposal(msg.number, msg.value)
                else:
                    print("Dropping outdated accept request")

    def _handle_promise(self, msg, sender, comm):
        # If promise appears after quorum has been found, it can be
        # discarded.
        if msg.resource in self.outgoing:
            assert(msg.resource in self.activeproposals)
            proposal = self.activeproposals[msg.resource]
            if msg.number == proposal.number:
                self.outgoing[msg.resource] += 1
                if self.outgoing[msg.resource] >= self.minquorum:
                    self.cache[msg.resource] = PaxosNode.Proposal(proposal.number, proposal.value)
                    for i in range(self.count):
                        if (i != self.id):
                            comm.send(self.id, i, req.Accept(msg.number, msg.resource, proposal.value))
                    # Now that quorum has been reached, rest of promises can just be ignored
                    del self.activeproposals[msg.resource]
                    del self.outgoing[msg.resource]
            elif msg.number > self.outgoing[msg.resource]:
                print("Node {}: Received future promise ({}: {})".
                      format(self.id, msg.resource, msg.number))
            else:
                print("Node {}: Received outdated prommise({}: {})".
                      format(self.id, msg.resource, msg.number))

    def _handle_reject(self, msg, sender, comm):
        if (msg.resource in self.outgoing):
            proposal = self.activeproposals[msg.resource]
            print("Node {}: got reject for {}({})".format(self.id, msg.resource, msg.number))
            if (msg.number == proposal.number):
                del self.activeproposals[msg.resource]
                del self.outgoing[msg.resource]

    def tick(self, comm):
        messages = comm.receive(self.id)
        for sender, msg in messages:
            if isinstance(msg, req.Propose):
                self._handle_propose(msg, sender, comm)
            elif isinstance(msg, req.Accept):
                self._handle_accept(msg, sender, comm)
            elif isinstance(msg, resp.Promise):
                self._handle_promise(msg, sender, comm)
            elif isinstance(msg, resp.Reject):
                self._handle_reject(msg, sender, comm)
                pass


