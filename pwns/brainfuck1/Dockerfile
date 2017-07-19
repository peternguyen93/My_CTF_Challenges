FROM ubuntu:16.04
MAINTAINER peternguyen
RUN apt-get update
RUN apt-get install xinetd -y
# RUN apt-get install libc6-dev -y
RUN useradd -m brainfuck1
RUN chmod 774 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

ADD brainfuck /home/brainfuck1/brainfuck1
ADD flag /home/brainfuck1/
ADD brainfuck1.xinetd /etc/xinetd.d/brainfuck1

RUN chown -R root:brainfuck1 /home/brainfuck1/
RUN chmod 750 /home/brainfuck1/
RUN chown root:brainfuck1 /home/brainfuck1/flag
RUN chmod 440 /home/brainfuck1/flag

CMD ["/usr/sbin/xinetd","-dontfork"]
