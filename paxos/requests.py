from collections import namedtuple

class Propose(namedtuple('Propose', ['number', 'resource'])):
    def __repr__(self):
        return "ProposeRequest: {}({})".format(
            self.resource, self.number)

class Accept(namedtuple('Accept', ['number', 'resource','value'])):
    def __repr__(self):
        return "AcceptRequest: {}({}): {}".format(self.resource,
                                                      self.number, self.value)
