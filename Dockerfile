FROM debian:latest as build
RUN apt update && apt -y install build-essential cmake git
ADD . build/
WORKDIR /build
RUN cmake . && make
RUN git clone https://github.com/sousmangoosta/frigate.git
WORKDIR /build/frigate
RUN git checkout adla_detector
FROM ghcr.io/blakeblackshear/frigate:stable-standard-arm64
RUN curl -LO https://github.com/numbqq/libadla_deb_aml/raw/main/jammy/arm64/libadla_0.4-202307_arm64.deb && \
    dpkg -i libadla_0.4-202307_arm64.deb && rm -f libadla_0.4-202307_arm64.deb
COPY --from=build build/libadla_interface.so /usr/lib/aarch64-linux-gnu/
COPY --from=build build/data/ssdlite_mobiledet_coco_int8_vim4.adla /
COPY --from=build build/frigate/frigate/detectors/plugins/adla.py /opt/frigate/frigate/detectors/plugins/
