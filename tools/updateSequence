#!/usr/bin/python
import sys
import pexpect
import pxssh
import argparse

def main():
    parser = argparse.ArgumentParser(description='Update node client code')
    parser.add_argument('-a', '--address', help='IPv4 address of node to update.')

    args = parser.parse_args()

    if( args.address == None ):
        print "ERROR: A node address argument is required. (--address or -a)."
        sys.exit(1)

    print "=== Updating sequence for " + args.address + " ==="

    login = "pi"
    pword = "raspberry"
    getSequenceCmd = 'scp ' + login + '@192.168.2.3:/home/pi/Projects/CrimsonRegiment/setup-files/client/etc/crled/sequence.xml /etc/crled/sequence.xml'

    p = pxssh.pxssh()
    p.login( args.address, login, pword )
    p.sendline('netstat -n -t')
    PROMPT = p.PROMPT

    p.expect(PROMPT)
    print "Netstat Done"
    print p.before

    p.sendline(getSequenceCmd)
    i=p.expect(['yes/no','password'])
    if(i==0):
        p.sendline('yes')
        p.expect('password')

    p.sendline(pword)
    p.expect( PROMPT )

    print "new sequence file copy done"
    print p.before

    p.sendline('sudo service cr-led-clientd restart')
    p.expect( PROMPT )

    print "client restart done"
    print p.before

    p.sendline('sudo service cr-led-clientd status')
    p.expect( PROMPT )

    print "client status done"
    print p.before

    p.sendline('exit');
    print 'Doner'

if __name__ == "__main__":
    main()

