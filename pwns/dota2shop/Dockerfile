FROM ubuntu:16.04
MAINTAINER peternguyen
RUN apt-get update
RUN apt-get install xinetd -y
# RUN apt-get install libc6-dev -y
RUN useradd -m dota2shop
RUN chmod 774 /tmp
RUN chmod -R 774 /var/tmp
RUN chmod -R 774 /dev
RUN chmod -R 774 /run
RUN chmod 1733 /tmp /var/tmp /dev/shm

ADD dota2shop /home/dota2shop/dota2shop
ADD flag /home/dota2shop/
ADD dota2shop.xinetd /etc/xinetd.d/dota2shop

RUN chown -R root:dota2shop /home/dota2shop/
RUN chmod 750 /home/dota2shop/
RUN chown root:dota2shop /home/dota2shop/flag
RUN chmod 440 /home/dota2shop/flag

CMD ["/usr/sbin/xinetd","-dontfork"]
