Paxos
=====

Because of reasons, I wanted to understand how Paxos algorithm
works. The problem is that while the algorithm may be simple, the
original paper from Lamport is incomprehensible and the followup
"Paxos made simple" is not either easy to follow. To figure out
things, I implemented the simplest possible version with fixed number
of nodes, no leader election, no separate keeping track of cluster
connectivity and no timeouts (failed proposals are detected by the
simple fact that they never succeed). The simple implementation also
expects that all nodes are reliable and communication has only
non-byzantine errors.

basicnode.py
------------

A basic node that implements proposer, acceptor and learner roles and
a cache for accepted values. Not yet accepted proposals are stored in
`activeproposals`. Accept counts for outgoing proposals are stored in
`outgoing`.

comm.py
-------

Datagram communication simulator between nodes, identified by
numbers. The interesting part is that transmit delays and probablities
for dropped data can be set from test runner.