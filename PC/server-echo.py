# test server, just simply echoes what is received from 3DS to stdout 

__author__ = 'Eric Ahn'

from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

class SocketServer(DatagramProtocol):

    def datagramReceived(self, data, (host, port)):
        print "received %r from %s:%d" % (data, host, port)

def main():
    reactor.listenUDP(50000, SocketServer())
    reactor.run()

if __name__ == '__main__':
    main()
