# libadla_interface

This project provides a dynamic library to instance and run an SSD model.

First use case is to deploy a ssdlite mobiledet model on frigate.

A docker image is provided for easy deployment.

Config elements to add in your frigate config file.
```
detectors:
  adla:
    type: adla
    model:
      path: /ssdlite_mobiledet_coco_int8_vim4.adla
```

Docker compose yml example:
```
services:
  frigate:
    container_name: frigate
    restart: unless-stopped
    image: ghcr.io/sousmangoosta/libadla_interface:latest
    depends_on:
      - mqtt
    devices:
      - /dev/adla0:/dev/adla0
    volumes:
      - ./frigate.yml:/config/config.yml
    privileged: true
```