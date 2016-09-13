from collections import namedtuple

class Promise(namedtuple('Promise', ['number', 'resource'])):
    def __repr__(self):
        return "PromiseResponse: {}({})".format(self.resource, self.number)

class Reject(namedtuple('Reject', ['number', 'resource'])):
    def __repr__(self):
        return "RejectResponse: {}({})".format(self.resource, self.number)

# Accept is not responded to

