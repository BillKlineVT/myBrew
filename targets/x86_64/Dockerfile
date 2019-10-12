FROM ubuntu:bionic

USER root

ADD ../../myBrewLib/libmyBrewLib* /opt/myBrew/lib
ADD ../../myBrewApp/myBrewApp /opt/myBrew
ADD ../../myBrewTests/myBrewTests /opt/myBrew
ADD ../../images/* /opt/myBrew/images
ADD ../../python/* /opt/myBrew/python
ADD ../../audio/* /opt/myBrew/audio

RUN mkdir /DB
VOLUME /DB

ENV LD_LIBRARY_PATH /opt/myBrew/lib/

WORKDIR /opt/myBrew
ENTRYPOINT ./myBrew