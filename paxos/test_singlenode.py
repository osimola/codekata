# Run this with pytest

from comm import * # includes np
from basicnode import * # includes req and resp

def test_propose_accept():
    c = Comm(5)
    c.delays = np.zeros((5, 5))
    node = PaxosNode(0, 5)

    c.send(1, 0, req.Propose(0, 'foo'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 1)
    assert(isinstance(rec[0][1], resp.Promise))

    c.send(1, 0, req.Accept(0, 'foo', 'bar'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 0)

    assert(node.get('foo') == 'bar')
 
def test_propose_reject():
    c = Comm(5)
    c.delays = np.zeros((5, 5))
    node = PaxosNode(0, 5)

    c.send(1, 0, req.Propose(1, 'foo'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 1)
    assert(isinstance(rec[0][1], resp.Promise))

    c.send(1, 0, req.Propose(0, 'foo'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 1)
    assert(isinstance(rec[0][1], resp.Reject))

def test_propose_override():
    c = Comm(5)
    c.delays = np.zeros((5, 5))
    node = PaxosNode(0, 5)

    c.send(1, 0, req.Propose(0, 'foo'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 1)
    assert(isinstance(rec[0][1], resp.Promise))

    c.send(1, 0, req.Propose(1, 'foo'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 1)
    assert(isinstance(rec[0][1], resp.Promise))

    # Late accept, should be ignored
    c.send(1, 0, req.Accept(0, 'foo', 'bar'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 0)
    assert(node.get('foo') is None)

    c.send(1, 0, req.Accept(1, 'foo', 'baz'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 0)
    assert(node.get('foo') == 'baz')

def test_late_proposal():
    # Accept arrives before proposal
    c = Comm(5)
    c.delays = np.zeros((5, 5))
    node = PaxosNode(0, 5)

    c.send(1, 0, req.Accept(0, 'foo', 'bar'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 0)
    assert(node.get('foo') == 'bar')

    c.send(1, 0, req.Propose(0, 'foo'))
    node.tick(c)
    rec = c.receive(1)
    assert(len(rec) == 1)
    assert(isinstance(rec[0][1], resp.Reject))

