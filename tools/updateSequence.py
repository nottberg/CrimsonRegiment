import pexpect
import pxssh

p = pxssh.pxssh()
p.login( "192.168.2.8", "pi", "raspberry" )
p.sendline('netstat -n -t')
PROMPT = p.PROMPT

p.expect(PROMPT)
print "Netstat Done"
print p.before

p.sendline('scp pi@192.168.2.3:/home/pi/Projects/CrimsonRegiment/setup-files/client/etc/crled/sequence.xml /etc/crled/sequence.xml')
p.expect('password')
p.sendline('raspberry')
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
