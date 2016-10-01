#!/usr/bin/python
import sys
import pexpect
import pxssh
import argparse

def main():
    parser = argparse.ArgumentParser(description='Update node client code')
    parser.add_argument('-a', '--address', help='IPv4 address of node to update.')
    parser.add_argument('-p', '--package', help='The package file to use for the update.')

    args = parser.parse_args()

    if( args.address == None ):
        print "ERROR: A node address argument is required. (--address or -a)."
        sys.exit(1)

    if( args.package == None ):
        print "ERROR: A node package argument is required. (--package or -p)."
        sys.exit(1)

    print "=== Start update of " + args.address + " with " + args.package

    login = "pi"
    pword = "raspberry"
    getPkgCmd = 'scp ' + login + '@192.168.2.3:/home/pi/Projects/CrimsonRegiment/led-client-deamon/build/' + args.package + ' .'
    installPkgCmd = 'sudo dpkg -i ' + args.package

    p = pxssh.pxssh()
    p.login( args.address, login, pword )
    p.sendline('netstat -n -t')
    PROMPT = p.PROMPT

    p.expect(PROMPT)
    print "Netstat Done"
    print p.before

    p.sendline( getPkgCmd )
    i=p.expect(['yes/no','password'])
    if(i==0):
        p.sendline('yes')
        p.expect('password')

    p.sendline(pword)
    p.expect( PROMPT )

    print "package fetch complete"
    print p.before

    p.sendline( installPkgCmd )
    p.expect( PROMPT, timeout=120 )

    print "package install done"
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

    print "=== Complete ==="

    sys.exit(0)

if __name__ == "__main__":
    main()

