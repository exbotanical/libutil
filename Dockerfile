FROM ubuntu:20.04

RUN apt-get update && apt-get install -y git gcc make

COPY entrypoint.sh /entrypoint.sh
ENTRYPOINT ["/entrypoint.sh"]
