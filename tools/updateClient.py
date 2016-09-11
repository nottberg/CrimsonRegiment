import pexpect
import pxssh

p = pxssh.pxssh()
p.login( "192.168.2.8", "pi", "raspberry" )
p.sendline('netstat -n -t')
PROMPT = p.PROMPT

p.expect(PROMPT)
print "Netstat Done"
print p.before

p.sendline('scp pi@192.168.2.3:/home/pi/Projects/CrimsonRegiment/led-client-deamon/build/cr-led-clientd-1.0.3-Linux.deb .')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "cr-led-clientd-1.0.3-Linux.deb fetch done"
print p.before

p.sendline('sudo dpkg -i cr-led-clientd-1.0.3-Linux.deb')
p.expect( PROMPT, timeout=120 )

print "cr led client install done"
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
