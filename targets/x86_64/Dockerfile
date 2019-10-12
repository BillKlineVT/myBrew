FROM ubuntu:bionic

USER root

RUN apt-get update && \
    apt-get -y install qt5-default && \
    apt-get -y install python && \
    apt-get install vim

ADD myBrewLib/libmyBrewLib* /opt/myBrew/lib/
ADD myBrewApp/myBrewApp /opt/myBrew/
ADD myBrewTests/myBrewTests /opt/myBrew/
ADD images/* /opt/myBrew/images/
ADD python/* /opt/myBrew/python/
ADD audio/* /opt/myBrew/audio/

RUN mkdir /DB
VOLUME /DB

ENV LD_LIBRARY_PATH /opt/myBrew/lib/

WORKDIR /opt/myBrew
ENTRYPOINT ./myBrewApp