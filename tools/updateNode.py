import pexpect
import pxssh

p = pxssh.pxssh()
p.login( "192.168.2.14", "pi", "raspberry" )
p.sendline('netstat -n -t')
PROMPT = p.PROMPT

p.expect(PROMPT)
print "Netstat Done"
print p.before

p.sendline('scp pi@192.168.2.7:/home/pi/ptpd_2.3.1-debian1-1_armhf.deb .')
i=p.expect(['yes/no','password'])
if(i==0):
  p.sendline('yes')
  p.expect('password')

p.sendline('raspberry')
p.expect( PROMPT )

print "PTPD fetch done"
print p.before

p.sendline('scp pi@192.168.2.7:/etc/default/ptpd .')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "/etc/default/ptpd fetch done"
print p.before

p.sendline('scp pi@192.168.2.7:/etc/ptpd.conf .')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "/etc/ptpd.conf fetch done"
print p.before

p.sendline('scp pi@192.168.2.7:/etc/init.d/ptpd ./ptpd.initd')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "/etc/init.d/ptpd fetch done"
print p.before

p.sendline('scp pi@192.168.2.7:/lib/dhcpcd/dhcpcd-hooks/51-ptpd.conf .')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "/lib/dhcpcd/dhcpcd-hooks/51-ptpd.conf fetch done"
print p.before

p.sendline('scp pi@192.168.2.7:/home/pi/Projects/CrimsonRegiment/led-client-deamon/build/cr-led-clientd-1.0.1-Linux.deb .')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "cr-led-clientd-1.0.1-Linux.deb fetch done"
print p.before

p.sendline('scp pi@192.168.2.7:/home/pi/cr-led-clientd.initd ./cr-led-clientd.initd')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "cr-led-clientd-1.0.1-Linux.deb fetch done"
print p.before

p.sendline('sudo apt-get -y install libpcap0.8')
p.expect( PROMPT, timeout=120 )

print "libpcap0.8 install done"
print p.before

p.sendline('sudo dpkg -i ptpd*.deb')
p.expect( PROMPT, timeout=120 )

print "ptpd install done"
print p.before

p.sendline('sudo dpkg -i cr-led*.deb')
p.expect( PROMPT, timeout=120 )

print "cr led client install done"
print p.before

p.sendline('sudo mv ptpd.conf /etc/ptpd.conf')
p.expect( PROMPT )

print "/etc/ptpd.conf move done"
print p.before

p.sendline('sudo mv ptpd /etc/default/ptpd')
p.expect( PROMPT )

print "/etc/default/ptpd move done"
print p.before

p.sendline('sudo mv ptpd.initd /etc/init.d/ptpd')
p.expect( PROMPT )

print "/etc/default/ptpd move done"
print p.before

p.sendline('sudo mv 51-ptpd.conf /lib/dhcpcd/dhcpcd-hooks/51-ptpd.conf')
p.expect( PROMPT )

print "51-ptpd.conf move done"
print p.before

p.sendline('sudo mv cr-led-clientd.initd /etc/init.d/cr-led-clientd')
p.expect( PROMPT )

print "/etc/init.d/cr-led-clientd move done"
print p.before

p.sendline('sudo update-rc.d cr-led-clientd defaults')
p.expect( PROMPT )

print "update-rc.d cr-led-clientd done"
print p.before

p.sendline('sudo mkdir /etc/crled')
p.expect( PROMPT )

print "/etc/crled created"
print p.before

p.sendline('sudo chmod og+rwx /etc/crled')
p.expect( PROMPT )

print "/etc/crled permissions changed"
print p.before

p.sendline('scp pi@192.168.2.7:/etc/crled/client.conf /etc/crled')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "/etc/crled/client.conf fetch done"
print p.before

p.sendline('sudo chmod og+rw /etc/crled/client.conf')
p.expect( PROMPT )

print "/etc/crled/client.conf permissions changed"
print p.before

p.sendline('scp pi@192.168.2.7:/etc/crled/sequence.xml /etc/crled')
p.expect('password')
p.sendline('raspberry')
p.expect( PROMPT )

print "/etc/crled/sequence.xml fetch done"
print p.before

p.sendline('sudo chmod og+rw /etc/crled/sequence.xml')
p.expect( PROMPT )

print "/etc/crled/sequence.xml permissions changed"
print p.before

p.sendline('exit');
print 'Doner'
