#! /bin/bash

docker run -d -p 8080:8080 --env-file=env_template wp3_services
