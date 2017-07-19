FROM ubuntu:16.04
MAINTAINER peternguyen
RUN apt-get update
RUN apt-get install xinetd -y
# RUN apt-get install libc6-dev -y
RUN useradd -m brainfuck2
RUN chmod 774 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

ADD brainfuck /home/brainfuck2/brainfuck2
ADD flag /home/brainfuck2/
ADD brainfuck2.xinetd /etc/xinetd.d/brainfuck2

RUN chown -R root:brainfuck2 /home/brainfuck2/
RUN chmod 750 /home/brainfuck2/
RUN chown root:brainfuck2 /home/brainfuck2/flag
RUN chmod 440 /home/brainfuck2/flag

CMD ["/usr/sbin/xinetd","-dontfork"]
