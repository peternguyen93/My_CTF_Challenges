FROM ubuntu:16.04
MAINTAINER peternguyen
RUN apt-get update
RUN apt-get install xinetd -y
# RUN apt-get install libc6-dev -y
RUN useradd -m oldschool
RUN chmod 774 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

ADD oldschool /home/oldschool/oldschool
ADD flag /home/oldschool/
ADD oldschool.xinetd /etc/xinetd.d/oldschool

RUN chown -R root:oldschool /home/oldschool/
RUN chmod 750 /home/oldschool/
RUN chown root:oldschool /home/oldschool/flag
RUN chmod 440 /home/oldschool/flag

CMD ["/usr/sbin/xinetd","-dontfork"]
