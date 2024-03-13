FROM phusion/baseimage:bionic-1.0.0
LABEL maintainer="dev@hellar.io,admin@hellar.io"

ARG USER_ID
ARG GROUP_ID

ENV HOME /hellar

# add user with specified (or default) user/group ids
ENV USER_ID ${USER_ID:-1000}
ENV GROUP_ID ${GROUP_ID:-1000}
RUN groupadd -g ${GROUP_ID} hellar
RUN useradd -u ${USER_ID} -g hellar -s /bin/bash -m -d /hellar hellar
RUN mkdir /hellar/.hellarcore
RUN chown hellar:hellar -R /hellar

COPY hellarcore-2.0.4.9-x86_64-linux-gnu.tar.gz /tmp/
RUN tar -xvf /tmp/hellarcore-2.0.4.9-x86_64-linux-gnu.tar.gz -C /tmp/
RUN cp /tmp/hellarcore*/bin/* /usr/local/bin
RUN rm -rf /tmp/hellarcore*

ADD ./bin /usr/local/bin
RUN chmod a+x /usr/local/bin/*

USER hellar

VOLUME ["/hellar"]

EXPOSE 7788 7778 27787 27788

WORKDIR /hellar

CMD ["hellar_oneshot"]
