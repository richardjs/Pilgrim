from subprocess import Popen, PIPE
from random import shuffle
import sys

def send(e, line):
    e.stdin.write(line + '\n')
    e.stdin.flush()

def listen(e):
    return e.stdout.readline().strip()

def send_and_listen(e, line):
    send(e, line)
    return listen(e)

if(len(sys.argv) != 3):
    print 'Usage: python play.py [engine 1 cmd] [engine 2 cmd]'
    sys.exit()

e1 = Popen(sys.argv[1].split(), stdin = PIPE, stdout = PIPE)
e2 = Popen(sys.argv[2].split(), stdin = PIPE, stdout = PIPE)

listen(e1)
listen(e2)

turns = [e1, e2]
shuffle(turns)

while send_and_listen(e1, 'getwinner') == '.':
    e = turns[0]
    o = turns[1]

    move = send_and_listen(e, 'think')
    print move
    send(e, 'move ' + move)
    send(o, 'move ' + move)
    
    turns = [o, e]

if send_and_listen(e1, 'getwinner') == 'w':
    print '1 wins'
else:
    print '2 wins'