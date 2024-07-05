FROM alpine:3.20.0 AS build

RUN apk update
RUN apk add --no-cache \
    libstdc++ \
    build-base \
    bash \
    zlib-dev \
    cmake \
    openjdk17-jdk

RUN addgroup -S usr && adduser -S usr -G usr
USER usr

WORKDIR /dt4dds-challenges
COPY src/ ./src
COPY tools/ ./tools
COPY scripts/ ./scripts
COPY Makefile .
RUN mkdir bin

RUN make linux
RUN make tools

RUN ln -s /dt4dds-challenges/bin/dt4dds-challenges /dt4dds-challenge/dt4dds-challenges