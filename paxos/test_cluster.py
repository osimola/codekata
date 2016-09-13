# Run this with pytest

from comm import * # includes np
from basicnode import * # includes req and resp

def topleftones(onesize, totalsize):
    upper = np.concatenate((np.ones((onesize, onesize)),
                            np.zeros((onesize, totalsize - onesize))),
                           axis = 1)
    return np.concatenate((upper,
                          np.zeros((totalsize - onesize, totalsize))),
                          axis = 0)

def tick_all(nodes, comm, count = 1):
    for i in range(count):
        for node in nodes:
            node.tick(comm)

def test_trivial_success():
    c = Comm(5)
    c.delays = np.zeros((5, 5))
    nodes = [PaxosNode(i, 5) for i in range(5)]

    # Zero delay, no dropped messages
    nodes[0].trySet(c, 'foo', 'bar')

    tick_all(nodes, c, 2)
    for node in nodes:
        assert(node.get('foo') == 'bar')

    nodes[0].trySet(c, 'foo', 'baz')
    tick_all(nodes, c, 2)
    for node in nodes:
        assert(node.get('foo') == 'baz')

def test_out_of_order_success():
    c = Comm(5)
    c.delays = np.ones((5, 5)) * 5
    nodes = [PaxosNode(i, 5) for i in range(5)]

    # Delay of five
    nodes[0].trySet(c, 'foo', 'bar')
    tick_all(nodes, c)

    # Next request is faster
    c.delays = np.ones((5, 5)) * 1
    nodes[0].trySet(c, 'foo', 'baz')

    # Run until delayed messages have been handled
    tick_all(nodes, c, 10)
    for node in nodes:
        assert(node.get('foo') == 'baz')

def test_partition_success():
    c = Comm(5)
    c.passprobs = topleftones(3, 5);
    nodes = [PaxosNode(i, 5) for i in range(5)]

    nodes[0].trySet(c, 'foo', 'bar')
    tick_all(nodes, c, 10)

    for node in nodes[:3]:
        assert(node.get('foo') == 'bar')
    for node in nodes[3:]:
        assert(node.get('foo') is None)

def test_partition_fail():
    c = Comm(5)
    c.passprobs = topleftones(2, 5);
    nodes = [PaxosNode(i, 5) for i in range(5)]

    nodes[0].trySet(c, 'foo', 'bar')
    tick_all(nodes, c, 10)

    for node in nodes:
        assert(node.get('foo') is None)

def test_one_way_comm():
    c = Comm(5)
    # Traffic from upper to lower numbered nodes is blocked:
    # Since requests come from node 0, responses are blocked
    c.passprobs = np.triu(np.ones((5, 5)))
    nodes = [PaxosNode(i, 5) for i in range(5)]

    nodes[0].trySet(c, 'foo', 'bar')
    tick_all(nodes, c, 10)

    for node in nodes:
        assert(node.get('foo') is None)
